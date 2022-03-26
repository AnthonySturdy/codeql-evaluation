float sdfSphere(float3 p, float3 param){
	return length(p) - param.x;
}

float sdfTorus(float3 p, float3 param){
	float2 q = float2(length(p.xz) - param.x, p.y); return length(q) - param.y;
}

float GetDistanceToScene(float3 p, out int index)
{
    float dist = renderSettings.maxDist; 
	int objIndex = 0;

	dist = min(dist, sdfSphere(p - ObjectsList[0].Position, ObjectsList[0].Parameters));
	dist = min(dist, sdfTorus(p - ObjectsList[1].Position, ObjectsList[1].Parameters));

	index = objIndex; 
	return dist;
}