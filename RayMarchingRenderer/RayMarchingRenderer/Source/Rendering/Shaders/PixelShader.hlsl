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

/////////////////
// SCENE DISTANCE
/////////////////

float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}

float noise(float3 x)
{
    // The noise function returns a value in the range -1.0f -> 1.0f

    float3 p = floor(x);
    float3 f = frac(x);

    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0 + 113.0 * p.z;

    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
                   lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
               lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
                   lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}

float GetDistanceToScene(float3 p)
{
    const float3 offset = float3(360, 0, 1800);
    p += offset;
    float h = noise(float3(p.x / 300, 0.0f, p.z / 300)) * 250;
    h += noise(float3(p.x / 200, 0.0f, p.z / 200)) * 70;
    h += noise(float3(p.x / 150, 0.0f, p.z / 150)) * 50;

    h += noise(float3(p.x / 40, 0.0f, p.z / 40)) * 10;

    return p.y + h - 100.0f;
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
        if (abs(dist) < rs.intersectionThreshold)
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

float ShadowMarch(float3 ro, float3 lightDir)
{
    float result = 1.0f;

    const float3 rd = normalize(lightDir);

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
        result = min(result, 200.0f * distInfo / depth);
        
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

    const float3 lightDir = float3(0.8f, 0.5f, -0.7f);

    const float diffuse = CalculateDiffuse(ray.hitNormal, normalize(lightDir));

    float specular = 0.0f;
    float shadowAmount = 1.0f;
    if (diffuse > 0.0f)
    {
        specular = CalculateSpecular(rd, 
									reflect(ray.hitNormal, normalize(lightDir)),
									1.0f, 
									256.0f);
        shadowAmount = ShadowMarch(ray.hitPosition + ray.hitNormal * renderSettings.intersectionThreshold * 2.0f, lightDir);
    }

    lightCol += LightsList[0].Colour * (diffuse * shadowAmount + specular);

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