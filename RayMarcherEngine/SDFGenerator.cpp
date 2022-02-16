#include "pch.h"
#include "SDFGenerator.h"
#include <filesystem>
#include <fstream>

std::string SDFGenerator::GetSceneDistanceFunction()
{
	return R"(float GetDistanceToScene(float3 p, out int index) {
   float dist = renderSettings.maxDist;
   int objIndex = 0;
    
   for (uint i = 0; i < OBJECT_COUNT; ++i) {
       if(!object[i].isActive)
           break;
       
        float dists[5]; // Precalculate distances to avoid branching code
        dists[0] = sdSphere(p - object[i].position, object[i].params.x);
        dists[1] = sdBox(p - object[i].position, object[i].params);
        dists[2] = sdTorus(p - object[i].position, object[i].params);
        dists[3] = sdCone(p - object[i].position, object[i].params.xy, object[i].params.z);
        dists[4] = sdCappedCylinder(p - object[i].position, object[i].params.x, object[i].params.y);

        float prevDist = dist;
        dist = min(dist, dists[object[i].objectType]);
        objIndex = lerp(objIndex, i, prevDist != dist);
    }
    
    index = objIndex;
    
    return dist;
})";
}

void SDFGenerator::WriteToShaderFile(std::string s)
{
	const auto p = std::filesystem::current_path() / "GeneratedSceneDistance.hlsli";
	std::ofstream file;
	file.open(p);
	file.write(s.c_str(), s.size());
	file.close();
}
