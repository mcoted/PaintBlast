#pragma once

#include "Src/Core/Utils.h"
#include "Src/Rendering/Image.h"
#include "Src/Rendering/ImageView.h"
#include <vulkan/vulkan.h>

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR caps;
	VkSurfaceFormatKHR formats[64];
	uint32_t formatCount;
	VkPresentModeKHR presentModes[64];
	uint32_t presentModeCount;
};

class SwapChain
{
public:
	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

	SwapChain(Int32 width, Int32 height);

private:
	Int32 m_Width;
	Int32 m_Height;

	VkSwapchainKHR m_SwapChain;

	Image m_Images[8];
	ImageView m_ImageViews[8];
	VkImage m_SwapChainImages[8];
	UInt32 m_SwapChainImageCount;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;


	void Create();
};