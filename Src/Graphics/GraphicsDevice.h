#pragma once

#include "Src/Core/Array.h"
#include <vulkan/vulkan.h>
#include <memory>

class GraphicsDevice;
GraphicsDevice* GetGraphicsDevice();

class SwapChain;

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

	void SetTargetSwapChainSize(int width, int height);
	void GetTargetSwapChainSize(int* width, int* height) { *width = m_TargetSwapChainWidth, * height = m_TargetSwapChainHeight; }

	VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
	VkDevice GetLogicalDevice() const { return m_Device; }
	VkSurfaceKHR GetWindowSurface() const { return m_Surface; }

	VkInstance GetVulkanInstance() const { return m_Instance; }
	void SetWindowSurface(VkSurfaceKHR surface);

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
	
	Array<const char*> m_InstanceExtensions;
	Array<const char*> m_DeviceExtensions;

	std::unique_ptr<SwapChain> m_SwapChain;

	Int32 m_TargetSwapChainWidth;
	Int32 m_TargetSwapChainHeight;

	bool CheckValidationLayers();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void RecreateSwapChain(int width, int height);

	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
};