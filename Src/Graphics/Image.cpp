#include "Image.h"

Image::Image()
	: m_Image(NULL)
{
}

void Image::InitWithVkImage(VkImage image)
{
	m_Image = image;
}