#include "RenderMesh.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "Render/Vulkan/VulkanDevice.h"
#include "Render/Vulkan/VulkanUtilities.h"

namespace Palette
{
	RenderMeshResourse::RenderMeshResourse(VertexRenderData& vertexData, std::vector<uint32_t>& indexData)
		: IMesh(vertexData, indexData)
	{
		_CreateVertexBuffer();
		_CreateIndexBuffer();
	}

	RenderMeshResourse::~RenderMeshResourse()
	{
		// Memory that is bound to a buffer object may be freed once the buffer is no longer used, 
		// so we shall free it after the buffer has been destroyed
		vkDestroyBuffer(PaletteGlobal::device, m_VertexBuffer, nullptr);
		vkFreeMemory(PaletteGlobal::device, m_VertexBufferMemory, nullptr);

		vkDestroyBuffer(PaletteGlobal::device, m_IndexBuffer, nullptr);
		vkFreeMemory(PaletteGlobal::device, m_IndexBufferMemory, nullptr);
	}

	void RenderMeshResourse::_CreateVertexBuffer()
	{
		// The vertex buffer we have right now works correctly, but the memory type that
		// allows us to access it from the CPU may not be the most optimal memory type for the graphics card itself to read from.
		VkDeviceSize bufferSize = sizeof(m_VertexRenderData.m_VertexData[0]) * m_VertexRenderData.m_VertexData.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingBuffer, stagingBufferMemory);

		void* data;
		VK_CHECK_RESULT(vkMapMemory(PaletteGlobal::device, stagingBufferMemory, 0, bufferSize, 0, &data))
		memcpy(data, m_VertexRenderData.m_VertexData.data(), (size_t)bufferSize);
		vkUnmapMemory(PaletteGlobal::device, stagingBufferMemory);

		// VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VertexBuffer, m_VertexBufferMemory);
		
		CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

		vkDestroyBuffer(PaletteGlobal::device, stagingBuffer, nullptr);
		vkFreeMemory(PaletteGlobal::device, stagingBufferMemory, nullptr);
	}

	void RenderMeshResourse::_CreateIndexBuffer() 
	{
		VkDeviceSize bufferSize = sizeof(m_IndexData[0]) * m_IndexData.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(PaletteGlobal::device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, m_IndexData.data(), (size_t)bufferSize);
		vkUnmapMemory(PaletteGlobal::device, stagingBufferMemory);

		// VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_IndexBuffer, m_IndexBufferMemory);

		CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(PaletteGlobal::device, stagingBuffer, nullptr);
		vkFreeMemory(PaletteGlobal::device, stagingBufferMemory, nullptr);
	}
}
