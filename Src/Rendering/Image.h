#pragma once

#include "Src/Core/Utils.h"
#include <vulkan/vulkan.h>

class Image
{
public:
	Image();

	VkFormat GetFormat() const { return m_Format; }
	Int32 GetWidth() const { return m_Width; }
	Int32 GetHeight() const { return m_Height; }

	void InitWithVkImage(VkImage image);
	VkImage GetVkImage() const { return m_Image; }

private:
	VkImage m_Image;
	VkFormat m_Format;
	Int32 m_Width;
	Int32 m_Height;
};