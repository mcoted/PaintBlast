#include "ImageView.h"
#include "Src/Core/Log.h"
#include "Src/Rendering/GraphicsDevice.h"

ImageView::ImageView()
    : m_Image()
    , m_ImageView(NULL)
    , m_Format(VK_FORMAT_UNDEFINED)
{
}

void ImageView::InitWithImage(Image image, VkFormat format)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image.GetVkImage();
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(GetGraphicsDevice()->GetLogicalDevice(), &createInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        LogErrorAndAbort("Failed to create image view\n");

    m_Image = image;
    m_Format = format;
}