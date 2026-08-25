#include "RenderComponent.hpp"
#include "BatchRenderer.hpp"
#include "Primitive.h"
#include "Viewport.hpp"

namespace Gui
{
    void RenderComponent::Render(BatchRenderer& batcher, const glm::mat4& combinedTransform, Gui::Viewport* activeViewport)
    {
        // Fetch the active, sandboxed viewport shader registers natively
        if (activeViewport && activeViewport->GetActiveShader()) {
            auto* activeShader = activeViewport->GetActiveShader();
            
            // ====================================================================
            // --- FIXED: FORCE STATE STRIPPING BEFORE DRAWING COMPONENT PANELS ---
            // ====================================================================
            // Forcing an explicit flush and resetting u_PrimitiveType to 0 breaks any 
            // leftover text atlas data loops, restoring your grey layout panels instantly!
            batcher.Flush();
            activeShader->SetUniformInt("u_PrimitiveType", 0);
        }

        for (auto& primitive : m_Primitives) {
            if (primitive) {
                batcher.Submit(*primitive, combinedTransform);
            }
        }
    }
} // namespace Gui