#ifndef VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#endif
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

constexpr uint32_t WIDTH  = 800;
constexpr uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:

    GLFWwindow *window = nullptr;

    // RAII Vulkan Objects
    vk::raii::Context  context;
    vk::raii::Instance instance = nullptr;

    void initWindow() {

        glfwInit();

        // GLFW was designed for OpenGL, so we must explicitly tell it NOT to create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Disable resizing, look into it later
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Initializing the window
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {

        createInstance();
        setupDebugMessenger();
    }

    std::vector<const char*> getRequiredInstanceExtensions() {

        // Get the required instance extensions from GLFW.
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
        }

        // Contacts the Vulkan loader to retrieve a list of every instance-level extension the machine supports
        auto extensionProperties = context.enumerateInstanceExtensionProperties();

        // TODO: Follow best practices here
        // Check if the required extensions are supported by the Vulkan implementation.
        for (const auto& extension : extensions) {

            bool extensionFound = false;
            for (const auto& extensionProperty : extensionProperties) {
                if (strcmp(extensionProperty.extensionName, extension) == 0) {
                    extensionFound = true;
                    break;
                }
            }
            if (!extensionFound) {
                throw std::runtime_error("Required extension not supported: " + std::string(extension));
            }
        }

        return extensions;
    }

    void createInstance() {

        // Information about our application (Optional but recommended)
        constexpr vk::ApplicationInfo appInfo {
            .pApplicationName = "Hello Triangle",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = vk::ApiVersion14
        };

        auto requiredExtensions = getRequiredInstanceExtensions();

        // It tells the Vulkan driver which global extensions and validation layers we want to use
        vk::InstanceCreateInfo createInfo {
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data()
        };

        // Finally create instance
        instance = vk::raii::Instance(context, createInfo);
    }

    void setupDebugMessenger() {

    }

    void mainLoop() {

        bool running = true;
        while (running) {
            glfwPollEvents();

            // Check for exit
            if (glfwWindowShouldClose(window)) {
                running = false;
                break;
            }
        }
    }

    void cleanup() {

        // Terminate GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main(int argc, char* argv[]) {

    try {
        HelloTriangleApplication app;
        app.run();
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
