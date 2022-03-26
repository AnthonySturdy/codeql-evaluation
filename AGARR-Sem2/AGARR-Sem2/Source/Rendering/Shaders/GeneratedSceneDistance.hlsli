float sdfSphere(float3 p, float3 param)
{
    return length(p) - param.x;
}

float GetDistanceToScene(float3 p, out int index)
{
    float dist = renderSettings.maxDist;

    int idx = 0;
    index = idx;

    dist = min(dist, sdfSphere(p, ObjectsList[0].Parameters));

    return dist;
}