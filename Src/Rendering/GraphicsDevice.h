#pragma once

#include "Src/Core/Array.h"
#include <vulkan/vulkan.h>

class GraphicsDevice;
GraphicsDevice* GetGraphicsDevice();

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR caps;
	VkSurfaceFormatKHR formats[64];
	uint32_t formatCount;
	VkPresentModeKHR presentModes[64];
	uint32_t presentModeCount;
};

struct QueueFamilyIndices
{
	QueueFamilyIndices()
		: graphicsFamily(-1)
		, presentFamily(-1) { }

	int graphicsFamily;
	int presentFamily;

	bool IsComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
};

class GraphicsDevice
{
public:
	GraphicsDevice();

	void Init(const Array<const char*>& extraExts);

	VkInstance GetVulkanInstance() const { return m_Instance; }
	void SetWindowSurface(VkSurfaceKHR surface);

private:
	VkInstance m_Instance;
#ifndef NDEBUG
	VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
	VkSurfaceKHR m_Surface;
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_Device;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;

	Array<const char*> m_InstanceExtensions;
	Array<const char*> m_DeviceExtensions;

	bool CheckValidationLayers();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
};