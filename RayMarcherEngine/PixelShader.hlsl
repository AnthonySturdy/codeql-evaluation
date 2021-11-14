cbuffer ConstantBuffer : register(b0) {
    struct WorldCamera
    {
        float fov;
        float3 position;    // 16 bytes
        float2 resolution;
        int cameraType;
        float padding;      // 32 bytes
        matrix view;        // 48 Bytes
    } camera;

    struct WorldObject
    {
        float3 position;
        float radius;       // 16 bytes
    } object;
}

struct PS_INPUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

float sdf(float3 p)
{
    return length(object.position - p) - object.radius;
}

bool RayMarch(float3 ro, float3 rd)
{
    float depth = 0.0f;
    for (int i = 0; i < 100; ++i) {
        float dist = sdf(ro + rd * depth);
        
        if (dist < 0.01f)
        {
            return true;
        }
        
        depth += dist;
    }
    
    return false;
}


float4 PS(PS_INPUT IN) : SV_TARGET
{
    float2 uv = (IN.Tex - 0.5f) * 2.0f * camera.fov;
    
    float3 ro = camera.position;    // Ray origin
    float3 rd = mul(normalize(float3(uv.x, uv.y, 1.0f)), (float3x4)camera.view);    // Ray direction

    float4 finalColour = float4(0.0f, 1.0f, 0.0f, 1.0f); // Sky color
    if (RayMarch(ro, rd))
    {
        finalColour.r = 1.0f;
    }
    
    return finalColour;
}