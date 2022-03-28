float sdfCylinder(float3 p, float3 param){
	float2 d = abs(float2(length(p.xz), p.y)) - float2(param.x, param.y); return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
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

	dist = min(dist, sdfCylinder(Rotate(Translate(p, ObjectsList[0].Position), ObjectsList[0].Rotation) / ObjectsList[0].Scale.x, ObjectsList[0].Parameters) * ObjectsList[0].Scale.x);

	return dist;
}