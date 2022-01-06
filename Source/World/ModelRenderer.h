#pragma once
#include "Render/IRenderElement.h"
#include "Resource/RenderMesh.h"
#include "Resource/Material.h"

namespace Palette
{
	class ModelRenderer : public IRenderElement
	{
	public:
		ModelRenderer();
		~ModelRenderer();

		Material GetMaterial(uint32_t slot = 0) { return slot < m_Materials.size() ? m_Materials[slot] : nullptr; }
		RenderMesh GetMeshes(uint32_t slot = 0) { return slot < m_Meshes.size() ? m_Meshes[slot] : nullptr; }
		std::vector<RenderMesh>& GetAllMeshes() { return m_Meshes; }
		std::vector<Material>& GetAllMaterials() { return m_Materials; }

		void _DrawItem_rt() override;

	protected:
		std::vector<RenderMesh> m_Meshes;
		std::vector<Material> m_Materials;
	};
}