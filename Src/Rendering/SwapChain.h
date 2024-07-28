#pragma once

#include "Src/Core/Utils.h"
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

	SwapChain();

private:
	VkSwapchainKHR m_SwapChain;
	UInt32 m_SwapChainImageCount;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;

	void Create();
};