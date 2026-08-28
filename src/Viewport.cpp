#include "Viewport.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "UiObject.hpp"
#include "Shader.hpp"
#include "BatchRenderer.hpp"

namespace Gui
{
    // 1. Framework Constructor Definition
    Viewport::Viewport(int x, int y, int width, int height)
        : m_x(x), m_y(y), m_width(width), m_height(height), m_UseCustomOrtho(false)
    {
        // No more hardcoded 270.0f! Viewport maps 1:1 to its designated pixel bounds natively
        float halfW = static_cast<float>(width)  * 0.5f;
        float halfH = static_cast<float>(height) * 0.5f;
        
        m_ProjectionMatrix = glm::ortho(-halfW, halfW, halfH, -halfH, -1.0f, 1.0f);

        m_LocalShader = std::make_unique<Gui::Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
        m_LocalBatcher = std::make_unique<Gui::BatchRenderer>();
    }

    // 2. Destructor Definition
    Viewport::~Viewport() = default;

    // 3. Scene Tree Registration Link
    void Viewport::AddObject(std::shared_ptr<Gui::UiObject> obj)
    {
        if (obj) {
            m_Objects.push_back(obj);
        }
    }

    // 4. Hardware Render Pass Execution Loop
    void Viewport::Render(int physicalWidth, int physicalHeight)
    {
        glViewport(m_x, m_y, m_width, m_height);

        // Read the center-origin matrix transforms cleanly
        glm::mat4 projection = m_UseCustomOrtho ? m_CustomOrthoMatrix : m_ProjectionMatrix;

        if (m_LocalShader && m_LocalBatcher) {
            // Open your isolated batch channel queue before drawing natively
            m_LocalBatcher->BeginBatch(Topology::Triangles, *m_LocalShader, projection);

            // Cascade drawing commands recursively down your tree hierarchy structures
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