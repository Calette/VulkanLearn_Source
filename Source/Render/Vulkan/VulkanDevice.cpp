#include <iostream>
#include <algorithm>
#include <set>

#include "VulkanDevice.h"
#include "VulkanGlobal.h"
#include "Common/DebugTool.h"
#include "World/Entity.h"
#include "World/ModelComponent.h"

namespace Palette
{
    using PaletteGlobal::instance;
    using PaletteGlobal::device;
    using PaletteGlobal::commandPool;
    using PaletteGlobal::graphicsQueue;
    using PaletteGlobal::physicalDevice;

    VulkanDevice::VulkanDevice(GLFWwindow* window)
    {
        _CreateInstance();
        SetupDebugMessenger();
        _CreateSurface(window);
        _PickPhysicalDevice();
        _CreateLogicalDevice();
        _CreateSwapChain(window);
        _CreateImageViews();
        // in advance
        _CreateCommandPool();
        //_CreateRenderPass();
        //_CreateDescriptorSetLayout();
        //_CreateGraphicsPipeline();
        //_CreateFramebuffers();
        _CreateCommandBuffers();
        _CreateSyncObjects();
    }

    VulkanDevice::~VulkanDevice()
    {
        MaterialResource::ReleaseDefualtMat();

        _CleanupSwapChain();

        //vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device, commandPool, nullptr);

        vkDestroyDevice(device, nullptr);

        if (PaletteGlobal::enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void VulkanDevice::PreRender_rt()
    {
        if (PaletteGlobal::frameCount != 0)
        {
            vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        }

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
    }

    void VulkanDevice::PostRender_rt()
    {
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void VulkanDevice::DrawFrame_rt()
    {
        // Check if a previous frame is using this image (i.e. there is its fence to wait on)
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) 
        {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        // Mark the image as now being in use by this frame
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        //presentInfo.pResults = nullptr; // Optional

        vkQueuePresentKHR(presentQueue, &presentInfo);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    VkCommandBuffer& VulkanDevice::GetCommandBuffer()
    {
        return commandBuffers[imageIndex];
    }

    VkExtent2D& VulkanDevice::GetExtent2D()
    {
        return swapChainExtent;
    }

    VkFramebuffer& VulkanDevice::GetSwapChainFramebuffer()
    {
        return swapChainFramebuffers[imageIndex];
    }

    void VulkanDevice::CreateFramebuffers(VkRenderPass& renderPass)
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] =
            {
                swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanDevice::FreeFramebuffers()
    {
        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
    }

    void VulkanDevice::_CleanupSwapChain()
    {
        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        //vkDestroyPipeline(device, graphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        //vkDestroyRenderPass(device, renderPass, nullptr);

        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }
    
    void VulkanDevice::_CreateInstance()
    {
        // checking for extension support
        if (false)
        {
            _ShowAllExtensionsSupport();
        }

        // checking for Validation Layer Support
        if (PaletteGlobal::enableValidationLayers && !_CheckValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Palette";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // which global extensions and validation layers we want to use
        auto extensions = _GetRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // it will automatically be used during vkCreateInstance and vkDestroyInstance and cleaned up after that.
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (PaletteGlobal::enableValidationLayers)
        {
            // using validation layers
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            // vkCreateDebugUtilsMessengerEXT requires a valid instance to have been created
            // but there is a way to create a separate debug utils messenger specifically for vkCreateInstance and vkDestroyInstance calls
            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }

    std::vector<const char*> VulkanDevice::_GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (PaletteGlobal::enableValidationLayers) {
            // to set up a callback in the program to handle messages and the associated details
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void VulkanDevice::_ShowAllExtensionsSupport()
    {
        uint32_t extensionCount = 0;
        // request just the number of extensions by leaving the latter parameter empty
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "available extensions:\n";
        for (const auto& extension : extensions)
        {
            std::cout << '\t' << extension.extensionName << '\n';
        }
    }

    bool VulkanDevice::_CheckValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void VulkanDevice::_CreateSurface(GLFWwindow* window)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void VulkanDevice::_PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (_IsDeviceSuitable(device))
            {
                PaletteGlobal::physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        // Use an ordered map to automatically sort candidates by increasing score
        //std::multimap<int, VkPhysicalDevice> candidates;

        //for (const auto& device : devices) 
        //{
        //    int score = rateDeviceSuitability(device);
        //    candidates.insert(std::make_pair(score, device));
        //}

        //// Check if the best candidate is suitable at all
        //if (candidates.rbegin()->first > 0) 
        //{
        //    physicalDevice = candidates.rbegin()->second;
        //}
        //else 
        //{
        //    throw std::runtime_error("failed to find a suitable GPU!");
        //}
    }

    bool VulkanDevice::_IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = _FindQueueFamilies(device);

        bool extensionsSupported = _CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
    }

    bool VulkanDevice::_CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices VulkanDevice::_FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            if (indices.IsComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    int VulkanDevice::_RateDeviceSuitability(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader)
        {
            return 0;
        }

        return score;
    }

    void VulkanDevice::_CreateLogicalDevice()
    {
        QueueFamilyIndices indices = _FindQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        // Previous implementations of Vulkan made a distinction between instance and device specific validation layers, but this is no longer the case
        // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#extendingvulkan-layers-devicelayerdeprecation
        if (PaletteGlobal::enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    void VulkanDevice::_CreateSwapChain(GLFWwindow* window)
    {
        SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = _ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = _ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = _ChooseSwapExtent(swapChainSupport.capabilities, window);

        imageCount = swapChainSupport.capabilities.minImageCount + 1;

        // 0 is a special value that means that there is no maximum
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        // the amount of layers each image consists of
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = _FindQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily)
        {
            // Images can be used across multiple queue families without explicit ownership transfers.
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            // An image is owned by one queue family at a timeand ownership must be explicitly transferred before using it in another queue family.
            // This option offers the best performance.
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        // do not want any transformation
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        // if the alpha channel should be used for blending with other windows
        // ignore the alpha channel
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        // we don't care about the color of pixels that are obscured, for example because another window is in front of them.
        createInfo.clipped = VK_TRUE;
        // it's possible that your swap chain becomes invalid or unoptimized while your application is running, for example because the window was resized
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    VkSurfaceFormatKHR VulkanDevice::_ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            //  The colorSpace member indicates if the SRGB color space is supported or not using the VK_COLOR_SPACE_SRGB_NONLINEAR_KHR flag. 
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanDevice::_ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            // triple buffering
            // VK_PRESENT_MODE_MAILBOX_KHR is a very nice trade-off if energy usage is not a concern. 
            // It allows us to avoid tearing while still maintaining a fairly low latency 
            // by rendering new images that are as up-to-date as possible right until the vertical blank.
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanDevice::_ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
    {
        // some window managers do allow us to differ here and this is indicated by setting the width and height in currentExtent to a special value: the maximum value of uint32_t
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            // if you are using a high DPI display (like Apple's Retina display), screen coordinates don't correspond to pixels
            // we must use glfwGetFramebufferSize to query the resolution of the window in pixel before matching it against the minimum and maximum image extent.
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent =
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    SwapChainSupportDetails VulkanDevice::_QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        // basic surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        // supported surface formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        // available presentation modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void VulkanDevice::_CreateImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    //void VulkanDevice::_CreateRenderPass()
    //{
    //    VkAttachmentDescription colorAttachment{};
    //    colorAttachment.format = swapChainImageFormat;
    //    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //    VkAttachmentReference colorAttachmentRef{};
    //    colorAttachmentRef.attachment = 0;
    //    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //    VkSubpassDescription subpass{};
    //    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    //    subpass.colorAttachmentCount = 1;
    //    subpass.pColorAttachments = &colorAttachmentRef;

    //    VkSubpassDependency dependency{};
    //    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    //    dependency.dstSubpass = 0;
    //    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //    dependency.srcAccessMask = 0;
    //    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    //    VkRenderPassCreateInfo renderPassInfo{};
    //    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    //    renderPassInfo.attachmentCount = 1;
    //    renderPassInfo.pAttachments = &colorAttachment;
    //    renderPassInfo.subpassCount = 1;
    //    renderPassInfo.pSubpasses = &subpass;
    //    renderPassInfo.dependencyCount = 1;
    //    renderPassInfo.pDependencies = &dependency;

    //    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) 
    //    {
    //        throw std::runtime_error("failed to create render pass!");
    //    }
    //}

    //void VulkanDevice::_CreateDescriptorSetLayout()
    //{
    //    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    //    uboLayoutBinding.binding = 0;
    //    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //    uboLayoutBinding.descriptorCount = 1;
    //    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    //    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    //    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    //    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //    layoutInfo.bindingCount = 1;
    //    layoutInfo.pBindings = &uboLayoutBinding;

    //    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    //    {
    //        throw std::runtime_error("failed to create descriptor set layout!");
    //    }
    //}

    //void VulkanDevice::_CreateGraphicsPipeline()
    //{
    //    // Viewports and scissors
    //    VkViewport viewport{};
    //    viewport.x = 0.0f;
    //    viewport.y = 0.0f;
    //    viewport.width = (float)swapChainExtent.width;
    //    viewport.height = (float)swapChainExtent.height;
    //    viewport.minDepth = 0.0f;
    //    viewport.maxDepth = 1.0f;

    //    VkRect2D scissor{};
    //    scissor.offset = { 0, 0 };
    //    scissor.extent = swapChainExtent;

    //    VkPipelineViewportStateCreateInfo viewportState{};
    //    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    //    viewportState.viewportCount = 1;
    //    viewportState.pViewports = &viewport;
    //    viewportState.scissorCount = 1;
    //    viewportState.pScissors = &scissor;

    //    VkPipelineRasterizationStateCreateInfo rasterizer{};
    //    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //    // beyond the near and far planes are clamped to them as opposed to discarding them. 
    //    // This is useful in some special cases like shadow maps.
    //    rasterizer.depthClampEnable = VK_FALSE;
    //    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    //    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    //    rasterizer.lineWidth = 1.0f;
    //    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    //    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    //    rasterizer.depthBiasEnable = VK_FALSE;

    //    VkPipelineMultisampleStateCreateInfo multisampling{};
    //    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    //    multisampling.sampleShadingEnable = VK_FALSE;
    //    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    //    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    //    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    //    colorBlendAttachment.blendEnable = VK_FALSE;

    //    VkPipelineColorBlendStateCreateInfo colorBlending{};
    //    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    //    colorBlending.logicOpEnable = VK_FALSE;
    //    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    //    colorBlending.attachmentCount = 1;
    //    colorBlending.pAttachments = &colorBlendAttachment;
    //    colorBlending.blendConstants[0] = 0.0f;
    //    colorBlending.blendConstants[1] = 0.0f;
    //    colorBlending.blendConstants[2] = 0.0f;
    //    colorBlending.blendConstants[3] = 0.0f;

    //    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    //    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //    pipelineLayoutInfo.setLayoutCount = 1;
    //    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    //    pipelineLayoutInfo.pushConstantRangeCount = 0;

    //    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
    //    {
    //        throw std::runtime_error("failed to create pipeline layout!");
    //    }

    //    // temp
    //    Entity* entity = new Entity();
    //    ModelComponent* model = entity->AddComponent<ModelComponent>();
    //    //PaletteGlobal::client->GetWorld()->AddEntity(entity);
    //    Shader shader = model->GetModelRenderer()->GetMaterial()->GetShaders()[0];

    //    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    //    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    //    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    //    VkVertexInputBindingDescription bindingDescription;
    //    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;

    //    switch (model->GetModelRenderer()->GetMeshes()->GetVertexRenderData().m_VertexFormat)
    //    {
    //    case VertexFormat::VERTEX_P2_C3:
    //        bindingDescription = Vertex_P2_C3::GetBindingDescription();
    //        attributeDescriptions = Vertex_P2_C3::GetAttributeDescriptions();
    //        break;
    //    default:
    //        throw std::runtime_error("failed to create Vertex Description!");
    //        break;
    //    }

    //    vertexInputInfo.vertexBindingDescriptionCount = 0;
    //    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    //    vertexInputInfo.vertexBindingDescriptionCount = 1;
    //    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    //    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    //    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    //    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    //    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    //    inputAssembly.primitiveRestartEnable = VK_FALSE;

    //    VkGraphicsPipelineCreateInfo pipelineInfo{};
    //    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    //    pipelineInfo.stageCount = 2;
    //    pipelineInfo.pStages = shader->GetShaderModule(0)->GetShaderStages();
    //    pipelineInfo.pVertexInputState = &vertexInputInfo;
    //    pipelineInfo.pInputAssemblyState = &inputAssembly;
    //    pipelineInfo.pViewportState = &viewportState;
    //    pipelineInfo.pRasterizationState = &rasterizer;
    //    pipelineInfo.pMultisampleState = &multisampling;
    //    //pipelineInfo.pDepthStencilState = nullptr; // Optional
    //    pipelineInfo.pColorBlendState = &colorBlending;
    //    //pipelineInfo.pDynamicState = nullptr; // Optional
    //    pipelineInfo.layout = pipelineLayout;
    //    pipelineInfo.renderPass = renderPass;
    //    pipelineInfo.subpass = 0;
    //    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    //    //pipelineInfo.basePipelineIndex = -1; // Optional

    //    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) 
    //    {
    //        throw std::runtime_error("failed to create graphics pipeline!");
    //    }
    //}

    //void VulkanDevice::_CreateFramebuffers()
    //{
    //    swapChainFramebuffers.resize(swapChainImageViews.size());

    //    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    //    {
    //        VkImageView attachments[] =
    //        {
    //            swapChainImageViews[i]
    //        };

    //        VkFramebufferCreateInfo framebufferInfo{};
    //        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    //        framebufferInfo.renderPass = renderPass;
    //        framebufferInfo.attachmentCount = 1;
    //        framebufferInfo.pAttachments = attachments;
    //        framebufferInfo.width = swapChainExtent.width;
    //        framebufferInfo.height = swapChainExtent.height;
    //        framebufferInfo.layers = 1;

    //        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
    //        {
    //            throw std::runtime_error("failed to create framebuffer!");
    //        }
    //    }
    //}

    void VulkanDevice::_CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = _FindQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = 0; // Optional

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void VulkanDevice::_CreateCommandBuffers()
    {
        commandBuffers.resize(swapChainImageViews.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        beginInfo = VkCommandBufferBeginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        //for (size_t i = 0; i < commandBuffers.size(); i++) 
        //{
        //    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) 
        //    {
        //        throw std::runtime_error("failed to begin recording command buffer!");
        //    }

        //    VkRenderPassBeginInfo renderPassInfo{};
        //    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        //    renderPassInfo.renderPass = renderPass;
        //    renderPassInfo.framebuffer = swapChainFramebuffers[i];
        //    renderPassInfo.renderArea.offset = { 0, 0 };
        //    renderPassInfo.renderArea.extent = swapChainExtent;

        //    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        //    renderPassInfo.clearValueCount = 1;
        //    renderPassInfo.pClearValues = &clearColor;

        //    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        //    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        //    RenderMesh mesh;
        //    /*for (auto entity : PaletteGlobal::client->GetWorld()->GetEntityList())
        //    {
        //        mesh = entity.second->GetComponent<ModelComponent>()->GetModelRenderer()->GetAllMeshes()[0];
        //    }*/
        //    Entity* entity = new Entity();
        //    mesh = entity->AddComponent<ModelComponent>()->GetModelRenderer()->GetAllMeshes()[0];
        //    
        //    VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer() };
        //    VkBuffer indexBuffer = mesh->GetIndexBuffer();
        //    VkDeviceSize offsets[] = { 0 };

        //    vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
        //    vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        //    //uint32_t vertexDataSize = static_cast<uint32_t>(mesh->GetVertexData().size());
        //    //vkCmdDraw(commandBuffers[i], vertexDataSize, 1, 0, 0);
        //    uint32_t indexDataSize = static_cast<uint32_t>(mesh->GetIndexData().size());
        //    vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indexDataSize), 1, 0, 0, 0);

        //    vkCmdEndRenderPass(commandBuffers[i]);

        //    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) 
        //    {
        //        throw std::runtime_error("failed to record command buffer!");
        //    }
        //}
    }

    void VulkanDevice::_CreateSyncObjects()
    {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // one semaphore to signal that an image has been acquired and is ready for rendering, 
        // and another one to signal that rendering has finished and presentation can happen
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create semaphores for a frame!");
            }
        }
    }
}
