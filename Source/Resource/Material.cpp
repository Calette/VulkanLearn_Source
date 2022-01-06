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
		m_Shaders.push_back(shader);
	}

	MaterialResource::MaterialResource(MaterialResource* material)
		: IMaterial()
	{
		for (auto shader : material->GetShaders())
		{
			m_Shaders.push_back(shader);
		}
	}

	Material MaterialResource::GetDefualtMat()
	{
		if (!DefualtMat)
		{
			DefualtMat = Material(new MaterialResource(ShaderResource::GetDefaultShader()));
			// always keep default mat
			//DefualtMat->AddRef();
		}
		return DefualtMat;
	}

	void MaterialResource::ReleaseDefualtMat()
	{
		DefualtMat->ReleaseRef();
	}
}
