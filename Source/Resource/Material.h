#pragma once
#include "Shader.h"
#include "Resourse.h"

namespace Palette
{
	enum BlendMode
	{
		BLEND_Opaque,
		BLEND_Translucent
	};

	class IMaterial : public IRef
	{
	protected:
		IMaterial() noexcept = default;
		~IMaterial() noexcept = default;

	public:
		std::vector<Shader*>& GetShaders() { return m_Shaders; };

	protected:
		std::vector<Shader*> m_Shaders;
	};

	class MaterialResource : public IMaterial
	{
	public:
		MaterialResource();
		~MaterialResource();
		MaterialResource(Shader* shader);
		MaterialResource(MaterialResource* material);

		void OnRefDestroy() override;

		static Material GetDefualtMat();
		static void ReleaseDefualtMat();

	protected:
		static Material DefualtMat;
		BlendMode m_BlendMode;
	};
}
