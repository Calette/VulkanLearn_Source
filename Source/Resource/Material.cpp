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
		for (auto shader : m_Shaders)
		{
			shader->GetShaderModule(0u)->ReleaseRef();
		}
		m_Shaders.clear();
	}

	MaterialResource::MaterialResource(Shader* shader)
		: IMaterial()
	{
		m_Shaders.push_back(shader);
		shader->GetShaderModule(0u)->AddRef();
	}

	MaterialResource::MaterialResource(MaterialResource* material)
		: IMaterial()
	{
		for (auto shader : material->GetShaders())
		{
			m_Shaders.push_back(shader);
			shader->GetShaderModule(0u)->AddRef();
		}
	}

	Material MaterialResource::GetDefualtMat()
	{
		if (DefualtMat)
		{
			DefualtMat = Material(new MaterialResource(Shader::GetDefaultShader()));
			// always keep default mat
			DefualtMat->GetShaders()[0]->GetShaderModule(0u)->AddRef();
		}
		return DefualtMat;
	}

	void MaterialResource::ReleaseDefualtMat()
	{
		//delete DefualtMat;
	}
}
