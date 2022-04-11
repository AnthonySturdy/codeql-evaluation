TextureCube SkyboxTex : register(t0);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Colour;
    float4 NormDepth;
    float4 ReflectionColDepth;
    float2 MetalicnessRoughness;
};

// Constant Buffers
cbuffer RenderSettings : register(b0)
{
    struct RS
    {
        uint2 resolution;
        unsigned int maxSteps;
        float maxDist;
        float intersectionThreshold;
		float AmbientOcclusionStrength;


        float2 PADDING;
    } renderSettings;
}

cbuffer Camera : register(b1)
{
    struct WC
    {
        matrix view;
        float3 position;
        float fov;
    } camera;
}

#define RAYMARCH_MAX_OBJECTS 30
cbuffer RayMarchScene : register(b2)
{
	struct Object
	{
        float4 Position;
        float4 Rotation;
        float4 Scale;
		float3 Parameters;
		unsigned int SDFType;
		unsigned int BoolOperator;

        // Material
        float3 Colour;
        float Metalicness;
        float Roughness;

		float2 PADDING;
	} ObjectsList[RAYMARCH_MAX_OBJECTS];
}

#define RAYMARCH_MAX_LIGHTS 10
cbuffer RayMarchLights : register(b3)
{
	struct Light
	{
		float4 Position;
		float3 Colour;
        float ShadowSharpness;
		float ConstantAttenuation;
		float LinearAttenuation;
		float QuadraticAttenuation;

		float PADDING;
	} LightsList[RAYMARCH_MAX_LIGHTS];
};

struct SceneDistanceInfo
{
    float distance;
    int index;
};

#include "GeneratedSceneDistance.hlsli"

// Ray Marching
struct Ray
{
    bool hit;
    float3 hitPosition;
    float3 hitNormal;
    int hitIndex;
    float depth;
    uint stepCount;
};

float3 CalculateNormal(float3 p)
{
    const float2 offset = float2(0.005f, 0.0f);

    int i = 0;
    float3 normal = float3(GetDistanceToScene(p + offset.xyy).distance - GetDistanceToScene(p - offset.xyy).distance,
                           GetDistanceToScene(p + offset.yxy).distance - GetDistanceToScene(p - offset.yxy).distance,
                           GetDistanceToScene(p + offset.yyx).distance - GetDistanceToScene(p - offset.yyx).distance);

    return normalize(normal);
}

Ray RayMarch(float3 ro, float3 rd, RS rs)
{
    // Initialise ray
    Ray ray;
    ray.hit = false;
    ray.hitPosition = float3(0.0f, 0.0f, 0.0f);
    ray.hitNormal = float3(0.0f, 0.0f, 0.0f);
    ray.hitIndex = -1;
    ray.depth = 0.0f;
    ray.stepCount = 0;
    
    // Step along ray direction
    [loop]
    for (; ray.stepCount < rs.maxSteps; ++ray.stepCount)
    {
        SceneDistanceInfo distInfo = GetDistanceToScene(ro + rd * ray.depth);

        // If distance less than threshold, ray has intersected
        if (distInfo.distance < rs.intersectionThreshold)
        {
            ray.hit = true;
            ray.hitPosition = ro + rd * ray.depth;
            ray.hitNormal = CalculateNormal(ray.hitPosition);
            ray.hitIndex = distInfo.index;
                    
            return ray;
        }
        
        // Increment total depth by distance to scene
        ray.depth += distInfo.distance;
        if (ray.depth > rs.maxDist)
            break;
    }
    
    return ray;
}

float ShadowMarch(float3 ro, int lightIdx)
{
    float result = 1.0f;

    const float3 rd = normalize(LightsList[lightIdx].Position.xyz - ro);

    float depth = 0;
    [loop]
    for (int i = 0; i < renderSettings.maxSteps; ++i)
    {
        const float3 p = ro + rd * depth;
        const SceneDistanceInfo distInfo = GetDistanceToScene(p);

        // If ray is able to become close to light, there is no shadow.
        if (dot(normalize(LightsList[lightIdx].Position.xyz - p), rd) < 0)
            break;

        // If distance less than threshold, ray has intersected
        if (distInfo.distance < renderSettings.intersectionThreshold)
            return 0.0f;

        // Soft shadowing
        result = min(result, LightsList[lightIdx].ShadowSharpness * distInfo.distance / depth);
        
        // Increment total depth by distance to light
        depth += distInfo.distance;
        if (depth > renderSettings.maxDist)
            break;
    }

    return result;
}

float CalculateDiffuse(float3 n, float3 ld)
{
    return saturate(dot(n, ld));
}

float CalculateSpecular(float3 rd, float3 ref, float li, float s)
{
    return saturate(li * pow(dot(rd, ref), s));
}

float3 CalculateLightColour(Ray ray)
{
    float3 lightCol = float3(0.0f, 0.0f, 0.0f);
    const float3 rd = normalize(ray.hitPosition - camera.position);

    [unroll(RAYMARCH_MAX_LIGHTS)]
    for (int i = 0; i < RAYMARCH_MAX_LIGHTS; ++i)
    {
        const float diffuse = CalculateDiffuse(ray.hitNormal, normalize(LightsList[i].Position.xyz - ray.hitPosition));

        float specular = 0.0f;
        float shadowAmount = 1.0f;
        if (diffuse > 0.0f)
        {
            specular = CalculateSpecular(rd, 
										reflect(ray.hitNormal, normalize(LightsList[i].Position.xyz - ray.hitPosition)), 
										1.0f, 
										(1.0f - ObjectsList[ray.hitIndex].Roughness) * 256.0f + 2.0f);
            shadowAmount = ShadowMarch(ray.hitPosition + ray.hitNormal * renderSettings.intersectionThreshold * 2.0f, i);
        }

        const float d = distance(ray.hitPosition, LightsList[i].Position.xyz);
        const float attentuation = 1.0f / (LightsList[i].ConstantAttenuation + LightsList[i].LinearAttenuation * d + LightsList[i].QuadraticAttenuation * d * d);

        lightCol += LightsList[i].Colour * ((diffuse * shadowAmount + specular) * attentuation);
    }

    return lightCol;
}

float4 CalculateSkyColour(float3 dir)
{
    float2 uv = float2(atan2(dir.x, dir.z) / (2 * 3.142f) + 0.5,
					   dir.y * 0.5 + 0.5);


    return SkyboxTex.Sample(Sampler, dir);
}

PS_OUTPUT main(PS_INPUT Input) : SV_TARGET
{
    PS_OUTPUT output;

    const float aspectRatio = renderSettings.resolution[0] / (float) renderSettings.resolution[1];
    float2 uv = Input.TexCoord;
    uv.y = 1.0f - uv.y; // Flip UV on Y axis
    uv = uv * 2.0f - 1.0f; // Move UV to (-1, 1) range
    uv.x *= aspectRatio; // Apply viewport aspect ratio
    
    const float3 ro = camera.position; // Ray origin
    const float3 rd = normalize(mul(transpose(camera.view), float4(uv, tan(-camera.fov), 0.0f)).xyz); // Ray direction

    // Calculate sky colour
    float4 finalColour = CalculateSkyColour(rd);

    Ray ray = RayMarch(ro, rd, renderSettings);
    if (ray.hit)
    {
        const float3 lightCol = CalculateLightColour(ray);

        // Reflection
        RS rs = renderSettings; // render settings with lower fidelity
        rs.intersectionThreshold * 5.0f;
        rs.maxSteps /= 2;

        Ray refRay; // reflection ray
        refRay.hit = false;
        float3 refLight = float3(0.8f, 0.8f, 0.8f);
        if (ObjectsList[ray.hitIndex].Metalicness)
        {
            refRay = RayMarch(ray.hitPosition + (ray.hitNormal * rs.intersectionThreshold * 2.0f), reflect(rd, ray.hitNormal), rs);
            refLight = CalculateLightColour(refRay);
        }

        // Choose colour based on if reflection ray hit
        const float3 refCol = lerp(CalculateSkyColour(reflect(rd, ray.hitNormal)).rgb,
							ObjectsList[refRay.hitIndex].Colour,
							round(refRay.hit));
        
        output.ReflectionColDepth = float4(refCol * (0.2f + refLight), refRay.depth);

        // Ambient Occlusion
        const float ao = 1.0f - float(ray.stepCount) / (renderSettings.maxSteps / renderSettings.AmbientOcclusionStrength);

        finalColour = float4((ObjectsList[ray.hitIndex].Colour * (0.2f + lightCol) * ao), 1.0f);
    }

    output.Colour = finalColour;
    output.NormDepth = float4(ray.hitNormal * .5 + .5, ray.depth / renderSettings.maxDist);
    output.MetalicnessRoughness = float2(ObjectsList[ray.hitIndex].Metalicness, ObjectsList[ray.hitIndex].Roughness);
    return output;
}