float sdfSphere(float3 p, float3 param){
	return length(p) - param.x;
}

float GetDistanceToScene(float3 p, out int index)
{
    float dist = renderSettings.maxDist; 
	int objIndex = 0;

	dist = min(dist, sdfSphere(p - object[0].position, object[0].params));
	dist = min(dist, sdfSphere(p - object[1].position, object[1].params));

	index = objIndex; 
	return dist;
}