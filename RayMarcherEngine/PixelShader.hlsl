struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

cbuffer ConstantBuffer : register(b0) {
    struct RenderSettings
    {
        unsigned int maxSteps;
        float maxDist;
        float intersectionThreshold;
        float PADDING;      // 16 bytes
    } renderSettings;
    
    struct WorldCamera {
        float fov;
        float3 position;    // 16 bytes
        float2 resolution;
        int cameraType;
        float PADDING;      // 32 bytes
        matrix view;        // 48 Bytes
    } camera;

    struct WorldObject {
        float3 position;
        float radius;       // 16 bytes
    } object;
}

struct Ray {
    bool hit;
    float3 hitPosition;
    float3 hitNormal;
    float depth;
    uint stepCount;
};

float sdf(float3 p) {
    return min(length(float3(0.0f, 0.0f, 0.0f) - p) - (object.radius * 2.0f), length(object.position - p) - object.radius);
}

float3 CalculateNormal(float3 p) {
    const float3 offset = float3(0.001f, 0.0f, 0.0f);

    float3 normal = float3(sdf(p + offset.xyy) - sdf(p - offset.xyy),
                           sdf(p + offset.yxy) - sdf(p - offset.yxy),
                           sdf(p + offset.yyx) - sdf(p - offset.yyx));

    return normalize(normal);
}

Ray RayMarch(float3 ro, float3 rd) {
    // Initialise ray
    Ray ray;
    ray.hit = false;
    ray.hitPosition = float3(0.0f, 0.0f, 0.0f);
    ray.hitNormal = float3(0.0f, 0.0f, 0.0f);
    ray.depth = 0.0f;
    ray.stepCount = 0;
    
    // Step along ray direction
    float depth = 0.0f;
    for (uint i = 0; i < renderSettings.maxSteps; ++i) {
        float curDist = sdf(ro + rd * depth);
        
        // If distance less than threshold, ray has intersected
        if (curDist < renderSettings.intersectionThreshold)
        {
            ray.hit = true;
            ray.hitPosition = ro + rd * depth;
            ray.hitNormal = CalculateNormal(ray.hitPosition);
            ray.depth = depth;
            ray.stepCount = i;
            
            return ray;
        }
        
        // Increment total depth by  distance to scene
        depth += curDist;
        if (depth > renderSettings.maxDist)
            break;
    }
    
    return ray;
}


float4 PS(PS_INPUT IN) : SV_TARGET {
    float2 uv = (IN.Tex - 0.5f) * 2.0f;
    uv.y = 1.0f - uv.y;
    
    float3 ro = camera.position;    // Ray origin
    float3 rd = normalize(mul(float3(uv * camera.fov, 1.0f), (float3x4)camera.view)); // Ray direction

    float4 finalColour = float4(rd * .5 + .5, 1.0f); // Sky color
    Ray ray = RayMarch(ro, rd);
    if (ray.hit) {
        finalColour = float4(ray.stepCount / 50.0f, 0.0f, 0.0f, 1.0f);
    }
    
    return finalColour;
}