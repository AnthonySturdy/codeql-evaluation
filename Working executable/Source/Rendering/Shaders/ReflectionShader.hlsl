Texture2D<float4> InColour : register(t0);
Texture2D<float4> InNormDepth : register(t1);
Texture2D<float4> InReflectionColDepth : register(t2);
Texture2D<float2> InMetalicnessRoughness : register(t3);

RWTexture2D<float4> Output : register(u0);

// Combines object colour with reflection colour.
//
// Reflection is blurred based on surface roughness
// using the gaussian blur algorithm from:
// https://xorshaders.weebly.com/tutorials/blur-shaders-5-part-2

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    Output[DTid.xy] = float4(InNormDepth[DTid.xy].rgb, 1.0f);

    const float PI2 = 6.283185f;

    const float size = 50.0f;
    const float directions = 16.0f;
    const float quality = 6.0f;

    float3 blurredReflection = InReflectionColDepth[DTid.xy].rgb;
    if (InMetalicnessRoughness[DTid.xy].x)
    {
        [unroll(directions)]
        for (float d = 0.0; d < PI2; d += PI2 / directions)
        {
            [unroll(quality)]
            for (float i = 1.0 / quality; i <= 1.0; i += 1.0 / quality)
            {
                float newSize = InMetalicnessRoughness[DTid.xy].g * size;
                float2 uv = DTid.xy + float2(cos(d), sin(d)) * newSize * i;
                blurredReflection += InReflectionColDepth[uv].rgb;
            }
        }

        blurredReflection /= quality * directions - 15.0f;
    }

    Output[DTid.xy] = float4(lerp(InColour[DTid.xy].rgb, 
								  lerp(InColour[DTid.xy].rgb, blurredReflection, 0.6f), 
								  InMetalicnessRoughness[DTid.xy].r), 
							 1.0f);
}