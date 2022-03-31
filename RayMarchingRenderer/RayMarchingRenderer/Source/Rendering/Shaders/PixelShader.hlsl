struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
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

        float3 PADDING;
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

		float3 PADDING;
	} ObjectsList[RAYMARCH_MAX_OBJECTS];
}

#define RAYMARCH_MAX_LIGHTS 10
cbuffer RayMarchLights : register(b3)
{
	struct Light
	{
		float4 Position;
		float3 Colour;
		float ConstantAttenuation;
		float LinearAttenuation;
		float QuadraticAttenuation;

		float2 PADDING;
	} LightsList[RAYMARCH_MAX_LIGHTS];
};

#include "GeneratedSceneDistance.hlsli"

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
    const float2 offset = float2(0.001f, 0.0f);

    float3 normal = float3(GetDistanceToScene(p + offset.xyy) - GetDistanceToScene(p - offset.xyy),
                           GetDistanceToScene(p + offset.yxy) - GetDistanceToScene(p - offset.yxy),
                           GetDistanceToScene(p + offset.yyx) - GetDistanceToScene(p - offset.yyx));

    return normalize(normal);
}

Ray RayMarch(float3 ro, float3 rd)
{
    // Initialise ray
    Ray ray;
    ray.hit = false;
    ray.hitPosition = float3(0.0f, 0.0f, 0.0f);
    ray.hitNormal = float3(0.0f, 0.0f, 0.0f);
    ray.depth = 0.0f;
    ray.stepCount = 0;
    
    // Step along ray direction
    for (; ray.stepCount < renderSettings.maxSteps; ++ray.stepCount)
    {
        float curDist = GetDistanceToScene(ro + rd * ray.depth);

        // If distance less than threshold, ray has intersected
        if (curDist < renderSettings.intersectionThreshold)
        {
            ray.hit = true;
            ray.hitPosition = ro + rd * ray.depth;
            ray.hitNormal = CalculateNormal(ray.hitPosition);
                    
            return ray;
        }
        
        // Increment total depth by distance to scene
        ray.depth += curDist;
        if (ray.depth > renderSettings.maxDist)
            break;
    }
    
    return ray;
}

float ShadowMarch(float3 ro, int lightIdx)
{
    float result = 1.0f;

    const float3 rd = normalize(LightsList[lightIdx].Position.xyz - ro);

    float depth = 0;
    for (int i = 0; i < renderSettings.maxSteps; ++i)
    {
        const float curDist = GetDistanceToScene(ro + rd * depth);

        // If able to become close to light, there is no shadow.
        if ((distance(ro + rd * depth, LightsList[lightIdx].Position.xyz) - 0.05f) < renderSettings.intersectionThreshold)
            break;

        // If distance less than threshold, ray has intersected
        if (curDist < renderSettings.intersectionThreshold)
            return 0.0f;

        result = min(result, 32 * curDist / depth);
        
        // Increment total depth by distance to light
        depth += curDist;
        if (depth > renderSettings.maxDist)
            break;
    }

    return result;
}

float3 CalculateLightColour(Ray ray)
{
    float3 lightCol = float3(0.0f, 0.0f, 0.0f);
    const float3 rd = normalize(ray.hitPosition - camera.position);

    [unroll(RAYMARCH_MAX_LIGHTS)]
    for (int i = 0; i < RAYMARCH_MAX_LIGHTS; ++i)
    {
        const float diffuse = saturate(dot(ray.hitNormal, normalize(LightsList[i].Position.xyz - ray.hitPosition)));

        float specular = 0.0f;
        if (diffuse > 0.0f)
            specular = saturate(1.0f * pow(dot(rd, reflect(ray.hitNormal, normalize(LightsList[i].Position.xyz - ray.hitPosition))), 32.0f));

        const float d = distance(ray.hitPosition, LightsList[i].Position.xyz);
        const float attentuation = 1.0f / (LightsList[i].ConstantAttenuation + LightsList[i].LinearAttenuation * d + LightsList[i].QuadraticAttenuation * d * d);

        float shadowAmount = 1.0f;
        if(i == 0)
			shadowAmount = ShadowMarch(ray.hitPosition + ray.hitNormal * renderSettings.intersectionThreshold * 2.0f, i);

        lightCol += LightsList[i].Colour * ((diffuse * shadowAmount + specular) * attentuation);
    }

    return lightCol;
}


float4 main(PS_INPUT Input) : SV_TARGET
{
    const float aspectRatio = renderSettings.resolution[0] / (float) renderSettings.resolution[1];
    float2 uv = Input.TexCoord;
    uv.y = 1.0f - uv.y; // Flip UV on Y axis
    uv = uv * 2.0f - 1.0f; // Move UV to (-1, 1) range
    uv.x *= aspectRatio; // Apply viewport aspect ratio
    
    const float3 ro = camera.position; // Ray origin
    const float3 rd = normalize(mul(transpose(camera.view), float4(uv, tan(-camera.fov), 0.0f)).xyz); // Ray direction

    float4 finalColour = float4(rd * .5 + .5, 1.0f); // Sky color
    Ray ray = RayMarch(ro, rd);
    if (ray.hit)
    {
        const float3 lightCol = CalculateLightColour(ray);
        //return float4(lightCol, 1.0f);
        finalColour = float4(((ray.hitNormal * .5 + .5) * (0.2f + lightCol)), 1.0f);
    }
    return finalColour;
}