#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace Gui {
    class UiObject;
    class BatchRenderer;
    class Shader;
}

namespace Gui
{
    class Viewport {
    public:
        Viewport(int x, int y, int width, int height);
        ~Viewport(); // Destructor handles automated cleanup paths

        void AddObject(std::shared_ptr<Gui::UiObject> obj);
        const std::vector<std::shared_ptr<Gui::UiObject>>& GetObjects() const { return m_Objects; }

        // --- FIXED: THE EXACT TWO-ARGUMENT SIGNATURE YOUR APPLICATION.CPP EXPECTS ---
        void Render(int physicalWidth, int physicalHeight);

        // Public shader accessor hook for child component access
        Gui::Shader* GetActiveShader() const { return m_LocalShader.get(); }

        bool m_UseCustomOrtho = false;
        glm::mat4 m_CustomOrthoMatrix = glm::mat4(1.0f);

    private:
        int m_x, m_y;
        int m_width, m_height;
        std::vector<std::shared_ptr<Gui::UiObject>> m_Objects;
        glm::mat4 m_ProjectionMatrix;

        // --- FIXED: EACH VIEWPORT INSTANTIATED ITS OWN PRIVATE ISOLATED SHADER LAYER ---
        std::unique_ptr<Gui::BatchRenderer> m_LocalBatcher;
        std::unique_ptr<Gui::Shader> m_LocalShader;
    };
} // namespace Gui