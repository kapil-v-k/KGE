#pragma once
#include "Component.h"
#include "Font.h"
#include "BatchRenderer.hpp"
#include <string>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace Gui {

    class TextComponent : public Component {
    private:
        std::string m_Text;
        glm::vec4   m_TextColor{ 1.0f, 1.0f, 1.0f, 1.0f };
        Font*       m_FontAtlas{ nullptr };
        
        // --- NEW: STRATEGY 2 LOCAL TRANSFORMS ---
        glm::vec2   m_LocalPosition{ 0.0f, 0.0f }; // Component-level offset
        float       m_LocalScale{ 1.0f };          // Component-level font scale 
        float       m_LocalRotationRadians{ 0.0f };// Component-level tilt rotation
        std::function<std::string(float)> m_UpdateScript{ nullptr };
    public:
        explicit TextComponent(UiObject* owner = nullptr) 
        : Component(owner), m_Text(""), m_TextColor(1.0f, 1.0f, 1.0f, 1.0f), m_LocalScale(1.0f), m_LocalPosition(0.0f, 0.0f) {}

        ~TextComponent() override = default;

        [[nodiscard]] static std::unique_ptr<TextComponent> Create(const std::string& initialText = "", float initialScale = 1.0f) {
            auto comp = std::make_unique<TextComponent>(nullptr);
            comp->SetText(initialText);
            comp->SetLocalScale(initialScale);
            return comp;
        }
        void BindUpdateScript(std::function<std::string(float)> script) {
            m_UpdateScript = std::move(script);
        }

        void SetText(const std::string& text) { m_Text = text; }
        void SetColor(const glm::vec4& color) { m_TextColor = color; }
        void SetFont(Font* font) { m_FontAtlas = font; }

        // --- NEW: STRATEGY 2 API ENDPOINTS ---
        void SetLocalPosition(float x, float y) { m_LocalPosition = { x, y }; }
        void SetLocalScale(float scaleFactor) { m_LocalScale = scaleFactor; }
        
        void SetLocalRotation(float degrees) { 
            m_LocalRotationRadians = degrees * (glm::pi<float>() / 180.0f); 
        }
        void RotateLocal(float degreeDelta) {
            m_LocalRotationRadians += degreeDelta * (glm::pi<float>() / 180.0f);
        }

        [[nodiscard]] RenderLayer GetRenderLayer() const override { 
            return RenderLayer::Foreground; 
        }

        void Update(float deltaTime) override {
            // If a developer attached a dynamic data callback loop script...
            if (m_UpdateScript) {
                // ...execute it, fetch the updated string, and rewrite our characters map!
                std::string updatedText = m_UpdateScript(deltaTime);
                SetText(updatedText);
            }
        }

        void Render(class BatchRenderer& batcher, const glm::mat4& combinedTransform, Viewport* activeViewport) override;
    };

} // namespace Gui