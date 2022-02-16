#include "pch.h"
#include "SDFShaderGenerator.h"
#include <fstream>

std::string SDFShaderGenerator::GenerateSignedDistanceFunction(int objectType)
{
	std::string function = "float sdf" + SDFFuncContents[objectType].first + "(float3 p, float3 param){\n\t";
	function += SDFFuncContents[objectType].second;
	function += "\n}\n\n";

	return function;
}

std::string SDFShaderGenerator::GetSceneDistanceFunctionContents(std::vector<GameObject>& gameObjects)
{
	std::string objectsDistanceCheck = "";
	for (int i = 0; i < gameObjects.size(); i++)
	{
		std::string index = std::to_string(i);
		objectsDistanceCheck += "\tdist = min(dist, sdf" + SDFFuncContents[gameObjects[i].GetObjectType()].first + "(p - object[" + index + "].position, object[" + index + "].params));\n";
	}

	return "float dist = renderSettings.maxDist; \n\tint objIndex = 0;\n\n" + objectsDistanceCheck + "\n\tindex = objIndex; \n\treturn dist;";
}

void SDFShaderGenerator::WriteToHeaderShader(std::string content, std::ios_base::openmode writeMode)
{
	std::ofstream file;
	file.open(ShaderHeaderPath, writeMode);
	file.write(content.c_str(), content.size());
	file.close();
}

void SDFShaderGenerator::WriteDistanceFunctionToShader(std::string funcContents)
{
	if (!std::filesystem::exists(ShaderHeaderTemplatePath))
		return; // TODO: Error handling

	// Load template text from file
	std::ifstream shaderTemplate(ShaderHeaderTemplatePath);
	std::string templateContent;
	templateContent.assign((std::istreambuf_iterator<char>(shaderTemplate)),
	                       (std::istreambuf_iterator<char>()));
	shaderTemplate.close();

	// Replace flag with function contents
	size_t start_pos = templateContent.find(DistanceFunctionContentsFlag);
	if (start_pos == std::string::npos)
		return; // TODO: Error handling
	templateContent.replace(start_pos, DistanceFunctionContentsFlag.length(), funcContents);

	// Write content to file
	std::ofstream file;
	file.open(ShaderHeaderPath, std::ios::app);
	file.write(templateContent.c_str(), templateContent.size());
	file.close();
}
