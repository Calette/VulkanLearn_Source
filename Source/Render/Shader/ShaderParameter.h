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
	};

	struct ShaderParameter
	{
		std::string name;
		ShaderParameterType type;
		unsigned binding;
	};
}