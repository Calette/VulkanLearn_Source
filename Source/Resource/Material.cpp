#include "Material.h"

namespace Palette
{
	class MaterialParameterData
	{
	
	};

	using MaterialParameterDictionary = std::unordered_map<std::string, MaterialParameterData>;

	Material MaterialResource::DefaultMat = nullptr;

	MaterialResource::MaterialResource()
	{

	}

	MaterialResource::~MaterialResource()
	{
		for(auto param : m_Parameters)
		{
			free(param.second.data);
		}
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
		// todo
		//copyMat->m_Shaders = m_Shaders;
		copyMat->m_BlendMode = m_BlendMode;
		copyMat->m_Parameters = m_Parameters;
		return Material(copyMat);
	}

	Material MaterialResource::GetDefaultMat()
	{
		if (!DefaultMat)
		{
			DefaultMat = Material(new MaterialResource(ShaderResource::GetDefaultShader()));
		}
		return DefaultMat;
	}

	void MaterialResource::ReleaseDefaultMat()
	{
		DefaultMat->ReleaseRef();
	}
}
