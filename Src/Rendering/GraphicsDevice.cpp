#pragma once

#include "GraphicsDevice.h"
#include "Src/Core/Log.h"
#include "Src/Core/Utils.h"
#include "Src/Rendering/SwapChain.h"

#include <vulkan/vulkan.h>
#include <string.h>


// Extension functions loaders
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

GraphicsDevice* g_MainGraphicsDevice = NULL;

GraphicsDevice* GetGraphicsDevice()
{
    if (g_MainGraphicsDevice == NULL)
        g_MainGraphicsDevice = new GraphicsDevice();

    return g_MainGraphicsDevice;
}

GraphicsDevice::GraphicsDevice()
    : m_InstanceExtensions(Allocator::Persistent)
    , m_DeviceExtensions(Allocator::Persistent)
    , m_QuerySurfaceSizeCallback(NULL)
{
    m_Instance = NULL;
}

void GraphicsDevice::Init(const Array<const char*>& requiredExtensions)
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "PaintBlast";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "PaintBlastEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Display available extensions
    uint32_t extCount = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr) == VK_SUCCESS)
    {
        Array<VkExtensionProperties> props(extCount, Allocator::TempStack);
        if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, props.Data()) == VK_SUCCESS)
        {
            printf("Vulkan Extensions:\n");
            for (uint32_t i = 0; i < extCount; ++i)
                printf(" %2d: %s\n", i, props[i].extensionName);
        }
    }

    if (!CheckValidationLayers())
        LogErrorAndAbort("Validation layer not found!\n");

    for (int i = 0; i < requiredExtensions.Count(); ++i)
        m_InstanceExtensions.PushBack(requiredExtensions[i]);

    m_InstanceExtensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    m_DeviceExtensions.PushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    m_DeviceExtensions.PushBack(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);


    createInfo.enabledExtensionCount = m_InstanceExtensions.Count();
    createInfo.ppEnabledExtensionNames = m_InstanceExtensions.Data();

#ifndef NDEBUG
    static const char* validationLayer = "VK_LAYER_KHRONOS_validation";
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = &validationLayer;
#else
    createInfo.enabledLayerCount = 0;
#endif

    if (vkCreateInstance(&createInfo, NULL, &m_Instance) != VK_SUCCESS)
        LogErrorAndAbort("vkCreateInstance failed\n");

    SetupDebugMessenger();
}

void GraphicsDevice::SetWindowSurface(VkSurfaceKHR surface)
{
    m_Surface = surface;

    // Once we have a surface, we can pick a physical device
    // that matches the required surface properties
    PickPhysicalDevice();
    
    CreateLogicalDevice();
    CreateSwapChain();
}

bool GraphicsDevice::CheckValidationLayers()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    Array<VkLayerProperties> availableLayers(layerCount, Allocator::TempStack);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

    for (uint32_t i = 0; i < layerCount; ++i)
    {
        if (strcmp(availableLayers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0)
            return true;
    }

    return false;
}

#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    LogError("Validation layer debug message:\n%s\n", pCallbackData->pMessage);
    return VK_FALSE;
}
#endif

void GraphicsDevice::SetupDebugMessenger()
{
#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = VulkanDebugCallback;
    createInfo.pUserData = nullptr; // Optional

    if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        LogErrorAndAbort("Failed to set up debug messenger\n");
#endif
}

void GraphicsDevice::PickPhysicalDevice()
{
    m_PhysicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        LogErrorAndAbort("Failed to find GPUs with Vulkan support\n");

    Array<VkPhysicalDevice> devices(deviceCount, Allocator::TempStack);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.Data());

    for (uint32_t i = 0; i < deviceCount; ++i)
    {
        auto device = devices[i];
        if (IsDeviceSuitable(device))
        {
            m_PhysicalDevice = device;
            break;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE)
        LogErrorAndAbort("Failed to find a suitable GPU!\n");
}

void GraphicsDevice::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

    // Poor man's std::set
    int uniqueIndices[2];
    uniqueIndices[0] = indices.graphicsFamily;
    int uniqueCount = 1;
    if (indices.graphicsFamily != indices.presentFamily)
    {
        uniqueIndices[1] = indices.presentFamily;
        ++uniqueCount;
    }

    VkDeviceQueueCreateInfo createInfos[2];
    uint32_t createInfoCount = 0;

    float queuePriority = 1.0f;
    for (int i = 0; i < uniqueCount; ++i)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueIndices[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        createInfos[createInfoCount++] = queueCreateInfo;
    }


    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = createInfoCount;
    createInfo.pQueueCreateInfos = createInfos;
    createInfo.pEnabledFeatures = &deviceFeatures;

    VkPhysicalDeviceSynchronization2Features sync2Features{};
    sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2Features.synchronization2 = VK_TRUE;
    createInfo.pNext = &sync2Features;

    createInfo.ppEnabledExtensionNames = m_DeviceExtensions.Data();
    createInfo.enabledExtensionCount = m_DeviceExtensions.Count();

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
        LogErrorAndAbort("Failed to create logical device\n");

    vkGetDeviceQueue(m_Device, indices.graphicsFamily, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, indices.presentFamily, 0, &m_PresentQueue);
}

void GraphicsDevice::CreateSwapChain()
{
    m_SwapChain.reset(new SwapChain());
}

QueueFamilyIndices GraphicsDevice::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    Array<VkQueueFamilyProperties> queueFamilies(queueFamilyCount, Allocator::TempStack);

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.Data());
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;

        if (indices.IsComplete())
            break;
    }

    return indices;
}

bool GraphicsDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    auto indices = FindQueueFamilies(device);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails details = SwapChain::QuerySwapChainSupport(device, m_Surface);
        swapChainAdequate = details.formatCount > 0 && details.presentModeCount > 0;
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool GraphicsDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    Array<VkExtensionProperties> props(extensionCount, Allocator::TempStack);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, props.Data());

    int matchCount = 0;
    for (int i = 0; i < (int)extensionCount; ++i)
    {
        auto prop = props[i];
        for (int j = 0; j < m_DeviceExtensions.Count(); ++j)
        {
            if (strcmp(m_DeviceExtensions[j], prop.extensionName) == 0)
                ++matchCount;
        }
    }

    return (matchCount == m_DeviceExtensions.Count());
}
