#include "UiObject.hpp"
#include <glad/glad.h>
#include "BatchRenderer.hpp"
#include "ClipContainerComponent.h"

namespace Gui
{
    void UiObject::Render(Gui::BatchRenderer& batcher, const glm::mat4& parentTransformMatrix, Gui::Viewport* activeViewport)
    {
        glm::mat4 localTransform = glm::mat4(1.0f);
        localTransform = glm::translate(localTransform, glm::vec3(m_Position, 0.0f));
        localTransform = glm::rotate(localTransform, m_RotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        localTransform = glm::scale(localTransform, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));

        glm::mat4 combinedTransform = parentTransformMatrix * localTransform;

        bool thisWidgetIsAMaskContainer = false;

        // ====================================================================
        // --- PASS 1: DRAW BACKGROUND PANELS AND TELEMETRY SHAPES FIRST ---
        // ====================================================================
        for (auto& component : m_Components) {
            if (dynamic_cast<Gui::ClipContainerComponent*>(component.get())) {
                thisWidgetIsAMaskContainer = true;
            }
            if (component->GetRenderLayer() == RenderLayer::Background) {
                component->Render(batcher, combinedTransform, activeViewport);
            }
        }

        // ====================================================================
        // --- PASS 2: DRAW TYPOGRAPHY TEXT AND NUMBERS SECOND ---
        // ====================================================================
        for (auto& component : m_Components) {
            if (component->GetRenderLayer() == RenderLayer::Foreground) {
                component->Render(batcher, combinedTransform, activeViewport);
            }
        }

        // ====================================================================
        // --- PASS 3: RECURSIVELY TRAVERSE NON-NEEDLE CHILDREN NEXT ---
        // ====================================================================
        for (auto& child : m_Children) {
            if (child) {
                child->Render(batcher, combinedTransform, activeViewport);
            }
        }

        if (thisWidgetIsAMaskContainer) {
            batcher.Flush(); 
            glDisable(GL_STENCIL_TEST); 
            glDisable(GL_SCISSOR_TEST);
            glStencilMask(0xFF); 
        }
    }
} // namespace Gui