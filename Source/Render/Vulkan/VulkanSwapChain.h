#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "VulkanCommon.h"

namespace Palette
{
	class VulkanSwapChain
	{
    public:
        VulkanSwapChain(VkSurfaceKHR surface);
        void CreateFramebuffers(VkRenderPass& renderPass);
        void FreeFramebuffers();
        void CleanupSwapChain();
        void ResizeSwapChain();

    private:
        void _CreateSwapChain();
        void _CreateImageViews();
        SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR _ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    public:
        uint32_t                    imageCount;

		VkSwapchainKHR              swapChain = VK_NULL_HANDLE;
        // The images were created by the implementation for the swap chain and they will be automatically cleaned up once the swap chain has been destroyed
        std::vector<VkImage>        swapChainImages;
        VkFormat                    swapChainImageFormat;
        VkExtent2D                  swapChainExtent;
        std::vector<VkImageView>    swapChainImageViews;
        std::vector<VkFramebuffer>  swapChainFramebuffers;
    private:
        VkSurfaceKHR                surface;
	};
}