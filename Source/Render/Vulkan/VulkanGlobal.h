#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

#include "Client/Client.h"
#include "Render/Vulkan/VulkanDevice.h"

namespace PaletteGlobal
{
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	extern const std::string PROJECTPATH;
	extern const std::string SHADERPATH;

	extern VkDevice device;
	extern VkPhysicalDevice physicalDevice;
	extern VkInstance instance;
	extern VkCommandPool commandPool;
	extern VkQueue graphicsQueue;

	extern Palette::Client* client;
	extern Palette::VulkanDevice* vulkanDevice;
	extern uint32_t frameCount;
}
