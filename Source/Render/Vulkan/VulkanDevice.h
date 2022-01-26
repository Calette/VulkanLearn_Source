#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "VulkanCommon.h"

namespace Palette 
{
    class VulkanSwapChain;

    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char*> validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    
	struct VulkanDevice
	{
    public:
        VulkanDevice();
        ~VulkanDevice();

        void PreRender_rt();
        void DrawFrame_rt();
        void PostRender_rt();

        VkCommandBuffer& GetCommandBuffer() { return commandBuffers[imageIndex]; }
        VkExtent2D& GetExtent2D();
        VulkanSwapChain* GetVulkanSwapChain() { return vulkanSwapChain; }
        VkFramebuffer& GetSwapChainFramebuffer();
        std::vector<VkCommandBuffer>& GetCommandBuffers() { return commandBuffers; }
        uint32_t GetImageCount() const;
        uint32_t GetImageIndex() const { return imageIndex; }

    protected:
        void _CreateInstance();
        void _CreateSurface();
        void _PickPhysicalDevice();
        void _CreateLogicalDevice();
        void _CreateCommandPool();
        void _CreateCommandBuffers();
        void _CreateSyncObjects();

        //void _CleanupSwapChain();

        void _WindowResize();

        std::vector<const char*> _GetRequiredExtensions();
        void _ShowAllExtensionsSupport();
        bool _CheckValidationLayerSupport();
        bool _IsDeviceSuitable(VkPhysicalDevice device);
        bool _CheckDeviceExtensionSupport(VkPhysicalDevice device);
        int _RateDeviceSuitability(VkPhysicalDevice device);
        SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice device);

    protected:
        VkSurfaceKHR                    surface;
        
        // Device queues are implicitly cleaned up when the device is destroyed, so we don't need to do anything in cleanup
        VkQueue                         presentQueue;

        VulkanSwapChain*                vulkanSwapChain;

        VkCommandBufferBeginInfo        beginInfo;

        std::vector<VkCommandBuffer>    commandBuffers;

        std::vector<VkSemaphore>        imageAvailableSemaphores;
        std::vector<VkSemaphore>        renderFinishedSemaphores;
        std::vector<VkFence>            inFlightFences;
        std::vector<VkFence>            imagesInFlight;

        size_t                          currentFrame    = 0;
        uint32_t                        imageIndex      = 0;
    };
}