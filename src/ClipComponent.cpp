#include "ClipComponent.h"
#include "BatchRenderer.hpp"
#include "Viewport.hpp"
#include "Shader.hpp"
#include "UiObject.hpp"
#include "RenderComponent.hpp"
#include <glad/glad.h>

namespace Gui {

    // ====================================================================
    // --- STRATEGY 1: NATIVE HARDWARE SCISSOR TEST PIPELINE --------------
    // ====================================================================
    void ScissorClipComponent::BeginClip(BatchRenderer& batcher, const glm::mat4& transform, Viewport* viewport, UiObject* owner) {
        batcher.Flush();

        if (!viewport) return;

        float worldX = transform[3][0];
        float worldY = transform[3][1];

        float clipW = m_ClipSize.x;
        float clipH = m_ClipSize.y;

        float globalCenterX = static_cast<float>(viewport->GetX()) + (static_cast<float>(viewport->GetWidth()) * 0.5f);
        float globalCenterY = static_cast<float>(viewport->GetY()) + (static_cast<float>(viewport->GetHeight()) * 0.5f);

        int scissorX = static_cast<int>((globalCenterX + worldX) - (clipW * 0.5f));
        int scissorY = static_cast<int>((globalCenterY + worldY) - (clipH * 0.5f));

        glEnable(GL_SCISSOR_TEST);
        glScissor(scissorX, scissorY, static_cast<int>(clipW), static_cast<int>(clipH));
    }

    void ScissorClipComponent::EndClip(BatchRenderer& batcher) {
        batcher.Flush();
        glDisable(GL_SCISSOR_TEST);
    }

    // ====================================================================
    // --- STRATEGY 2: NATIVE HARDWARE STENCIL BUFFER PIPELINE ------------
    // ====================================================================
    void StencilClipComponent::BeginClip(BatchRenderer& batcher, const glm::mat4& transform, Viewport* viewport, UiObject* owner) {
        batcher.Flush();
        
        if (!viewport) return;

        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        
        glEnable(GL_SCISSOR_TEST);
        glScissor(viewport->GetX(), viewport->GetY(), viewport->GetWidth(), viewport->GetHeight());
        glClear(GL_STENCIL_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST); 

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        if (!m_MaskPrimitives.empty()) {
            auto stencilDrawPipeline = RenderComponent::Create(nullptr);
            if (stencilDrawPipeline) {
                for (auto& primitive : m_MaskPrimitives) {
                    if (primitive) stencilDrawPipeline->AddPrimitives(primitive);
                }
                stencilDrawPipeline->Render(batcher, transform, viewport);
            }
        }
        batcher.Flush();

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00); 
    }

    void StencilClipComponent::EndClip(BatchRenderer& batcher) {
        batcher.Flush();
        glDisable(GL_STENCIL_TEST);
    }

    // ====================================================================
    // --- STRATEGY 3: MODERN SIGNED DISTANCE FIELD SHADER CLIPPING -------
    // ====================================================================
    void SdfClipComponent::BeginClip(BatchRenderer& batcher, const glm::mat4& transform, Viewport* viewport, UiObject* owner) {
        batcher.Flush();

        if (!viewport) return;
        auto* activeShader = viewport->GetActiveShader();
        if (!activeShader) return;

        // ====================================================================
        // --- FIXED: COMPUTE FULL INVERSE TRANSFORMATION MATRIX FOR SHADERS --
        // ====================================================================
        // Calculating the mathematical inverse of the 4x4 combined matrix column
        // maps any absolute screen coordinates right back into this mask's local space!
        glm::mat4 inverseTransform = glm::inverse(transform);

        float boxWidth  = m_ClipSize.x;
        float boxHeight = m_ClipSize.y;
        float radiusCorner = 25.0f; 

        activeShader->SetUniformInt("u_UseSdfClip", 1);
        
        // Upload the complete inverse tracking matrix to your uniform registers
        activeShader->SetUniformMat4("u_InverseClipMatrix", inverseTransform);
        
        activeShader->SetUniformVec2("u_ClipHalfSize", glm::vec2(boxWidth * 0.5f, boxHeight * 0.5f));
        activeShader->SetUniformFloat("u_ClipCornerRadius", radiusCorner);
    }

    void SdfClipComponent::EndClip(BatchRenderer& batcher) {
        batcher.Flush();
    }

} // namespace Gui