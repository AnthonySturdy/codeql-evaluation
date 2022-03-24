struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PS_INPUT Input) : SV_TARGET
{
	return float4(Input.TexCoord, 1.0f, 1.0f);
}