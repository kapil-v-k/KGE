#pragma once
#include "Viewport.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations to keep headers lightweight and improve compile times
struct GLFWwindow; 

namespace Gui {

    struct ApplicationConfig {
        std::string title{ "Graphics Engine Window" };
        int width{ 1280 };
        int height{ 720 };
        bool vsync{ true };
    };

    class Application {
    private:
        ApplicationConfig m_Config;
        GLFWwindow* m_WindowHandle{ nullptr };
        bool m_IsRunning{ false };

        // An application contains and manages multiple Viewport (screen) zones
        std::vector<std::shared_ptr<Viewport>> m_Viewports;

        // User-friendly callback hook for handling custom resize logic
        std::function<void(int, int)> m_ResizeCallback;
        std::function<void(float)> m_UpdateCallback;
        // Internal platform abstraction layer (PAL) setups
        bool InitWindowContext();
        void ShutdownWindowContext();

    public:
        // Explicitly delete copying to uphold strict RAII resource ownership
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) noexcept = default;
        Application& operator=(Application&&) noexcept = default;

        // Private constructor forced through an explicit smart factory method
        explicit Application(ApplicationConfig config);
        ~Application();

        // Safe static factory generation
        [[nodiscard]] static std::unique_ptr<Application> Create(const ApplicationConfig& config = ApplicationConfig{}) {
            return std::make_unique<Application>(config);
        }

        // --- User-Friendly High-Level API Methods ---

        // Factory function for generating and nesting viewports directly inside this application window
        std::shared_ptr<Viewport> CreateViewport(int x, int y, int width, int height) {
            auto viewport = std::make_shared<Viewport>(x, y, width, height);
            m_Viewports.push_back(viewport);
            return viewport;
        }

        // Clean event listener hook for the client application
        void OnWindowResize(std::function<void(int, int)> callback) {
            m_ResizeCallback = std::move(callback);
        }

        void OnUpdate(std::function<void(float)> callback) { 
            m_UpdateCallback = std::move(callback); 
        }

        // Blocks the main execution thread and fires the engine lifecycle loops
        int Run();

        // Explicitly close or kill the execution lifecycle
        void Stop() { m_IsRunning = false; }

        // Getters for current size configuration properties
        [[nodiscard]] int GetWidth() const { return m_Config.width; }
        [[nodiscard]] int GetHeight() const { return m_Config.height; }
        [[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_WindowHandle; }

        // Internal engine bridge: Dispatches the physical runtime callbacks to client space
        void DispatchResize(int newWidth, int newHeight) {
            m_Config.width = newWidth;
            m_Config.height = newHeight;
            if (m_ResizeCallback) {
                m_ResizeCallback(newWidth, newHeight);
            }
        }
    };

} // namespace Gui