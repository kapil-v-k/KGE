#include "ClipContainerComponent.h"
#include <glad/glad.h>
#include "Viewport.hpp" 
#include "BatchRenderer.hpp"
#include "Primitive.h"

namespace Gui
{
    ClipContainerComponent::ClipContainerComponent(int width, int height)
        : m_width(width), m_height(height)
    {
    }

    std::shared_ptr<ClipContainerComponent> ClipContainerComponent::Create(int width, int height)
    {
        return std::make_shared<ClipContainerComponent>(width, height);
    }

    // --- FIXED: TRANSLATES AND MOVES UNIQUE_PTR DATA NATIVELY ---
    void ClipContainerComponent::AddPrimitives(std::unique_ptr<Gui::Primitive> primitive)
    {
        if (primitive) {
            m_Primitives.push_back(std::move(primitive));
        }
    }

    void ClipContainerComponent::Render(BatchRenderer& batcher, const glm::mat4& combinedTransformMatrix, Gui::Viewport* activeViewport)
    {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 
        glStencilMask(0xFF); 
        glClear(GL_STENCIL_BUFFER_BIT); 

        for (auto& primitive : m_Primitives) {
            if (primitive) {
                batcher.Submit(*primitive, combinedTransformMatrix);
            }
        }
        batcher.Flush(); 

        glStencilFunc(GL_EQUAL, 1, 0xFF); 
        glStencilMask(0x00); 
    }
} // namespace Gui