#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        ~Viewport(); 

        int GetX() const { return m_x; }
        int GetY() const { return m_y; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        
        void AddObject(std::shared_ptr<Gui::UiObject> obj);
        const std::vector<std::shared_ptr<Gui::UiObject>>& GetObjects() const { return m_Objects; }

        void Render(int physicalWidth, int physicalHeight);

        Gui::Shader* GetActiveShader() const { return m_LocalShader.get(); }

        // ====================================================================
        // --- PUBLIC TRANSFORMATION STATE DATA REGISTERS ---------------------
        // ====================================================================
        bool m_UseCustomOrtho = false;
        glm::mat4 m_CustomOrthoMatrix = glm::mat4(1.0f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f); // ◄── MOVED TO PUBLIC PASS

        // ====================================================================
        // --- INLINE ACCESS METHOD HOOK PROVIDES NATIVE RESOLUTION ZOOM ------
        // ====================================================================
        void UpdateLayoutBounds(int x, int y, int width, int height) {
             m_x = x;
            m_y = y;
            m_width = width;
            m_height = height;

            // ====================================================================
            // --- FUTURE-PROOF: COORDINATES SCALE PERFECTLY TO PIXELS -----------
            // ====================================================================
            // By using pure half-width and half-height pixel bounds, this viewport 
            // becomes 100% universal and can be initialized to any size later!
            float halfW = static_cast<float>(width)  * 0.5f;
            float halfH = static_cast<float>(height) * 0.5f;
            
            m_ProjectionMatrix = glm::ortho(-halfW, halfW, halfH, -halfH, -1.0f, 1.0f);
        }

    private:
        int m_x, m_y;
        int m_width, m_height;
        std::vector<std::shared_ptr<Gui::UiObject>> m_Objects;

        std::unique_ptr<Gui::BatchRenderer> m_LocalBatcher;
        std::unique_ptr<Gui::Shader> m_LocalShader;
    };
} // namespace Gui