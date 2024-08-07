#pragma once

#include "Src/Core/Utils.h"
#include <vulkan/vulkan.h>

class Framebuffer
{
public:
	Framebuffer();

private:
	VkFramebuffer m_Framebuffer;
};