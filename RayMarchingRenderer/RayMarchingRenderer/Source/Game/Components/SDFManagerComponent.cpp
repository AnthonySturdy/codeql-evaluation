#include "pch.h"
#include "SDFManagerComponent.h"

#include <fstream>


void SDFManagerComponent::RenderGUI()
{
	for (int i = 0; i < SDFFuncContents.size(); i++)
	{
		if (ImGui::Button("Remove"))
		{
			SDFFuncContents.erase(SDFFuncContents.begin() + i);
			break;
		}

		ImGui::PushID(i);

		ImGui::InputText("##", &SDFFuncContents[i].first);
		ImGui::Text(("float sdf" + SDFFuncContents[i].first + "(float3 p, float3 param) {").c_str());
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputTextMultiline("###", &SDFFuncContents[i].second);
		ImGui::PopItemWidth();
		ImGui::Text("}");

		ImGui::PopID();

		ImGui::Separator();
	}

	if (ImGui::Button("New SDF"))
	{
		SDFFuncContents.push_back({ "NewSDF" + std::to_string(SDFFuncContents.size()), "return 0;" });
	}
}

std::string SDFManagerComponent::GenerateSignedDistanceFunction(int objectType) const
{
	std::string function = "float sdf" + SDFFuncContents[objectType % SDFFuncContents.size()].first + "(float3 p, float3 param){\n\t";
	function += SDFFuncContents[objectType % SDFFuncContents.size()].second;
	function += "\n}\n\n";

	return function;
}

std::string SDFManagerComponent::GenerateSceneDistanceFunctionContents(const std::vector<RayMarchObjectComponent*>& raymarchObjects) const
{
	std::string objectsDistanceCheck;
	for (int i = 0; i < raymarchObjects.size(); i++)
	{
		std::string index = std::to_string(i);
		objectsDistanceCheck += "\tdist = min(dist, sdf" + SDFFuncContents[raymarchObjects[i]->GetSDFType() % SDFFuncContents.size()].first;
		objectsDistanceCheck += +"(Rotate(Translate(p, ObjectsList[" + index + "].Position), ObjectsList[" + index + "].Rotation) / ObjectsList[" + index + "].Scale.x, ObjectsList[" + index + "].Parameters) * ObjectsList[" + index + "].Scale.x);\n";
	}

	return "float dist = renderSettings.maxDist;\n\n" + objectsDistanceCheck + "\n\treturn dist;";
}

void SDFManagerComponent::WriteStringToHeaderShader(const std::string& content, std::ios_base::openmode writeMode) const
{
	std::ofstream file;
	file.open(ShaderHeaderPath, writeMode);
	file.write(content.c_str(), content.size());
	file.close();
}

void SDFManagerComponent::WriteSceneDistanceFunctionToShaderHeader(const std::string& funcContents) const
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
	const size_t start_pos = templateContent.find(DistanceFunctionContentsFlag);
	if (start_pos == std::string::npos)
		return; // TODO: Error handling
	templateContent.replace(start_pos, DistanceFunctionContentsFlag.length(), funcContents);

	// Write content to file
	std::ofstream file;
	file.open(ShaderHeaderPath, std::ios::app);
	file.write(templateContent.c_str(), templateContent.size());
	file.close();
}
