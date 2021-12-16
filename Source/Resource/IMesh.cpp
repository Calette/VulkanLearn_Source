#pragma once
#include "IMesh.h"

namespace Palette
{
	IMesh::IMesh(VertexRenderData& vertexRenderData, std::vector<uint32_t>& indexData)
		: m_VertexRenderData{ vertexRenderData }
		, m_IndexData{ indexData }
	{

	}
}