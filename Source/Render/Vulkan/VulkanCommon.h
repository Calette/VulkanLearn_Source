#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <iostream>
#include <assert.h>

namespace Palette
{
    std::string GetErrorString(VkResult errorCode);

#define VK_CHECK_RESULT(f)          \
{						            \
	VkResult res = (f);				\
	if (res != VK_SUCCESS)	        \
	{						        \
		std::cout << "Fatal : VkResult is \"" << GetErrorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n";   \
		assert(res == VK_SUCCESS);	\
	}								\
}
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        // any value of uint32_t could in theory be a valid queue family index including 0. 
        // Luckily C++17 introduced a data structure to distinguish between the case of a value existing or not
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
}