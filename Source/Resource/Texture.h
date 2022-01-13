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
		float GetMaxSamplerAnisotropy();

	protected:
		float maxSamplerAnisotropy = 0.0f;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	};
}