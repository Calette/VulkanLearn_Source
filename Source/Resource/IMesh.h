#pragma once
#include <vector>

#include "Common/IRef.h"
#include "Render/Mesh/VertexInputBindingDescription.h"
#include "Render/Mesh/VertexData.h"

namespace Palette
{
	class IMesh : public IRef
	{
	public:
		IMesh(VertexRenderData& vertexData, std::vector<uint32_t>& indexData);
		//~IMesh();

		VertexRenderData& GetVertexRenderData() { return m_VertexRenderData; }
		std::vector<uint32_t>& GetIndexData() { return m_IndexData; }
		void SetVertexData(VertexRenderData& vertexData) { m_VertexRenderData = vertexData; }
		void SetIndexData(std::vector<uint32_t>& indexData) { m_IndexData = indexData; }

		//void OnRefDestroy() override;

	protected:
		std::vector<uint32_t> m_IndexData;
		VertexRenderData m_VertexRenderData;
	};
}