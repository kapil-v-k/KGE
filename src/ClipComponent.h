#pragma once
#include "Component.h"
#include "Primitive.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace Gui {

    // 1. --- ABSTRACT BASE MASKING INTERFACE ---
    class ClipComponent : public Component {
    protected:
        std::vector<std::shared_ptr<Primitive>> m_MaskPrimitives;
        glm::vec2 m_ClipSize;

    public:
        ClipComponent(UiObject* owner, const glm::vec2& size) 
            : Component(owner), m_ClipSize(size) {}
        ~ClipComponent() override = default;

        template<typename... Args>
        void AddPrimitives(std::shared_ptr<Args>... args) {
            (m_MaskPrimitives.push_back(std::static_pointer_cast<Primitive>(args)), ...);
        }

        const glm::vec2& GetClipSize() const { return m_ClipSize; }

        // --- FIXED: ADDED THE 4th ARGUMENT TO THE BASE METHOD AS WELL ---
        virtual void BeginClip(class BatchRenderer& batcher, const glm::mat4& transform, class Viewport* viewport, class UiObject* owner) = 0;
        virtual void EndClip(class BatchRenderer& batcher) = 0;
    };

    // 2. --- HIGH-PERFORMANCE UP-RIGHT SCISSOR CLIPPING ---
    class ScissorClipComponent : public ClipComponent {
    public:
        ScissorClipComponent(UiObject* owner, const glm::vec2& size) : ClipComponent(owner, size) {}
        
        [[nodiscard]] static std::shared_ptr<ScissorClipComponent> Create(UiObject* owner, const glm::vec2& size) {
            return std::make_shared<ScissorClipComponent>(owner, size);
        }

        void BeginClip(class BatchRenderer& batcher, const glm::mat4& transform, class Viewport* viewport, class UiObject* owner) override;
        void EndClip(class BatchRenderer& batcher) override;
    };

    // 3. --- SEAMLESS ROTATED STENCIL BUFFER CLIPPING ---
    class StencilClipComponent : public ClipComponent {
    public:
        StencilClipComponent(UiObject* owner, const glm::vec2& size) : ClipComponent(owner, size) {}
        
        [[nodiscard]] static std::shared_ptr<StencilClipComponent> Create(UiObject* owner, const glm::vec2& size) {
            return std::make_shared<StencilClipComponent>(owner, size);
        }

        void BeginClip(class BatchRenderer& batcher, const glm::mat4& transform, class Viewport* viewport, class UiObject* owner) override;
        void EndClip(class BatchRenderer& batcher) override;
    };

    // 4. --- MODERN SIGNED DISTANCE FIELD SHADER CLIPPING ---
    class SdfClipComponent : public ClipComponent {
    public:
        SdfClipComponent(UiObject* owner, const glm::vec2& size) : ClipComponent(owner, size) {}
        
        [[nodiscard]] static std::shared_ptr<SdfClipComponent> Create(UiObject* owner, const glm::vec2& size) {
            return std::make_shared<SdfClipComponent>(owner, size);
        }

        void BeginClip(class BatchRenderer& batcher, const glm::mat4& transform, class Viewport* viewport, class UiObject* owner) override;
        void EndClip(class BatchRenderer& batcher) override;
    };

} // namespace Gui