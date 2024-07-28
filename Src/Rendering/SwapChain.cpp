#pragma once

#include "SwapChain.h"
#include "GraphicsDevice.h"
#include "Src/Core/Log.h"

SwapChain::SwapChain()
    : m_SwapChain(NULL)
{
    Create();
}

SwapChainSupportDetails SwapChain::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.caps);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount >= COUNTOF(details.formats))
        LogErrorAndAbort("Too many surface formats to fit in buffer, please increase 'SwapChainSupportDetails.formats'\n");

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats);
    details.formatCount = formatCount;

    uint32_t modeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, nullptr);

    if (modeCount >= COUNTOF(details.presentModes))
        LogErrorAndAbort("Too many surface preset modes to fit in buffer, please increase 'SwapChainSupportDetails.presentModes'\n");

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, details.presentModes);
    details.presentModeCount = modeCount;

    return details;

}

static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR* formats, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        auto f = formats[i];
        if (f.format == VK_FORMAT_R8G8B8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return f;
    }

    return formats[0];
}

static VkPresentModeKHR ChooseSwapPresentMode(VkPresentModeKHR* modes, uint32_t count)
{
    return VK_PRESENT_MODE_FIFO_KHR; // Guaranteed to be present
}

static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& caps)
{
    if (caps.currentExtent.width != INT_MAX)
    {
        return caps.currentExtent;
    }
    else
    {
        int width;
        int height;
        GetGraphicsDevice()->QuerySurfaceSize(width, height);

        VkExtent2D actualExtent{ (uint32_t)width, (uint32_t)height };
        actualExtent.width = CLAMP(actualExtent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
        actualExtent.height = CLAMP(actualExtent.height, caps.minImageExtent.height, caps.maxImageExtent.height);

        return actualExtent;
    }
}

void SwapChain::Create()
{
    GraphicsDevice* gfx = GetGraphicsDevice();
    VkDevice device = gfx->GetLogicalDevice();

    SwapChainSupportDetails sup = QuerySwapChainSupport(gfx->GetPhysicalDevice(), gfx->GetWindowSurface());
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(sup.formats, sup.formatCount);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(sup.presentModes, sup.presentModeCount);
    VkExtent2D extent = ChooseSwapExtent(sup.caps);

    uint32_t imageCount = sup.caps.minImageCount + 1;
    if (sup.caps.maxImageCount > 0 && imageCount > sup.caps.maxImageCount)
        imageCount = sup.caps.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = gfx->GetWindowSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = gfx->FindQueueFamilies(gfx->GetPhysicalDevice());
    uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = sup.caps.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_FALSE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
        LogErrorAndAbort("Failed to create swap chain\n");

    //vkGetSwapchainImagesKHR(device, m_SwapChain, &m_SwapChainImageCount, nullptr);
    //if (m_SwapChainImageCount > COUNTOF(m_SwapChainImages))
    //{
    //    fprintf(stderr, "Not enough `m_SwapChainImages`\n");
    //    exit(0);
    //}

    //vkGetSwapchainImagesKHR(gfx->GetLogicalDevice(), m_SwapChain, &m_SwapChainImageCount, m_SwapChainImages);

    //m_SwapChainImageFormat = surfaceFormat.format;
    //m_SwapChainExtent = extent;
}