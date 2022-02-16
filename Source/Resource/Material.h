#pragma once
#include "Shader.h"
#include "Resourse.h"
#include "Render/Shader/ConstantBuffer.h"
#include "Render/Shader/ShaderParameter.h"

namespace Palette
{
	class IRenderPass;

	enum class BlendMode
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
		std::vector<Shader>& GetShaders() { return m_Shaders; }

	protected:
		std::vector<Shader>	m_Shaders;
	};

	class MaterialResource : public IMaterial
	{
	public:
		MaterialResource();
		~MaterialResource();
		MaterialResource(Shader shader);

		void OnRefDestroy() override;
		
		Material Copy();

		static Material GetDefaultMat();
		static void ReleaseDefaultMat();

	protected:
		static Material	DefaultMat;
		BlendMode		m_BlendMode;

		//struct KeyHasher
		//{
		//	std::size_t operator()(const Shader s) const
		//	{
		//		return ((std::hash<std::string>()(s->GetName())
		//			^ (std::hash<std::string>()(s->GetSourcePath()) << 1)) >> 1)
		//			^ (std::hash<uint32_t>()((uint32_t)s->GetPassType()) << 1);
		//	}
		//};
		//std::unordered_map<Shader, VkConstantBuffer*, KeyHasher>	m_DescriptorSets;
		std::vector<MaterialParameter>							m_Parameters;
	};
}
