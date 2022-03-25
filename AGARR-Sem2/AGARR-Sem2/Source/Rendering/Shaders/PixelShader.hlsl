struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

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

#include "GeneratedSceneDistance.hlsli"

float4 main(PS_INPUT Input) : SV_TARGET
{
	return float4(Input.TexCoord, camera.fov, 1.0f);
}