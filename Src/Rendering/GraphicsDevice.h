#pragma once

#include "Src/Core/Array.h"
#include <vulkan/vulkan.h>

class GraphicsDevice;
GraphicsDevice* GetGraphicsDevice();

struct QueueFamilyIndices
{
	QueueFamilyIndices()
		: graphicsFamily(-1)
		, presentFamily(-1) { }

	int graphicsFamily;
	int presentFamily;

	bool IsComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
};

typedef void (*QuerySurfaceSizeFunc)(int& width, int& height);

class GraphicsDevice
{
public:
	GraphicsDevice();

	void Init(const Array<const char*>& extraExts);

	VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
	VkDevice GetLogicalDevice() const { return m_Device; }
	VkSurfaceKHR GetWindowSurface() const { return m_Surface; }

	VkInstance GetVulkanInstance() const { return m_Instance; }
	void SetWindowSurface(VkSurfaceKHR surface);

	void SetQuerySurfaceSizeCallback(QuerySurfaceSizeFunc func) { m_QuerySurfaceSizeCallback = func; }
	void QuerySurfaceSize(int& width, int& height) const { if (m_QuerySurfaceSizeCallback != NULL) m_QuerySurfaceSizeCallback(width, height); }

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

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
	
	QuerySurfaceSizeFunc m_QuerySurfaceSizeCallback;

	Array<const char*> m_InstanceExtensions;
	Array<const char*> m_DeviceExtensions;

	bool CheckValidationLayers();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();

	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
};