#pragma once

#include "Src/Core/Array.h"
#include <vulkan/vulkan.h>

class GraphicsDevice;
GraphicsDevice* GetGraphicsDevice();

class GraphicsDevice
{
public:
	GraphicsDevice();

	void Init(const Array<const char*>& extraExts);

private:
	VkInstance m_Instance;
#ifndef NDEBUG
	VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif

	Array<const char*> m_InstanceExtensions;
	Array<const char*> m_DeviceExtensions;

	bool CheckValidationLayers();
	void SetupDebugMessenger();

};