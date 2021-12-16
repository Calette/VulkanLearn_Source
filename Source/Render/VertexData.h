#pragma once
#include "VertexInputBindingDescription.h"

namespace Palette
{
	struct VertexRenderData
	{
		VertexFormat m_VertexFormat;
		std::vector<uint8_t> m_VertexData;
		uint32_t m_VertexCount;
	};
}
