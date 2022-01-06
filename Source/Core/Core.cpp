#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
// provides the EXIT_SUCCESS and EXIT_FAILURE macros
#include <cstdlib>

#include "Render/Vulkan/VulkanGlobal.h"
#include "Render/Vulkan/VulkanDevice.h"
#include "Common/Guid.h"

namespace Palette
{
    class PaletteEngine
    {
    public:
        void Run()
        {
            _InitWindow();
            _InitWorld();
            _InitVulkan();
            _MainLoop();
            _Cleanup();
        }

    private:
        GLFWwindow* window;
        VulkanDevice* vulkanDevice;

        void _InitWindow()
        {
            glfwInit();
            // GLFW was originally designed to create an OpenGL context, we need to tell it to not create an OpenGL context with a subsequent call
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            window = glfwCreateWindow(PaletteGlobal::WIDTH, PaletteGlobal::HEIGHT, "Palette", nullptr, nullptr);
        }

        void _InitVulkan()
        {
            vulkanDevice = new VulkanDevice(window);
        }

        void _InitWorld()
        {
            uuid4_init();

            PaletteGlobal::world = new World();
            PaletteGlobal::world->Initialize();
        }

        void _Tick_ot()
        {
            PaletteGlobal::world->Tick_ot();


        }

        void _Render_rt()
        {


            vulkanDevice->DrawFrame();
        }

        void _MainLoop()
        {
            while (!glfwWindowShouldClose(window))
            {
                // checks for events like pressing the X button until the window has been closed by the user
                glfwPollEvents();

                _Tick_ot();
                _Render_rt();
            }

            // wait for the logical device to finish operations before exiting mainLoop and destroying the window
            vkDeviceWaitIdle(PaletteGlobal::device);
        }

        void _Cleanup()
        {
            delete PaletteGlobal::world;
            delete vulkanDevice;
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    };
}

int main()
{
    Palette::PaletteEngine app;
    try
    {
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
