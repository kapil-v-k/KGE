#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <functional>

namespace Gui {

    class ButtonComponent : public Component {
    private:
        glm::vec2 m_Size;
        bool m_IsHovered = false;
        bool m_IsPressed = false;

        float m_FlashTimer = 0.0f;
        bool m_IsFlashing = false;

        // Custom action callback hooks executed instantly on interaction states
        std::function<void()> m_OnClickCallback = nullptr;
        std::function<void(bool)> m_OnHoverCallback = nullptr;

    public:
        // Core framework component linking parameters
        ButtonComponent(UiObject* owner, const glm::vec2& size) 
            : Component(owner), m_Size(size) {}
        ~ButtonComponent() override = default;

        [[nodiscard]] static std::shared_ptr<ButtonComponent> Create(UiObject* owner, const glm::vec2& size) {
            return std::make_shared<ButtonComponent>(owner, size);
        }

        // --- PUBLIC REGISTRATION HOOK INTERFACES ---------------------------
        void OnClick(std::function<void()> callback) { m_OnClickCallback = callback; }
        void OnHover(std::function<void(bool)> callback) { m_OnHoverCallback = callback; }

        // --- GETTERS & INTERACTION TRACKERS ---------------------------------
        [[nodiscard]] bool IsHovered() const { return m_IsHovered; }
        [[nodiscard]] bool IsPressed() const { return m_IsPressed; }
        [[nodiscard]] const glm::vec2& GetSize() const { return m_Size; }
        [[nodiscard]] bool IsFlashing() const { return m_IsFlashing; }
        // Checking routine calculates 2D bounding boxes under your center-origin matrix layouts
        bool CheckIntersection(float mouseX, float mouseY, const glm::mat4& worldTransform);

        // Core processing steps called inside your application update pass
        void HandleInput(float mouseX, float mouseY, bool mousePressedNow, const glm::mat4& worldTransform);    
        void Update(float deltaTime);
    };

} // namespace Gui