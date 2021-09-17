#pragma once
#include <vector>

#include "Render/VertexInputBindingDescription.h"

namespace Palette
{
	class IMesh
	{
	public:
		IMesh(std::vector<Vertex>& vertexData, std::vector<uint32_t>& indexData, VertexFormat vertexFormat);
		//~IMesh();

		std::vector<Vertex>& GetVertexData() { return m_VertexData; }
		std::vector<uint32_t>& GetIndexData() { return m_IndexData; }
		void SetVertexData(std::vector<Vertex>& vertexData) { m_VertexData = vertexData; }
		void SetIndexData(std::vector<uint32_t>& indexData) { m_IndexData = indexData; }

		VertexFormat GetVertexFormat() { return m_VertexFormat; }

	protected:
		std::vector<Vertex> m_VertexData;
		std::vector<uint32_t> m_IndexData;

		VertexFormat m_VertexFormat;
	};
}