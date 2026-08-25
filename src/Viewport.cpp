#include "Viewport.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "UiObject.hpp"
#include "Shader.hpp"
#include "BatchRenderer.hpp"

namespace Gui
{
    Viewport::Viewport(int x, int y, int width, int height)
        : m_x(x), m_y(y), m_width(width), m_height(height)
    {
        m_ProjectionMatrix = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);

        // --- FIXED: INITIALIZE INDEPENDENT STABLE GPU STATE PIPELINES ---
        m_LocalShader = std::make_unique<Gui::Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
        m_LocalBatcher = std::make_unique<Gui::BatchRenderer>();
    }

    Viewport::~Viewport() = default;

    void Viewport::AddObject(std::shared_ptr<Gui::UiObject> obj)
    {
        if (obj) {
            m_Objects.push_back(obj);
        }
    }

    void Viewport::Render(int physicalWidth, int physicalHeight)
    {
        glViewport(m_x, m_y, m_width, m_height);

        glm::mat4 projection = m_UseCustomOrtho ? m_CustomOrthoMatrix : m_ProjectionMatrix;

        if (m_LocalShader && m_LocalBatcher) {
            
            // Open your batch channel queue before drawing
            m_LocalBatcher->BeginBatch(Topology::Triangles, *m_LocalShader, projection);

            // Cascade drawing commands recursively down your tree hierarchy
            for (auto& object : m_Objects) {
                if (object) {
                    object->Render(*m_LocalBatcher, glm::mat4(1.0f), this);
                }
            }
            
            // Flush and finalize vertex processing data sheets safely
            m_LocalBatcher->EndBatch();
        }
    }
} // namespace Gui