#pragma once
#include <string>

namespace Palette
{
	enum class ShaderParameterType
	{
		Boolean,
		Integer,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Color,
		Matrix4x4,
		Texture,
		UnKnown
	};

	struct MaterialParameter
	{
		std::string name;
		ShaderParameterType type;
		size_t size;
		void* data;
	};

	struct ShaderParameter
	{
		std::string name;
		ShaderParameterType type;
		unsigned set;
		unsigned binding;
		uint32_t offset;
		size_t size;
		void* defaultData;
		void* targetData;
	};
}