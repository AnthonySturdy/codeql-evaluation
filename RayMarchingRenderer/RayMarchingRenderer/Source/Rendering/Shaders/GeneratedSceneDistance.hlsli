float sdfTorus(float3 p, float3 param){
	// Julia
float3 c = float3(3.0f, 3.0f, 3.0f);
float4 z = float4(p, 0.0f);
float md2 = 1;
float mz2 = dot(z, z);

[unroll(11)]
for(int i = 0; i < 11; i++)
{
    md2 *= 4.0 * mz2; // dz -> 2·z·dz, meaning |dz| -> 2·|z|·|dz| (can take the 4 out of the loop and do an exp2() afterwards)
    float4 qsqr = float4( z.x*z.x - z.y*z.y - z.z*z.z - z.w*z.w,
                2.0*z.x*z.y,
                2.0*z.x*z.z,
                2.0*z.x*z.w );
    z = qsqr + float4(param, 0.0f); // z  -> z^2 + c

    mz2 = dot(z,z);

    if(mz2 > 4.0) break;
}

return 0.25 * sqrt(mz2/md2) * log(mz2);
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

	dist = min(dist, sdfTorus(Rotate(Translate(p, ObjectsList[0].Position), ObjectsList[0].Rotation) / ObjectsList[0].Scale.x, ObjectsList[0].Parameters) * ObjectsList[0].Scale.x);
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