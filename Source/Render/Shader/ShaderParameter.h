#pragma once
#include <string>

namespace Palette
{
	enum class ShaderParameterType
	{
		Boolean,
		Integer,
		Float,
		Vector,
		Color,
		Matrix,
		Texture
	};

	struct MaterialParameter
	{
		std::string name;
		ShaderParameterType type;
	};

	struct ShaderParameter
	{
		std::string name;
		ShaderParameterType type;
		int binding;
	};
}