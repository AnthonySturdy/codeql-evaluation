float sdfSphere(float3 p, float3 param){
	return length(p) - param.x;
}

float sdfBox(float3 p, float3 param){
	float3 q = abs(p) - param.xyz; return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Transformation functions
float2x2 Rotate2D(float r)
{
    const float s = sin(r);
	const float c = cos(r);
    return float2x2(float2(c, -s),
					float2(s, c));
}

float3 Rotate(float3 p, float3 r)
{
    p.yz = mul(p.yz, Rotate2D(r.x));
    p.xz = mul(p.xz, Rotate2D(r.y));
    p.xy = mul(p.xy, Rotate2D(r.z));

    return p;
}

float3 Translate(float3 p, float3 t)
{
    return p - t;
}

// Distance function called from pixel shader
float GetDistanceToScene(float3 p)
{
    float dist = renderSettings.maxDist;

	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[0].Position), ObjectsList[0].Rotation) / ObjectsList[0].Scale.x, ObjectsList[0].Parameters) * ObjectsList[0].Scale.x);
	dist = min(dist, sdfBox(Rotate(Translate(p, ObjectsList[1].Position), ObjectsList[1].Rotation) / ObjectsList[1].Scale.x, ObjectsList[1].Parameters) * ObjectsList[1].Scale.x);

	return dist;
}