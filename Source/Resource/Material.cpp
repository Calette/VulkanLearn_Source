#include "Material.h"

namespace Palette
{
	class MaterialParameterData
	{
	
	};

	using MaterialParameterDictionary = std::unordered_map<std::string, MaterialParameterData>;

	Material* Material::DefualtMat = nullptr;

	Material::Material()
		: IMaterial()
	{

	}

	Material::~Material()
	{
		for (auto shader : m_Shaders)
		{
			shader->GetShaderModule(0u)->Release();
		}
		m_Shaders.clear();
	}

	Material::Material(Shader* shader)
		: IMaterial()
	{
		m_Shaders.push_back(shader);
		shader->GetShaderModule(0u)->AddRefrenceCount();
	}

	Material::Material(Material* material)
		: IMaterial()
	{
		for (auto shader : material->GetShaders())
		{
			m_Shaders.push_back(shader);
			shader->GetShaderModule(0u)->AddRefrenceCount();
		}
	}

	Material* Material::GetDefualtMat()
	{
		if (DefualtMat == nullptr)
		{
			DefualtMat = new Material(Shader::GetDefaultShader());
			// always keep default mat
			DefualtMat->GetShaders()[0]->GetShaderModule(0u)->AddRefrenceCount();
		}
		return DefualtMat;
	}

	void Material::ReleaseDefualtMat()
	{
		delete DefualtMat;
	}
}
