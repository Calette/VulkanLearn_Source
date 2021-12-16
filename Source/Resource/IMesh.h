#pragma once
#include <vector>

#include "Render/VertexInputBindingDescription.h"
#include "Render/VertexData.h"

namespace Palette
{
	class IMesh
	{
	public:
		IMesh(VertexRenderData& vertexData, std::vector<uint32_t>& indexData);
		//~IMesh();

		VertexRenderData& GetVertexRenderData() { return m_VertexRenderData; }
		std::vector<uint32_t>& GetIndexData() { return m_IndexData; }
		void SetVertexData(VertexRenderData& vertexData) { m_VertexRenderData = vertexData; }
		void SetIndexData(std::vector<uint32_t>& indexData) { m_IndexData = indexData; }

	protected:
		std::vector<uint32_t> m_IndexData;
		VertexRenderData m_VertexRenderData;
	};
}