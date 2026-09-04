#include "UiObject.hpp"
#include "Component.h"
#include "ButtonComponent.h"
#include "BatchRenderer.hpp"
#include "Viewport.hpp"
#include "ClipContainerComponent.h"
#include "ClipComponent.h" 
#include "Shader.hpp"

namespace Gui
{
    void UiObject::Render(Gui::BatchRenderer& batcher, const glm::mat4& parentTransformMatrix, Gui::Viewport* activeViewport)
    {
        glm::mat4 localTransform = glm::mat4(1.0f);
        localTransform = glm::translate(localTransform, glm::vec3(m_Position, 0.0f));
        localTransform = glm::rotate(localTransform, m_RotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        localTransform = glm::scale(localTransform, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));

        glm::mat4 combinedTransform = parentTransformMatrix * localTransform;

        // ====================================================================
        // --- PASS 1: DRAW BACKGROUND PANELS AND TELEMETRY SHAPES FIRST ------
        // ====================================================================
        for (auto& component : m_Components) {
            if (component && component->GetRenderLayer() == RenderLayer::Background) {
                component->Render(batcher, combinedTransform, activeViewport);
            }
        }
        
        // ====================================================================
        // --- STEP 2: ACTIVATE YOUR CLIPPING COMPONENT STRATEGY --------------
        // ====================================================================
        Gui::ClipComponent* clipCheck = nullptr;
        for (auto& component : m_Components) {
            if (component) {
                auto* potentialClip = dynamic_cast<Gui::ClipComponent*>(component.get());
                if (potentialClip != nullptr) {
                    clipCheck = potentialClip;
                    break;
                }
            }
        }
        
        if (clipCheck != nullptr) {
            clipCheck->BeginClip(batcher, combinedTransform, activeViewport, this);
        }

        batcher.Flush();

        // ====================================================================
        // --- PASS 3: DRAW TYPOGRAPHY TEXT AND NUMBERS SECOND ----------------
        // ====================================================================
        for (auto& component : m_Components) {
            if (component && component->GetRenderLayer() == RenderLayer::Foreground) {
                component->Render(batcher, combinedTransform, activeViewport);
            }
        }
        
        // ====================================================================
        // --- PASS 4: RECURSIVELY TRAVERSE NON-NEEDLE CHILDREN NEXT ---------
        // ====================================================================
        for (auto& child : m_Children) {
            if (child) {
                child->Render(batcher, combinedTransform, activeViewport);
            }
        }
        
        // ====================================================================
        // --- TEARDOWN ALL CLIPPING HARDWARE STATES CLEANLY ------------------
        // ====================================================================
        if (clipCheck != nullptr) {
            clipCheck->EndClip(batcher);
            
            if (activeViewport && activeViewport->GetActiveShader()) {
                activeViewport->GetActiveShader()->SetUniformInt("u_UseSdfClip", 0);
            }
        }
    }

    void UiObject::UpdateInput(float deltaTime, float mouseX, float mouseY, bool mousePressedNow, const glm::mat4& parentTransformMatrix, const glm::mat4& projectionMatrix)
    {
        glm::mat4 localTransform = glm::mat4(1.0f);
        localTransform = glm::translate(localTransform, glm::vec3(m_Position, 0.0f));
        
        if (m_RotationRadians != 0.0f) {
            localTransform = glm::rotate(localTransform, m_RotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        
        localTransform = glm::scale(localTransform, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));

        // Column-major scene graph multiplication ensures parents affect children correctly
        glm::mat4 combinedTransform = parentTransformMatrix * localTransform;

        // Cascade input events cleanly to all attached components
        for (auto& component : m_Components) {
            if (component) {
                auto* button = dynamic_cast<Gui::ButtonComponent*>(component.get());
                if (button != nullptr) {
                    // ====================================================================
                    // --- CLEAN RECURSION: EVALUATE ONLY LOCAL ATTACHED COMPONENTS -------
                    // ====================================================================
                    // Because button components are processed cleanly within their own local node's
                    // m_Components array vector, we run updates directly without needing protected getters!
                    button->Update(deltaTime); 
                    button->HandleInput(mouseX, mouseY, mousePressedNow, combinedTransform, projectionMatrix);
                }
            }
        }

        // Recursively walk down through your child node branches
        for (auto& child : m_Children) {
            if (child) {
                child->UpdateInput(deltaTime, mouseX, mouseY, mousePressedNow, combinedTransform, projectionMatrix);
            }
        }
    }
} // namespace Gui