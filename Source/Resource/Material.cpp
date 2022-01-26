#include "Material.h"

namespace Palette
{
	class MaterialParameterData
	{
	
	};

	using MaterialParameterDictionary = std::unordered_map<std::string, MaterialParameterData>;

	Material MaterialResource::DefualtMat = nullptr;

	MaterialResource::MaterialResource()
	{

	}

	MaterialResource::~MaterialResource()
	{
		
	}

	void MaterialResource::OnRefDestroy()
	{
		m_Shaders.clear();
	}

	MaterialResource::MaterialResource(Shader shader)
		: IMaterial()
	{
		m_Shaders.emplace_back(shader);
	}

	Material MaterialResource::Copy()
	{
		MaterialResource* copyMat = new MaterialResource();
		copyMat->m_Shaders = m_Shaders;
		return Material(copyMat);
	}

	bool MaterialResource::GetDescriptorSet(Shader shader, VkDescriptorSet& descriptorSet)
	{ 
		auto iter = m_DescriptorSets.find(shader);
		if (iter == m_DescriptorSets.end())
		{
			return false;
		}
		descriptorSet = iter->second->GetDescriptorSet();
		return true;
	}


	Material MaterialResource::GetDefualtMat()
	{
		if (!DefualtMat)
		{
			DefualtMat = Material(new MaterialResource(ShaderResource::GetDefaultShader()));
		}
		return DefualtMat;
	}

	void MaterialResource::ReleaseDefualtMat()
	{
		DefualtMat->ReleaseRef();
	}
}
