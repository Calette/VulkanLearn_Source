#pragma once
#include "Common/IPtr.h"

namespace Palette
{
	class IShaderModuleResourse;
	typedef TSharedPtr<IShaderModuleResourse> IShaderModule;

	class ShaderResource;
	typedef TSharedPtr<ShaderResource> Shader;

	class MaterialResource;
	typedef TSharedPtr<MaterialResource> Material;

	class RenderMeshResourse;
	typedef TSharedPtr<RenderMeshResourse> RenderMesh;
}
