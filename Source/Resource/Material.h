#pragma once
#include "Shader.h"

namespace Palette
{
	enum BlendMode
	{
		BLEND_Opaque,
		BLEND_Translucent
	};

	class IMaterial
	{
	public:
		std::vector<Shader*>& GetShaders() { return m_Shaders; };

	protected:
		std::vector<Shader*> m_Shaders;
	};

	class Material : public IMaterial
	{
	public:
		Material();
		~Material();
		Material(Shader* shader);
		Material(Material* material);

		static Material* GetDefualtMat();
		static void ReleaseDefualtMat();

	protected:
		static Material* DefualtMat;
		BlendMode m_BlendMode;
	};

	class SubMaterial : public IMaterial
	{
	public:
		std::vector<IMaterial*> m_Materials;
	};
}
