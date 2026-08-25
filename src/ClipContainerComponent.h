#pragma once

#include "Component.h"
#include <memory>
#include <vector>

namespace Gui {
    class Primitive;
    class BatchRenderer;
    class Viewport;
}

namespace Gui
{
    class ClipContainerComponent : public Component {
    public:
        ClipContainerComponent(int width, int height);
        ~ClipContainerComponent() override = default;

        static std::shared_ptr<ClipContainerComponent> Create(int width, int height);

        void Render(BatchRenderer& batcher, const glm::mat4& combinedTransformMatrix, Gui::Viewport* activeViewport) override;
        void AddPrimitives(std::unique_ptr<Gui::Primitive> primitive);

    private:
        int m_width;
        int m_height;
        std::vector<std::unique_ptr<Gui::Primitive>> m_Primitives;
    };
} // namespace Gui