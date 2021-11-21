#define OBJECT_COUNT 10

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

cbuffer ConstantBuffer : register(b0) {
    struct RenderSettings {
        unsigned int maxSteps;
        float maxDist;
        float intersectionThreshold;
        float PADDING;      // 16 bytes
    } renderSettings;
    
    struct WorldCamera {
        float fov;
        float3 position;    // 16 bytes
        uint2 resolution;
        int cameraType;
        float PADDING;      // 32 bytes
        matrix view;        // 48 Bytes
    } camera;

    struct WorldObject
    {
        bool isActive;
        float3 position;    // 16 bytes
        uint objectType;
        float3 params;      // 32 bytes
    } object[OBJECT_COUNT];
}

struct Ray {
    bool hit;
    uint hitObjectIndex;
    float3 hitPosition;
    float3 hitNormal;
    float depth;
    uint stepCount;
};

float sdCappedCylinder(float3 p, float h, float r)
{
    float2 d = abs(float2(length(p.xz), p.y)) - float2(h, r);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdCone(float3 p, float2 c, float h)
{
    float2 q = h * float2(c.x / c.y, -1.0);
    
    float2 w = float2(length(p.xz), p.y);
    float2 a = w - q * clamp(dot(w, q) / dot(q, q), 0.0, 1.0);
    float2 b = w - q * float2(clamp(w.x / q.x, 0.0, 1.0), 1.0);
    float k = sign(q.y);
    float d = min(dot(a, a), dot(b, b));
    float s = max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
    return sqrt(d) * sign(s);
}

float sdTorus(float3 p, float3 t)
{
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float sdBox(float3 p, float3 b) {
    float3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdSphere(float3 p, float s)
{
    return length(p) - s;
}

float GetDistanceToScene(float3 p, out int index) {
   float dist = renderSettings.maxDist;
   int objIndex = 0;
    
   for (uint i = 0; i < OBJECT_COUNT; ++i) {
       if(!object[i].isActive)
           break;
       
        float dists[5]; // Precalculate distances to avoid branching code
        dists[0] = sdSphere(p - object[i].position, object[i].params.x);
        dists[1] = sdBox(p - object[i].position, object[i].params);
        dists[2] = sdTorus(p - object[i].position, object[i].params);
        dists[3] = sdCone(p - object[i].position, object[i].params.xy, object[i].params.z);
        dists[4] = sdCappedCylinder(p - object[i].position, object[i].params.x, object[i].params.y);

        float prevDist = dist;
        dist = min(dist, dists[object[i].objectType]);
        objIndex = lerp(objIndex, i, prevDist != dist);
    }
    
    index = objIndex;
    
    return dist;
}

float3 CalculateNormal(float3 p) {
    const float3 offset = float3(0.001f, 0.0f, 0.0f);

    int index;
    float3 normal = float3(GetDistanceToScene(p + offset.xyy, index) - GetDistanceToScene(p - offset.xyy, index),
                           GetDistanceToScene(p + offset.yxy, index) - GetDistanceToScene(p - offset.yxy, index),
                           GetDistanceToScene(p + offset.yyx, index) - GetDistanceToScene(p - offset.yyx, index));

    return normalize(normal);
}

Ray RayMarch(float3 ro, float3 rd) {
    // Initialise ray
    Ray ray;
    ray.hit = false;
    ray.hitObjectIndex = 0;
    ray.hitPosition = float3(0.0f, 0.0f, 0.0f);
    ray.hitNormal = float3(0.0f, 0.0f, 0.0f);
    ray.depth = 0.0f;
    ray.stepCount = 0;
    
    // Step along ray direction
    float depth = 0.0f;
    for (uint i = 0; i < renderSettings.maxSteps; ++i) {
        int index;
        float curDist = GetDistanceToScene(ro + rd * depth, index);
        
        // If distance less than threshold, ray has intersected
        if (curDist < renderSettings.intersectionThreshold)
        {
            ray.hit = true;
            ray.hitObjectIndex = ceil(index);
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
    const float aspectRatio = camera.resolution.x / (float)camera.resolution.y;
    float2 uv = IN.Tex;
    uv.y = 1.0f - uv.y;     // Flip UV on Y axis
    uv = uv * 2.0f - 1.0f;  // Move UV to (-1, 1) range
    uv.x *= aspectRatio;    // Apply viewport aspect ratio
    
    float3 ro = camera.position;    // Ray origin
    float3 rd = normalize(mul(transpose(camera.view), float4(uv, tan(-camera.fov), 0.0f)).xyz); // Ray direction

    float4 finalColour = float4(rd * .5 + .5, 1.0f); // Sky color
    Ray ray = RayMarch(ro, rd);
    if (ray.hit) {
        finalColour = float4(ray.hitNormal * .5 + .5, 1.0f);
    }
    
    return finalColour;
}