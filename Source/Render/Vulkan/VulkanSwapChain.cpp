#include <algorithm>
#include "VulkanSwapChain.h"
#include "VulkanGlobal.h"
#include "VulkanUtilities.h"

namespace Palette
{
    using PaletteGlobal::device;
    VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface)
    {
        this->surface = surface;

        _CreateSwapChain();
        _CreateImageViews();
    }

    void VulkanSwapChain::CreateFramebuffers(VkRenderPass& renderPass)
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

            VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]))
        }
    }

    void VulkanSwapChain::FreeFramebuffers()
    {
        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
    }

    void VulkanSwapChain::_CreateImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            swapChainImageViews[i] = CreateImageView(swapChainImages[i], swapChainImageFormat);
        }
    }

    SwapChainSupportDetails VulkanSwapChain::_QuerySwapChainSupport(VkPhysicalDevice device)
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

    VkSurfaceFormatKHR VulkanSwapChain::_ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

    VkPresentModeKHR VulkanSwapChain::_ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

    VkExtent2D VulkanSwapChain::_ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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
            glfwGetFramebufferSize(PaletteGlobal::window, &width, &height);

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

    void VulkanSwapChain::_CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(PaletteGlobal::physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = _ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = _ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = _ChooseSwapExtent(swapChainSupport.capabilities);

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

        QueueFamilyIndices indices = FindQueueFamilies(PaletteGlobal::physicalDevice, surface);
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

        VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain))

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void VulkanSwapChain::CleanupSwapChain()
    {
        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        auto& commandBuffers = PaletteGlobal::vulkanDevice->GetCommandBuffers();
        vkFreeCommandBuffers(device, PaletteGlobal::commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        //vkDestroyPipeline(device, graphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        //vkDestroyRenderPass(device, renderPass, nullptr);

        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void VulkanSwapChain::ResizeSwapChain()
    {
        CleanupSwapChain();

        _CreateSwapChain();
        _CreateImageViews();
    }
}