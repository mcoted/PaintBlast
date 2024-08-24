#pragma once

#include "Src/Core/Array.h"
#include "Src/Core/Utils.h"
#include "Src/Graphics/ImageView.h"
#include "Src/Graphics/RenderPass.h"
#include <vulkan/vulkan.h>

class Framebuffer
{
public:
	Framebuffer();

	void InitWithRenderPassAndAttachments(RenderPass& renderPass, const Array<ImageView>& imageViews);

	const RenderPass* GetRenderPass() const { return m_RenderPass; }
	const Array<ImageView>& GetAttachments() const { return m_Attachements; }

private:
	VkFramebuffer m_Framebuffer;
	RenderPass* m_RenderPass;
	Array<ImageView> m_Attachements;
};