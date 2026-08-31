#include "UiObject.hpp"
#include <glad/glad.h>
#include "BatchRenderer.hpp"
#include "ClipContainerComponent.h"
#include "ClipComponent.h" 
#include "Shader.hpp"
#include "Viewport.hpp"
#include "ButtonComponent.h"

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
        // Background components (like your magenta frame or clock chassis plates) 
        // draw in unmasked space, guaranteeing they never clip themselves!
        for (auto& component : m_Components) {
            if (component && component->GetRenderLayer() == RenderLayer::Background) {
                component->Render(batcher, combinedTransform, activeViewport);
            }
        }
        
        // ====================================================================
        // --- STEP 2: ACTIVATE YOUR CLIPPING COMPONENT STRATEGY --------------
        // ====================================================================
        // We find if a clip component is registered and trigger it exactly here!
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
        
        // This turns on glScissor, Stencil writing, or SDF uniforms right before foreground tasks!
        if (clipCheck != nullptr) {
            clipCheck->BeginClip(batcher, combinedTransform, activeViewport, this);
        }

        batcher.Flush();
        // ====================================================================
        // --- PASS 3: DRAW TYPOGRAPHY TEXT AND NUMBERS SECOND ----------------
        // ====================================================================
        // Foreground elements now execute securely under the active mask rules!
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

    void UiObject::UpdateInput(float deltaTime,float mouseX, float mouseY, bool mousePressedNow, const glm::mat4& parentTransformMatrix)
    {
        // Re-calculate the exact matrix mapping matching your standard rendering pass
        glm::mat4 localTransform = glm::mat4(1.0f);
        localTransform = glm::translate(localTransform, glm::vec3(m_Position, 0.0f));
        localTransform = glm::rotate(localTransform, m_RotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        localTransform = glm::scale(localTransform, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));

        glm::mat4 combinedTransform = parentTransformMatrix * localTransform;

        // Check if this object contains our interactive button script component
        // (Bypasses keyword errors by manually reading your component list array)
        for (auto& component : m_Components) {
            if (component) {
                #include "ButtonComponent.h" // Ensures type visibility
                auto* button = dynamic_cast<Gui::ButtonComponent*>(component.get());
                if (button != nullptr) {
                    // Pass mouse states and combined matrix rules directly to the button handler!
                    button->Update(deltaTime);
                    button->HandleInput(mouseX, mouseY, mousePressedNow, combinedTransform);
                }
            }
        }

        // Recursively cascade input evaluation passes straight down to all child nodes
        for (auto& child : m_Children) {
            if (child) {
                child->UpdateInput(deltaTime, mouseX, mouseY, mousePressedNow, combinedTransform);
            }
        }
    }
} // namespace Gui