#pragma once
#include <glm/glm.hpp>

namespace Palette
{
	enum class ShaderType
	{
		VertexPixel,
		Compute,
		None
	};

	enum class PassType
	{
		DefualtPass = 1,
		SimplePass = 2,
	};

	enum class ConstantBufferType
	{
		GlobalConstant
	};

	struct GlobalConstant
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}