#pragma once
#include <glm/glm.hpp>

namespace Palette
{
	enum class ShaderType
	{
		vertexFrag,
		compute,
		errorShader
	};

	enum PassType
	{
		DEFAULT_PASS = 1,
		SIMPLE_PASS = 2
	};

	enum ConstantBufferType
	{
		CUSTOM_CONSTANT = 0,
		GLOBAL_CONSTANT = 1,
	};

	struct ConstantBuffer
	{
		virtual ~ConstantBuffer() = default;
		virtual VkDeviceSize GetSize() = 0;
		virtual void* GetData() = 0;
	};

	struct GlobalConstant : public ConstantBuffer
	{
		VkDeviceSize GetSize() override { return sizeof(GlobalConstant) - sizeof(void*); };
		void* GetData() override { return &view; };
		//glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct CustomConstant : public ConstantBuffer
	{
		CustomConstant(VkDeviceSize size) : size(size) {}
		VkDeviceSize GetSize() override { return size; };
		void* GetData() override { return data; };
	private:
		VkDeviceSize size;
		void* data;
	};
}