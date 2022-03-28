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
        int index;
        float curDist = GetDistanceToScene(ro + rd * ray.depth);

        // If distance less than threshold, ray has intersected
        if (curDist < renderSettings.intersectionThreshold)
        {
            ray.hit = true;
            ray.hitPosition = ro + rd * ray.depth;
            ray.hitNormal = CalculateNormal(ray.hitPosition);
                    
            return ray;
        }
        
        // Increment total depth by  distance to scene
        ray.depth += curDist;
        if (ray.depth > renderSettings.maxDist)
            break;
    }
    
    return ray;
}

float4 main(PS_INPUT Input) : SV_TARGET
{
    const float aspectRatio = renderSettings.resolution[0] / (float) renderSettings.resolution[1];
    float2 uv = Input.TexCoord;
    uv.y = 1.0f - uv.y; // Flip UV on Y axis
    uv = uv * 2.0f - 1.0f; // Move UV to (-1, 1) range
    uv.x *= aspectRatio; // Apply viewport aspect ratio
    
    float3 ro = camera.position; // Ray origin
    float3 rd = normalize(mul(transpose(camera.view), float4(uv, tan(-camera.fov), 0.0f)).xyz); // Ray direction

    float4 finalColour = float4(rd * .5 + .5, 1.0f); // Sky color
    Ray ray = RayMarch(ro, rd);
    if (ray.hit)
    {
        finalColour = float4(ray.hitNormal * .5 + .5, 1.0f);
    }

    //return float4(rd, 1.0f);
    return finalColour;
}