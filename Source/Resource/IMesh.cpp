#pragma once
#include "IMesh.h"

namespace Palette
{
	IMesh::IMesh(std::vector<Vertex>& vertexData, std::vector<uint32_t>& indexData, VertexFormat vertexFormat)
		: m_VertexData{ vertexData }
		, m_IndexData{ indexData }
		, m_VertexFormat{ vertexFormat }
	{

	}
}