cbuffer ConstantBuffer : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

struct PS_INPUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


float4 PS(PS_INPUT IN) : SV_TARGET
{
    return float4(IN.Tex, 0, 1.0f);
}