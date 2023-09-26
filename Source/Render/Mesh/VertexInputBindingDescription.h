#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>

namespace Palette
{
	enum class VertexAtterbute
	{
		Position = 1,
		Normal = 2,
		UV = 4,
		Color = 8,
		Tangent = 16
	};

	enum class VertexFormat
	{
		VERTEX_COMMON_P3_U2 = 0b00101,
		VERTEX_COMMON_P3_U2_C3 = 0b01101,
		VERTEX_COMMON = 0b11111,
		VERTEX_P3_C3_U2
	};

	struct Vertex
	{

	};

	struct Vertex_Common : public Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec4 color;
		glm::vec4 tangent;

		Vertex_Common(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec4 c, glm::vec4 t) : pos(p), normal(n), uv(u), color(c), tangent(t) {}

		static void GetBindingDescription(VkVertexInputBindingDescription& bindingDescription);

		static void GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, VertexFormat vertexFormat);
	};

	struct Vertex_P3_C3_T2 : public Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 uv;

		Vertex_P3_C3_T2(glm::vec3 p, glm::vec3 c, glm::vec2 t) : pos(p), color(c), uv(t) {}

		static void GetBindingDescription(VkVertexInputBindingDescription& bindingDescription);

		static void GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
	};

	void GetVertexInputDescriptions(VertexFormat vertexFormat, VkVertexInputBindingDescription& bindingDescription, std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
}