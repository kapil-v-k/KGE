#include "ButtonComponent.h"
#include "UiObject.hpp"

namespace Gui {

    bool ButtonComponent::CheckIntersection(float mouseX, float mouseY, const glm::mat4& worldTransform) {
        // ====================================================================
        // --- FIXED: EXTRACT POSITION CORRECTLY FROM THE 4th COLUMN ---------
        // ====================================================================
        // Because your projection matrix seats (0,0) center-origin, the world 
        // position translation offsets must be parsed from column index 3!
        float worldX = worldTransform[3][0];
        float worldY = worldTransform[3][1];

        // Bounding calculation parameters
        float halfW = m_Size.x * 0.5f;
        float halfH = m_Size.y * 0.5f;

        // Find edge boundaries
        float minX = worldX - halfW;
        float maxX = worldX + halfW;
        float minY = worldY - halfH;
        float maxY = worldY + halfH;

        // Standard AABB (Axis-Aligned Bounding Box) 2D intersection evaluation
        return (mouseX >= minX && mouseX <= maxX && mouseY >= minY && mouseY <= maxY);
    }

    void ButtonComponent::HandleInput(float mouseX, float mouseY, bool mousePressedNow, const glm::mat4& worldTransform) {
        if (!m_Owner) return;

        // 1. Evaluate Bounding Box Hover Intersection States
        bool currentlyInside = CheckIntersection(mouseX, mouseY, worldTransform);

        if (currentlyInside != m_IsHovered) {
            m_IsHovered = currentlyInside;
            if (m_OnHoverCallback) {
                m_OnHoverCallback(m_IsHovered); // Trigger hover event callback hooks
            }
        }

        // 2. Evaluate Mouse Click Action Triggers
        if (m_IsHovered) {
            if (mousePressedNow && !m_IsPressed) {
                m_IsPressed = true;

                m_FlashTimer = 1.0f; // Blinks in reverse video for 250 milliseconds
                m_IsFlashing = true;

                if (m_OnClickCallback) {
                    m_OnClickCallback(); 
                }
            } 
            else if (!mousePressedNow && m_IsPressed) {
                m_IsPressed = false;
                if (m_OnClickCallback) {
                    m_OnClickCallback(); // Trigger successful click callback instantly!
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
                m_IsFlashing = false; // Gracefully switches back to baseline themes
            }
        }
    }
} // namespace Gui