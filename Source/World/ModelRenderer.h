#pragma once
#include "Resource/RenderMesh.h"
#include "Resource/Material.h"
#include "Render/RenderScene.h"

namespace Palette
{
	class ModelRenderer
	{
	public:
		ModelRenderer();
		~ModelRenderer();

		Material GetMaterial(uint32_t slot = 0) { return slot < m_Materials.size() ? m_Materials[slot] : nullptr; }
		RenderMesh GetMeshes(uint32_t slot = 0) { return slot < m_Meshes.size() ? m_Meshes[slot] : nullptr; }
		std::vector<RenderMesh>& GetAllMeshes() { return m_Meshes; }
		std::vector<Material>& GetAllMaterials() { return m_Materials; }

		void AddPrimitive_rt(RenderScene* scene);

	protected:
		std::vector<RenderMesh> m_Meshes;
		std::vector<Material> m_Materials;
	};
}