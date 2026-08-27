#pragma once
#include "Component.h"
#include "Primitive.h"
#include "BatchRenderer.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace Gui {

    class RenderComponent : public Component {
    private:
        // Unified collection tracking using shared pointers to align with the shape primitives
        std::vector<std::shared_ptr<Primitive>> m_Primitives;

    public:
        explicit RenderComponent(UiObject* owner) : Component(owner) {}
        ~RenderComponent() override = default;

        [[nodiscard]] static std::shared_ptr<RenderComponent> Create(UiObject* owner)
        {
            return std::make_shared<RenderComponent>(owner);
        }

        // ====================================================================
        // --- FIXED: VARIADIC FOLD OPERATIONS TYPE CASTING HARMONY -----------
        // ====================================================================
        // Casting elements explicitly to your master base type (Primitive) 
        // stops parameter pack translation warnings across strict compilers!
        template<typename... Args>
        void AddPrimitives(std::shared_ptr<Args>... args) {
            (m_Primitives.push_back(std::static_pointer_cast<Primitive>(args)), ...);
        }

        void Render(class BatchRenderer& batcher, const glm::mat4& combinedTransform, Viewport* activeViewport);
    
    };

} // namespace Gui