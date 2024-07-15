// PaintBlast.cpp : Defines the entry point for the application.
//

#include "PaintBlast.h"
#include "Src/Core/Array.h"
#include "Src/Core/String.h"
#include "Src/Rendering/GraphicsDevice.h"

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

	uint32_t count = 0;
	const char** glfwExts = glfwGetRequiredInstanceExtensions(&count);

	Array<const char*> reqExts;
	for (uint32_t i = 0; i < count; ++i)
		reqExts.PushBack(glfwExts[i]);

	GetGraphicsDevice()->Init(reqExts);

	WindowLoop();

	return 0;
}