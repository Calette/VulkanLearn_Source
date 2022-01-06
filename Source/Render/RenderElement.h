#pragma once

#include "Resource/RenderMesh.h"
#include "Resource/Material.h"

namespace Palette
{
	void DrawMesh(RenderMesh mesh, Material mat);
	void DrawMeshInstance(RenderMesh mesh, Material mat);
}