#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

namespace Palette 
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char*> validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

	struct VulkanDevice
	{
    public:
        explicit VulkanDevice(GLFWwindow* window);
        ~VulkanDevice();

        void DrawFrame();

    protected:
        void _CreateInstance();
        void _CreateSwapChain(GLFWwindow* window);
        void _CreateSurface(GLFWwindow* window);
        void _PickPhysicalDevice();
        void _CreateLogicalDevice();
        void _CreateImageViews();
        void _CreateRenderPass();
        void _CreateDescriptorSetLayout();
        void _CreateGraphicsPipeline();
        void _CreateFramebuffers();
        void _CreateCommandPool();
        void _CreateCommandBuffers();
        void _CreateSyncObjects();

        void _CleanupSwapChain();

        std::vector<const char*> _GetRequiredExtensions();
        void _ShowAllExtensionsSupport();
        bool _CheckValidationLayerSupport();
        bool _IsDeviceSuitable(VkPhysicalDevice device);
        bool _CheckDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device);
        int _RateDeviceSuitability(VkPhysicalDevice device);
        VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR _ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
        SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice device);

    protected:
        VkSurfaceKHR surface;
        
        // Device queues are implicitly cleaned up when the device is destroyed, so we don't need to do anything in cleanup
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        // The images were created by the implementation for the swap chain and they will be automatically cleaned up once the swap chain has been destroyed
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
	};
}