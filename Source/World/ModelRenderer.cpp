#include "ModelRenderer.h"
#include "Render/RenderElement.h"

namespace Palette
{
	ModelRenderer::ModelRenderer()
	{
		// tempCode
		Material mat = MaterialResource::GetDefaultMat();
		m_Materials.push_back(mat);

		// Mesh1
		std::vector<Vertex_P3_C3_T2> vertices =
		{
			{{-0.5f, -0.5f, 0.0f},	{1.0f, 0.0f, 0.0f},	{1.0f, 0.0f}},
			{{0.5f,  -0.5f, 0.0f},	{0.0f, 1.0f, 0.0f},	{0.0f, 0.0f}},
			{{0.5f,  0.5f,  0.0f},	{0.0f, 0.0f, 1.0f},	{0.0f, 1.0f}},
			{{-0.5f, 0.5f,  0.0f},	{1.0f, 1.0f, 1.0f},	{1.0f, 1.0f}}
		};
		// It is possible to use either uint16_t or uint32_t for your index buffer depending on the number of entries in vertices
		std::vector<uint32_t> indices = 
		{
			0, 1, 2, 2, 3, 0
		};

		std::vector<uint8_t> verticesData;
		size_t bufferSize = sizeof(Vertex_P3_C3_T2) * vertices.size();
		verticesData.resize(bufferSize);
		memcpy(verticesData.data(), vertices.data(), bufferSize);
		VertexFormat vertexFormat = VertexFormat::VERTEX_P3_C3_U2;

		VertexRenderData vertexData = { vertexFormat, verticesData, static_cast<uint32_t>(vertices.size()) };

		RenderMesh renderMesh = RenderMesh(new RenderMeshResourse(vertexData, indices));
		m_Meshes.push_back(renderMesh);

		m_Materials.push_back(mat);

		// Mesh2
		std::vector<Vertex_P3_C3_T2> vertices2 =
		{
			{{-0.0f, -0.0f, 1.0f},	{1.0f, 0.0f, 0.0f},	{.5f, 0.0f}},
			{{0.75f, -0.0f, 1.0f},	{0.0f, 1.0f, 0.0f},	{0.0f, 0.0f}},
			{{0.75f, 0.75f, 1.0f},	{0.0f, 0.0f, 1.0f},	{0.0f, .5f}},
			{{-0.0f, 0.75f, 1.0f},	{1.0f, 1.0f, 1.0f},	{.5f, .5f}}
		};

		std::vector<uint8_t> verticesData2;
		verticesData2.resize(bufferSize);
		memcpy(verticesData2.data(), vertices2.data(), bufferSize);

		VertexRenderData vertexData2 = { vertexFormat, verticesData2, static_cast<uint32_t>(vertices2.size()) };

		RenderMesh renderMesh2 = RenderMesh(new RenderMeshResourse(vertexData2, indices));
		m_Meshes.push_back(renderMesh2);
	}

	ModelRenderer::~ModelRenderer()
	{

	}

	void ModelRenderer::AddPrimitive_rt(RenderScene* scene)
	{
		size_t size = m_Meshes.size();
		size_t maxSize = m_Materials.size() - 1;
		for (size_t i = 0; i < size; i++)
		{
			RenderElement* element = new RenderElement();
			element->Mesh = m_Meshes[i];
			element->Mat = m_Materials[min(i, maxSize)];

			scene->AddElement_rt(element);
		}
	}
}
