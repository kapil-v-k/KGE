#include "ButtonComponent.h"
#include "UiObject.hpp"
#include "Viewport.hpp"
#include "BatchRenderer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> // Required for std::max natively

namespace Gui {

    bool ButtonComponent::CheckIntersection(float mouseX, float mouseY, const glm::mat4& worldTransform, const glm::mat4& projectionMatrix) {
        GLFWwindow* nativeWin = glfwGetCurrentContext();
        if (!nativeWin) return false;

        int windowW = 1, windowH = 1;
        glfwGetWindowSize(nativeWin, &windowW, &windowH);

        // 1. Isolate the logical size of the Left Viewport cell panel
        float vpLogicalWidth  = static_cast<float>(windowW) * 0.5f;
        float vpLogicalHeight = static_cast<float>(windowH);

        // 2. Compute native OpenGL NDC coordinates relative to window client size
        float ratioX = mouseX / vpLogicalWidth;
        float ratioY = mouseY / vpLogicalHeight;

        float ndcX = (2.0f * ratioX) - 1.0f;
        float ndcY = 1.0f - (2.0f * ratioY); 

        // 3. Transform the synchronized NDC back into pure local space using your full matrix stack
        glm::mat4 fullSceneTransform = projectionMatrix * worldTransform;
        glm::mat4 completeInversePipeline = glm::inverse(fullSceneTransform);
        glm::vec4 localMouse = completeInversePipeline * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);

        if (localMouse.w != 0.0f) {
            localMouse.x /= localMouse.w;
            localMouse.y /= localMouse.w;
        }

        // ====================================================================
        // --- FIXED: TRUE SIGNED DISTANCE FIELD INTERSECTION KERNEL ----------
        // ====================================================================
        // Extracted parameters match your 100x45 footprint precisely
        float halfW = m_Size.x * 0.5f; // Exactly 50.0f
        float halfH = m_Size.y * 0.5f; // Exactly 22.5f
        
        // Define your active corner radius target factor to match your shader configuration
        // (Changing this parameter scales the physical interaction curvature natively!)
        float cornerRadius = 15.0f; 

        // Adjust dimensions to account for the corner arc center pivot offsets
        float componentExtentsX = halfW - cornerRadius;
        float componentExtentsY = halfH - cornerRadius;

        // Compute the absolute vector displacement inside the local quadrant space
        float dx = std::max(std::abs(localMouse.x) - componentExtentsX, 0.0f);
        float dy = std::max(std::abs(localMouse.y) - componentExtentsY, 0.0f);

        // Calculate absolute Euclidean distance relative to the corner arc radius line
        float absoluteSdfDistance = std::sqrt(dx * dx + dy * dy) - cornerRadius;

        // Visual stroke thickness padding compensator
        float visualStrokeCompensator = 1.5f;

        // Distance <= 0 means your cursor sits perfectly inside the curved boundary profiles!
        return (absoluteSdfDistance <= visualStrokeCompensator);
    }

    void ButtonComponent::HandleInput(float mouseX, float mouseY, bool mousePressedNow, const glm::mat4& worldTransform, const glm::mat4& projectionMatrix) {
        if (!m_Owner) return;

        m_IsHovered = CheckIntersection(mouseX, mouseY, worldTransform, projectionMatrix);

        if (m_OnHoverCallback) {
            m_OnHoverCallback(m_IsHovered);
        }

        if (m_IsHovered) {
            if (mousePressedNow && !m_IsPressed) {
                m_IsPressed = true;
            } 
            else if (!mousePressedNow && m_IsPressed) {
                m_IsPressed = false;
                
                m_FlashTimer = 1.0f; 
                m_IsFlashing = true;

                if (m_OnClickCallback) {
                    m_OnClickCallback(); 
                }
            }
        } else {
            if (!mousePressedNow) {
                m_IsPressed = false;
            }
        }
    }

    void ButtonComponent::Update(float deltaTime) {
        if (m_IsFlashing) {
            m_FlashTimer -= deltaTime;
            if (m_FlashTimer <= 0.0f) {
                m_FlashTimer = 0.0f;
                m_IsFlashing = false;
            }
        }
    }

    void ButtonComponent::Render(Gui::BatchRenderer& /*renderer*/, const glm::mat4& /*worldTransform*/, Gui::Viewport* /*viewport*/) {
        // Dispatched via standalone primitives
    }

} // namespace Gui