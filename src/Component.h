#pragma once
#include "Viewport.hpp"

namespace Gui {

    class UiObject; // Forward declaration
    class Viewport;
    
    enum class RenderLayer {
        Background, // Solid panels, shapes, scissor boxes
        Foreground  // Annotation text, overlays, cursor sights
    };

    class Component {
    protected:
        UiObject* m_Owner{ nullptr };

    public:
        // Core constructor accepts an early nullptr fallback for detached blueprint creation
        explicit Component(UiObject* owner = nullptr) : m_Owner(owner) {}
        virtual ~Component() = default;

        // Prevent structural copying to eliminate slicing and memory corruptions
        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;

        // Back-link injector called during the AddComponents() ownership handoff pass
        void SetOwner(UiObject* owner) { m_Owner = owner; }
        
        [[nodiscard]] virtual RenderLayer GetRenderLayer() const { 
            return RenderLayer::Background; 
        }
        
        virtual void Update(float deltaTime) {}
        virtual void Render(class BatchRenderer& batcher, const glm::mat4& combinedTransform, Viewport* activeViewport) {}
    };

} // namespace Gui