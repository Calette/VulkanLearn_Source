#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
// provides the EXIT_SUCCESS and EXIT_FAILURE macros
#include <cstdlib>

#include "Render/Vulkan/VulkanGlobal.h"
#include "Render/Vulkan/VulkanDevice.h"
#include "Client/Client.h"

namespace Palette
{
    class PaletteEngine
    {
    public:
        void Run()
        {
            _Init();
            _MainLoop();
            _Cleanup();
        }

    private:
        GLFWwindow* window;

        void _Init()
        {
            _InitWindow();
            _InitVulkan();
            _InitClient();
        }

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
            PaletteGlobal::vulkanDevice = new VulkanDevice(window);
        }

        void _InitClient()
        {
            PaletteGlobal::client = new Client();
        }

        void _Update_ot()
        {
            PaletteGlobal::client->Update_ot();
        }

        void _Render_rt()
        {
            PaletteGlobal::vulkanDevice->PreRender_rt();
            PaletteGlobal::client->Render_rt();
            PaletteGlobal::vulkanDevice->PostRender_rt();
            PaletteGlobal::vulkanDevice->DrawFrame_rt();
        }

        void _MainLoop()
        {
            while (!glfwWindowShouldClose(window))
            {
                // checks for events like pressing the X button until the window has been closed by the user
                glfwPollEvents();

                _Update_ot();
                _Render_rt();

                PaletteGlobal::frameCount++;
            }

            // wait for the logical device to finish operations before exiting mainLoop and destroying the window
            vkDeviceWaitIdle(PaletteGlobal::device);
        }

        void _Cleanup()
        {
            delete PaletteGlobal::client;
            delete PaletteGlobal::vulkanDevice;
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
