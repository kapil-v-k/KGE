#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"

namespace Gui {
    class BatchRenderer;
    class Viewport;
}

namespace Gui
{
    class UiObject : public std::enable_shared_from_this<UiObject> {
    public:
        UiObject() = default;
        ~UiObject() = default;

        static std::shared_ptr<UiObject> Create() {
            return std::make_shared<UiObject>();
        }
        float GetRotation() const { return m_RotationRadians; }
        void Move(float x, float y) { m_Position = glm::vec2(x, y); }
        void Scale(float s) { m_Scale = glm::vec2(s, s); }
        void SetRotation(float deg) { m_RotationRadians = deg * (3.14159265f / 180.0f); }

        void AddComponents(std::shared_ptr<Component> comp) { m_Components.push_back(comp); }
        template<typename... Args>
        void AddComponents(std::shared_ptr<Component> first, Args... args) {
            m_Components.push_back(first);
            AddComponents(args...);
        }

        void AddChild(std::shared_ptr<UiObject> child) { m_Children.push_back(child); }
        void UpdateInput(float deltaTime, float mouseX, float mouseY, bool mousePressedNow, const glm::mat4& parentTransformMatrix);
        void Render(Gui::BatchRenderer& batcher, const glm::mat4& parentTransformMatrix, Gui::Viewport* activeViewport);

    private:
        glm::vec2 m_Position = glm::vec2(0.0f);
        glm::vec2 m_Scale = glm::vec2(1.0f);
        float m_RotationRadians = 0.0f;

        std::vector<std::shared_ptr<Component>> m_Components;
        std::vector<std::shared_ptr<UiObject>> m_Children;
    };
} // namespace Gui