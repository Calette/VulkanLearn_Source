#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Palette
{
	class Texture
	{
	public:
		~Texture();
		void CreateTexture();
		void CreateTextureImageView();
		void CreateTextureSampler();

	protected:
		static float GetMaxSamplerAnisotropy();

	protected:
		static float maxSamplerAnisotropy;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	};
}