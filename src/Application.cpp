#include "Application.hpp"
#include "BatchRenderer.hpp"
#include "Shader.hpp"
#include <glad/glad.h>   // ALWAYS include your OpenGL loader before GLFW
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

namespace Gui {

    // Global C-style callback required by GLFW to pipe OS resize events to our engine
    static void WindowResizeCallback(GLFWwindow* window, int width, int height) {
        auto* appPtr = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (appPtr) {
            appPtr->DispatchResize(width, height);
        }
    }

    Application::Application(ApplicationConfig config) : m_Config(std::move(config)) {
        if (!InitWindowContext()) {
            throw std::runtime_error("Critical Error: Failed to initialize Platform Window Context.");
        }
    }

    Application::~Application() {
        ShutdownWindowContext();
    }

    bool Application::InitWindowContext() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW!\n";
            return false;
        }

        // Request a Modern OpenGL 3.3 Core Profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS portability
        #endif

        // ====================================================================
        // --- FIXED: BORDERLESS WINDOW HINTS PREVENT VIEWPORT SQUISHING -----
        // ====================================================================
        // Disabling decoration strips the OS title bar, caption height, and native 
        // side frames. This lets 1920x1080 sit at a native, pixel-perfect 1:1 scale!
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); 
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Create the physical window framework handle
        m_WindowHandle = glfwCreateWindow(
            m_Config.width, 
            m_Config.height, 
            m_Config.title.c_str(), 
            nullptr, 
            nullptr
        );

        if (!m_WindowHandle) {
            std::cerr << "Failed to create GLFW window handle!\n";
            glfwTerminate();
            return false;
        }

        // Lock the window position flush to (0,0) to sit perfectly clean inside the bezel
        glfwSetWindowPos(m_WindowHandle, 0, 0);

        glfwMakeContextCurrent(m_WindowHandle);

        // Initialize GLAD to map modern OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD/OpenGL context loaders!\n";
            return false;
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwSwapInterval(m_Config.vsync ? 1 : 0);

        // Attach 'this' pointer into the window handle context for callbacks
        glfwSetWindowUserPointer(m_WindowHandle, this);
        glfwSetFramebufferSizeCallback(m_WindowHandle, WindowResizeCallback);

        return true;
    }

    void Application::ShutdownWindowContext() {
        if (m_WindowHandle) {
            glfwDestroyWindow(m_WindowHandle);
            m_WindowHandle = nullptr;
        }
        glfwTerminate();
    }

    int Application::Run() 
    {
        float lastFrame = 0.0f;

        // Modern Core Profiles discard vertices unless a global VAO is bound!
        // This ensures the hardware accepts your batcher attributes natively.
        GLuint globalVAO;
        glGenVertexArrays(1, &globalVAO);
        glBindVertexArray(globalVAO);

        while (!glfwWindowShouldClose(m_WindowHandle)) 
        {
            // ====================================================================
            // --- FIXED: EMERGENCY KEY EXIT CLOSES BORDERLESS WINDOW -----------
            // ====================================================================
            // Since title bars are removed, pressing the ESCAPE key safely intercepts 
            // the runtime state loop to terminate your engine session cleanly!
            if (glfwGetKey(m_WindowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(m_WindowHandle, GLFW_TRUE);
            }

            float currentFrame = static_cast<float>(glfwGetTime());
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Clear color to a dark blue/grey tint instead of pitch black
            // This lets us visually verify if the window buffer clear is firing!
            glClearColor(0.12f, 0.14f, 0.18f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // Re-bind the context array targets safely per tick
            glBindVertexArray(globalVAO);

            if (m_UpdateCallback) {
                m_UpdateCallback(deltaTime);
            }

            int physicalWidth, physicalHeight;
            glfwGetFramebufferSize(m_WindowHandle, &physicalWidth, &physicalHeight);

            for (auto& viewport : m_Viewports) {
                if (viewport) {
                    viewport->Render(physicalWidth, physicalHeight);
                }
            }

            glfwSwapBuffers(m_WindowHandle);
            glfwPollEvents();
        }

        // Clean out hardware arrays on shutdown
        glDeleteVertexArrays(1, &globalVAO);
        return 0;
    }
} // namespace Gui