#pragma once
#include "IMesh.h"

namespace Palette
{
	class RenderMeshResourse : public IMesh
	{
	public:
		RenderMeshResourse(VertexRenderData& vertexData, std::vector<uint32_t>& indexData);
		~RenderMeshResourse();

		VkBuffer& GetVertexBuffer() { return m_VertexBuffer; }
		VkDeviceMemory& GetVertexBufferMemory() { return m_VertexBufferMemory; }

		VkBuffer& GetIndexBuffer() { return m_IndexBuffer; }
		VkDeviceMemory& GetIndexBufferMemory() { return m_IndexBufferMemory; }

	protected:
		void _CreateVertexBuffer();
		void _CreateIndexBuffer();

	protected:
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
	};
}
