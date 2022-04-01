float sdfSphere(float3 p, float3 param){
	return length(p) - param.x;
}

float sdfTorus(float3 p, float3 param){
	float2 q = float2(length(p.xz) - param.x, p.y); return length(q) - param.y;
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
	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[1].Position), ObjectsList[1].Rotation) / ObjectsList[1].Scale.x, ObjectsList[1].Parameters) * ObjectsList[1].Scale.x);
	dist = min(dist, sdfTorus(Rotate(Translate(p, ObjectsList[2].Position), ObjectsList[2].Rotation) / ObjectsList[2].Scale.x, ObjectsList[2].Parameters) * ObjectsList[2].Scale.x);
	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[3].Position), ObjectsList[3].Rotation) / ObjectsList[3].Scale.x, ObjectsList[3].Parameters) * ObjectsList[3].Scale.x);
	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[4].Position), ObjectsList[4].Rotation) / ObjectsList[4].Scale.x, ObjectsList[4].Parameters) * ObjectsList[4].Scale.x);
	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[5].Position), ObjectsList[5].Rotation) / ObjectsList[5].Scale.x, ObjectsList[5].Parameters) * ObjectsList[5].Scale.x);
	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[6].Position), ObjectsList[6].Rotation) / ObjectsList[6].Scale.x, ObjectsList[6].Parameters) * ObjectsList[6].Scale.x);

	return dist;
}