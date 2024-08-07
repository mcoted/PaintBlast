// PaintBlast.cpp : Defines the entry point for the application.
//

#include "PaintBlast.h"
#include "Src/Core/Array.h"
#include "Src/Core/String.h"
#include "Src/Core/Log.h"
#include "Src/Core/Memory.h"
#include "Src/Graphics/GraphicsDevice.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static GLFWwindow* s_Window = NULL;

static void WindowLoop()
{
	while (!glfwWindowShouldClose(s_Window))
	{
		glfwPollEvents();
	}
}

// Debug builds are a console application.
// Release builds are a window application to avoid showing the console.
// See Project Properties > Linker > System
#ifdef NDEBUG
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	s_Window = glfwCreateWindow(1024, 768, "PaintBlast", NULL, NULL);

	int width, height;
	glfwGetFramebufferSize(s_Window, &width, &height);

	uint32_t count = 0;
	const char** glfwExts = glfwGetRequiredInstanceExtensions(&count);

	GraphicsDevice* gfx = GetGraphicsDevice();

	{
		// Careful not to create long-lived temp-stack allocs in the main function
		Array<const char*> reqExts(Allocator::TempStack);
		for (uint32_t i = 0; i < count; ++i)
			reqExts.PushBack(glfwExts[i]);

		gfx->Init(reqExts);
		gfx->SetTargetSwapChainSize(width, height);
	}

	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(gfx->GetVulkanInstance(), s_Window, nullptr, &surface) != VK_SUCCESS)
		LogErrorAndAbort("Failed to create window surface\n");
	gfx->SetWindowSurface(surface);

	WindowLoop();

	return 0;
}