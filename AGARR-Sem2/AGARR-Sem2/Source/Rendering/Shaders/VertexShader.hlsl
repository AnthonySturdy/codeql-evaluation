cbuffer CameraConstantBuffer : register(b0)
{
    matrix View;
    matrix Projection;
}

cbuffer PerObjectConstantBuffer : register(b1)
{
    matrix World;
}
  
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

PS_INPUT main(VS_INPUT Input)
{
    PS_INPUT output = (PS_INPUT) 0;

    output.Pos = Input.Pos;
    output.TexCoord = Input.Tex;
    
	return output;
}