float sdfSphere(float3 p, float3 param){
	return length(p) - param.x;
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
SceneDistanceInfo GetDistanceToScene(float3 p)
{
    float dist = renderSettings.maxDist;
    float prevDist = renderSettings.maxDist;
    int index = 0;
    int curIndex = 0;

	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[0].Position), ObjectsList[0].Rotation) / ObjectsList[0].Scale.x, ObjectsList[0].Parameters) * ObjectsList[0].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[1].Position), ObjectsList[1].Rotation) / ObjectsList[1].Scale.x, ObjectsList[1].Parameters) * ObjectsList[1].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;


    SceneDistanceInfo info;
    info.distance = dist;
    info.index = index;

    return info;
}