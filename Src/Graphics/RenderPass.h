#pragma once

#include "Src/Core/Utils.h"
#include "Src/Graphics/SwapChain.h"
#include <vulkan/vulkan.h>

class RenderPass
{
public:
	RenderPass();

	void InitWithSwapChain(const SwapChain& swapChain);

	VkRenderPass GetVkRenderPass() const { return m_RenderPass; }

private:
	VkRenderPass m_RenderPass;
};