float sdfBox(float3 p, float3 param){
	float3 q = abs(p) - param.xyz; return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdfSphere(float3 p, float3 param){
	return length(p) - param.x;
}

// Transformation functions
float2x2 Rotate2D(const float r)
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

	dist = min(dist, sdfBox(Rotate(Translate(p, ObjectsList[0].Position), ObjectsList[0].Rotation) / ObjectsList[0].Scale.x, ObjectsList[0].Parameters) * ObjectsList[0].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfBox(Rotate(Translate(p, ObjectsList[1].Position), ObjectsList[1].Rotation) / ObjectsList[1].Scale.x, ObjectsList[1].Parameters) * ObjectsList[1].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfBox(Rotate(Translate(p, ObjectsList[2].Position), ObjectsList[2].Rotation) / ObjectsList[2].Scale.x, ObjectsList[2].Parameters) * ObjectsList[2].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfBox(Rotate(Translate(p, ObjectsList[3].Position), ObjectsList[3].Rotation) / ObjectsList[3].Scale.x, ObjectsList[3].Parameters) * ObjectsList[3].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfBox(Rotate(Translate(p, ObjectsList[4].Position), ObjectsList[4].Rotation) / ObjectsList[4].Scale.x, ObjectsList[4].Parameters) * ObjectsList[4].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[5].Position), ObjectsList[5].Rotation) / ObjectsList[5].Scale.x, ObjectsList[5].Parameters) * ObjectsList[5].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfSphere(Rotate(Translate(p, ObjectsList[6].Position), ObjectsList[6].Rotation) / ObjectsList[6].Scale.x, ObjectsList[6].Parameters) * ObjectsList[6].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;

	dist = min(dist, sdfBox(Rotate(Translate(p, ObjectsList[7].Position), ObjectsList[7].Rotation) / ObjectsList[7].Scale.x, ObjectsList[7].Parameters) * ObjectsList[7].Scale.x);
	index = lerp(index, curIndex, prevDist != dist);
	prevDist = dist;
	++curIndex;


    SceneDistanceInfo info;
    info.distance = dist;
    info.index = index;

    return info;
}