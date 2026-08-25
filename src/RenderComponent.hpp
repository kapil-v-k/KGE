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
        template<typename T, typename... Args>
        std::shared_ptr<T> AddPrimitives(Args&&... args) {
            
            // Allocate the specific sub-shape cleanly on the heap using std::make_shared
            auto shape = std::make_shared<T>(std::forward<Args>(args)...);
            
            // Push the shared pointer straight into our array cache seamlessly
            m_Primitives.push_back(shape);
            
            return shape; 
        }

        void Render(class BatchRenderer& batcher, const glm::mat4& combinedTransform, Viewport* activeViewport);
    
    };

} // namespace Gui