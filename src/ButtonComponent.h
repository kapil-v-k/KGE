#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <functional>
#include <memory>

namespace Gui {

    class ButtonComponent : public Component {
    private:
        glm::vec2 m_Size;
        bool m_IsHovered = false;
        bool m_IsPressed = false;
        
        float m_FlashTimer = 0.0f;
        bool m_IsFlashing = false;

        std::function<void()> m_OnClickCallback = nullptr;
        std::function<void(bool)> m_OnHoverCallback = nullptr;

    public:
        ButtonComponent(UiObject* owner, const glm::vec2& size) 
            : Component(owner), m_Size(size) {}
        
        ~ButtonComponent() override = default;

        [[nodiscard]] static std::shared_ptr<ButtonComponent> Create(UiObject* owner, const glm::vec2& size) {
            return std::make_shared<ButtonComponent>(owner, size);
        }

        void OnClick(std::function<void()> callback) { m_OnClickCallback = callback; }
        void OnHover(std::function<void(bool)> callback) { m_OnHoverCallback = callback; }

        [[nodiscard]] bool IsHovered() const { return m_IsHovered; }
        [[nodiscard]] bool IsPressed() const { return m_IsPressed; }
        [[nodiscard]] bool IsFlashing() const { return m_IsFlashing; }
        [[nodiscard]] const glm::vec2& GetSize() const { return m_Size; }

        bool CheckIntersection(float mouseX, float mouseY, const glm::mat4& worldTransform, const glm::mat4& projectionMatrix);
        void HandleInput(float mouseX, float mouseY, bool mousePressedNow, const glm::mat4& worldTransform, const glm::mat4& projectionMatrix);

        void Update(float deltaTime); 
        
        // --- FIXED: ENABLE COMPONENT RENDERING PASS FOR RECTANGLE DRAWING ---
        void Render(class BatchRenderer& renderer, const glm::mat4& worldTransform, class Viewport* viewport) override;
    };

} // namespace Gui