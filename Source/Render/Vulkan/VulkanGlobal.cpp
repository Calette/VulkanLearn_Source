#include "VulkanGlobal.h"
#include <direct.h>

namespace PaletteGlobal
{
	const std::string PROJECTPATH = _getcwd(NULL, 0);
	const std::string SHADERPATH = PROJECTPATH + "/Shaders/";

	int width = 800;
	int height = 600;

	GLFWwindow* window = nullptr;
	VkInstance instance = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	// Device queues are implicitly cleaned up when the device is destroyed, so we don't need to do anything in cleanup
	VkQueue graphicsQueue = VK_NULL_HANDLE;

	Palette::VulkanDevice* vulkanDevice = nullptr;
	Palette::Client* client = nullptr;
	uint32_t frameCount = 0;
}
