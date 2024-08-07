#pragma once

#include "Src/Core/Utils.h"
#include "Src/Rendering/Image.h"
#include <vulkan/vulkan.h>

class ImageView
{
public:
	ImageView();

	void InitWithImage(Image image, VkFormat format);

	Image GetImage() const { return m_Image; }
	VkImageView GetImageView() const { return m_ImageView; }
	VkFormat GetFormat() const { return m_Format; }

private:
	Image m_Image; // The underlying image
	VkImageView m_ImageView;
	VkFormat m_Format;
};