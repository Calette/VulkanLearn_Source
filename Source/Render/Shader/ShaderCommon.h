#pragma once

namespace Palette
{
	#define SETSHADERPARAM(name, type)															\
	public: void Set##name(std::string pName, type value)										\
	{																							\
		auto iter = m_Parameters.find(pName);													\
		if (iter != m_Parameters.end())															\
		{																						\
			auto size = sizeof(value);															\
			if(size != iter->second.size)														\
				printf("SetShaderParameters : Type of \"%s\" mismatch\n", pName.c_str());		\
			else																				\
				memcpy(iter->second.targetData, &value, size);									\
		}																						\
	}

	#define SETMATPARAM(name, type, pType)														\
	public: void Set##name(std::string pName, type value)										\
	{																							\
		auto iter = m_Parameters.find(pName);													\
		if (iter != m_Parameters.end())															\
			memcpy(iter->second.data, &value, sizeof(value));									\
		else																					\
		{																						\
			MaterialParameter param = MaterialParameter{ pName, pType };						\
			param.size = sizeof(value);															\
			param.data = malloc(param.size);													\
			memcpy(param.data, &value, param.size);												\
		}																						\
																								\
		for(auto shader : m_Shaders)															\
		{																						\
			shader->Set##name(pName, value);													\
		}																						\
	}
}