#pragma once

#include "GraphicsDevice.h"
#include "Src/Main/Logger.h"
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
{
    m_Instance = NULL;
}

void GraphicsDevice::Init(const Array<const char*>& extraExts)
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
        const int kMaxExtensions = 64;
        VkExtensionProperties props[kMaxExtensions];

        if (extCount >= kMaxExtensions)
        {
            fprintf(stderr, "Too many extensions to fit in buffer, please increase 'kMaxExtensions'\n");
            exit(0);
        }

        if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, props) == VK_SUCCESS)
        {
            printf("Vulkan Extensions:\n");
            for (uint32_t i = 0; i < extCount; ++i)
                printf(" %2d: %s\n", i, props[i].extensionName);
        }
    }

    if (!CheckValidationLayers())
    {
        fprintf(stderr, "Validation layer not found!\n");
        exit(0);
    }

    for (int i = 0; i < extraExts.Count(); ++i)
        m_InstanceExtensions.PushBack((const char*)extraExts[i]);

    m_InstanceExtensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    m_DeviceExtensions.PushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    m_DeviceExtensions.PushBack(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

    createInfo.enabledExtensionCount = m_InstanceExtensions.Count();
    createInfo.ppEnabledExtensionNames = (const char* const *)m_InstanceExtensions.Data();

#ifndef NDEBUG
    static const char* validationLayer = "VK_LAYER_KHRONOS_validation";
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = &validationLayer;
#else
    createInfo.enabledLayerCount = 0;
#endif

    if (vkCreateInstance(&createInfo, NULL, &m_Instance) != VK_SUCCESS)
        LogErrorAndAbort("vkCreateInstance failed");

    SetupDebugMessenger();
}

bool GraphicsDevice::CheckValidationLayers()
{
    const int kMaxLayerCount = 64;
    VkLayerProperties availableLayers[kMaxLayerCount];

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    if (layerCount >= kMaxLayerCount)
    {
        fprintf(stderr, "Too many validation layers to fit in buffer, please increase 'kMaxLayerCount'\n");
        exit(0);
    }

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
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
        LogErrorAndAbort("Failed to set up debug messenger");
#endif
}