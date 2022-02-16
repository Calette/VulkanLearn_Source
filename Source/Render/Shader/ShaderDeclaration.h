#pragma once
#include <glm/glm.hpp>

namespace Palette
{
	enum class ShaderType
	{
		vertexFrag,
		compute,
		none
	};

	enum PassType
	{
		defaultPass = 1,
		simplePass = 2
	};

	enum ConstantBufferType
	{
		customConstant = 0,
		globalConstant = 1
	};

	struct ConstantBuffer
	{

	};

	struct GlobalConstant : public ConstantBuffer
	{
		//glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}