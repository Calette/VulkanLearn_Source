#include "ModelRenderer.h"
#include "Render/RenderElement.h"

namespace Palette
{
	ModelRenderer::ModelRenderer()
	{
		Material mat = MaterialResource::GetDefualtMat();
		m_Materials.push_back(mat);

		std::vector<Vertex_P2_C3> vertices =
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};
		Vertex_P2_C3 vert = { {-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} };
		// It is possible to use either uint16_t or uint32_t for your index buffer depending on the number of entries in vertices
		std::vector<uint32_t> indices = 
		{
			0, 1, 2, 2, 3, 0
		};

		std::vector<uint8_t> verticesData;
		size_t bufferSize = sizeof(Vertex_P2_C3) * vertices.size();
		verticesData.resize(bufferSize);
		memcpy(verticesData.data(), vertices.data(), bufferSize);
		VertexFormat vertexFormat = VertexFormat::VERTEX_P2_C3;

		VertexRenderData vertexData = { vertexFormat, verticesData, vertices.size() };

		RenderMesh renderMesh = RenderMesh(new RenderMeshResourse(vertexData, indices));
		m_Meshes.push_back(renderMesh);
	}

	ModelRenderer::~ModelRenderer()
	{
		for (auto mat : m_Materials)
		{
			mat->GetShaders()[0]->RealeaseShaderModule(0);
		}

		//m_Meshes.clear();
	}

	void ModelRenderer::_DrawItem_rt()
	{
		int size = m_Materials.size();
		if (size == m_Meshes.size())
		{
			for (int i = 0; i < size; i++)
			{
				DrawMesh(m_Meshes[i], m_Materials[i]);
			}
		}
	}
}
