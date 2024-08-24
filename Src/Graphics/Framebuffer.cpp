#include "Framebuffer.h"
#include "Src/Graphics/GraphicsDevice.h"

Framebuffer::Framebuffer()
	: m_Framebuffer(NULL)
	, m_RenderPass(NULL)
	, m_Attachements(Allocator::Persistent)
{
}

void Framebuffer::InitWithRenderPassAndAttachments(RenderPass& renderPass, const Array<ImageView>& imageViews)
{
	Assert(imageViews.Count() > 0);

	UInt32 width = imageViews[0].GetImage().GetWidth();
	UInt32 height = imageViews[0].GetImage().GetHeight();

	Array<VkImageView> tmpAttachments(Allocator::TempStack);
	for (int i = 0; i < imageViews.Count(); ++i)
		tmpAttachments.PushBack(imageViews[i].GetVkImageView());

	VkFramebufferCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = renderPass.GetVkRenderPass();
	createInfo.attachmentCount = imageViews.Count();
	createInfo.pAttachments = tmpAttachments.Data();
	createInfo.width = width;
	createInfo.height = height;
	createInfo.layers = 1;

	if (vkCreateFramebuffer(GetGraphicsDevice()->GetLogicalDevice(), &createInfo, NULL, &m_Framebuffer) != VK_SUCCESS)
		LogErrorAndAbort("Failed to create framebuffer\n");

	m_RenderPass = &renderPass;
	m_Attachements = imageViews;
}
