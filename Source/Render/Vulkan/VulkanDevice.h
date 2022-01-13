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
        //void CreateFramebuffers(VkRenderPass& renderPass);
        //void FreeFramebuffers();

    protected:
        void _CreateInstance();
        //void _CreateSwapChain(GLFWwindow* window);
        void _CreateSurface();
        void _PickPhysicalDevice();
        void _CreateLogicalDevice();
        //void _CreateImageViews();
        //void _CreateRenderPass();
        //void _CreateDescriptorSetLayout();
        //void _CreateGraphicsPipeline();
        //void _CreateFramebuffers();
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
        //QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device);
        int _RateDeviceSuitability(VkPhysicalDevice device);
        //VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        //VkPresentModeKHR _ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        //VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
        SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice device);

    protected:
        VkSurfaceKHR                    surface;
        
        // Device queues are implicitly cleaned up when the device is destroyed, so we don't need to do anything in cleanup
        VkQueue                         presentQueue;

        //VkSwapchainKHR                swapChain;
        //// The images were created by the implementation for the swap chain and they will be automatically cleaned up once the swap chain has been destroyed
        //std::vector<VkImage>          swapChainImages;
        //VkFormat                      swapChainImageFormat;
        //VkExtent2D                    swapChainExtent;
        //std::vector<VkImageView>      swapChainImageViews;
        //std::vector<VkFramebuffer>    swapChainFramebuffers;
        VulkanSwapChain*                vulkanSwapChain;

        //VkRenderPass renderPass;
        //VkDescriptorSetLayout descriptorSetLayout;
        //VkPipelineLayout pipelineLayout;
        //VkPipeline graphicsPipeline;
        VkCommandBufferBeginInfo        beginInfo;

        //VkBuffer indexBuffer;
        //VkDeviceMemory indexBufferMemory;

        //std::vector<VkBuffer> uniformBuffers;
        //std::vector<VkDeviceMemory> uniformBuffersMemory;

        std::vector<VkCommandBuffer>    commandBuffers;

        std::vector<VkSemaphore>        imageAvailableSemaphores;
        std::vector<VkSemaphore>        renderFinishedSemaphores;
        std::vector<VkFence>            inFlightFences;
        std::vector<VkFence>            imagesInFlight;

        size_t                          currentFrame    = 0;
        uint32_t                        imageIndex      = 0;
    };
}