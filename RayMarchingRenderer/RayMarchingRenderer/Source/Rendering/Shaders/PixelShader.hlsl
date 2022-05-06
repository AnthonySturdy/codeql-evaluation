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

float GetDistanceToScene(float3 p)
{
    return length(p) - 1.0f;
}

// Ray Marching
struct Ray
{
    bool hit;
    float3 hitPosition;
    float3 hitNormal;
    float depth;
    uint stepCount;
};

float3 CalculateNormal(float3 p)
{
    const float2 offset = float2(0.005f, 0.0f);

    int i = 0;
    float3 normal = float3(GetDistanceToScene(p + offset.xyy) - GetDistanceToScene(p - offset.xyy),
                           GetDistanceToScene(p + offset.yxy) - GetDistanceToScene(p - offset.yxy),
                           GetDistanceToScene(p + offset.yyx) - GetDistanceToScene(p - offset.yyx));

    return normalize(normal);
}

Ray RayMarch(float3 ro, float3 rd, RS rs)
{
    // Initialise ray
    Ray ray;
    ray.hit = false;
    ray.hitPosition = float3(0.0f, 0.0f, 0.0f);
    ray.hitNormal = float3(0.0f, 0.0f, 0.0f);
    ray.depth = 0.0f;
    ray.stepCount = 0;
    
    // Step along ray direction
    [loop]
    for (; ray.stepCount < rs.maxSteps; ++ray.stepCount)
    {
        float dist = GetDistanceToScene(ro + rd * ray.depth);

        // If distance less than threshold, ray has intersected
        if (dist < rs.intersectionThreshold)
        {
            ray.hit = true;
            ray.hitPosition = ro + rd * ray.depth;
            ray.hitNormal = CalculateNormal(ray.hitPosition);
                    
            return ray;
        }
        
        // Increment total depth by distance to scene
        ray.depth += dist;
        if (ray.depth > rs.maxDist)
            break;
    }
    
    return ray;
}

float ShadowMarch(float3 ro, int lightIdx)
{
    float result = 1.0f;

    const float3 rd = normalize(LightsList[lightIdx].Position.xyz);

    float depth = 0;
    [loop]
    for (int i = 0; i < renderSettings.maxSteps; ++i)
    {
        const float3 p = ro + rd * depth;
        const float distInfo = GetDistanceToScene(p);

        // If distance less than threshold, ray has intersected
        if (distInfo < renderSettings.intersectionThreshold)
            return 0.0f;

        // Soft shadowing
        result = min(result, LightsList[lightIdx].ShadowSharpness * distInfo / depth);
        
        // Increment total depth by distance to light
        depth += distInfo;
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
        const float diffuse = CalculateDiffuse(ray.hitNormal, normalize(LightsList[i].Position.xyz));

        float specular = 0.0f;
        float shadowAmount = 1.0f;
        if (diffuse > 0.0f)
        {
            specular = CalculateSpecular(rd, 
										reflect(ray.hitNormal, normalize(LightsList[i].Position.xyz)), 
										1.0f, 
										256.0f);
            shadowAmount = ShadowMarch(ray.hitPosition + ray.hitNormal * renderSettings.intersectionThreshold * 2.0f, i);
        }

        lightCol += LightsList[i].Colour * (diffuse * shadowAmount + specular);
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
        
        // Ambient Occlusion
        const float ao = 1.0f - float(ray.stepCount) / (renderSettings.maxSteps / renderSettings.AmbientOcclusionStrength);

        finalColour = float4((0.2f + lightCol) * ao, 1.0f);
    }

    output.Colour = finalColour;
    output.ReflectionColDepth = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.NormDepth = float4(ray.hitNormal * .5 + .5, ray.depth / renderSettings.maxDist);
    output.MetalicnessRoughness = float2(0.0f, 0.0f);
    return output;
}