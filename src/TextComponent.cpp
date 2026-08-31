#include "TextComponent.h"
#include "BatchRenderer.hpp"
#include "Shader.hpp"
#include "Viewport.hpp"
#include <glad/glad.h>

namespace Gui
{
    void TextComponent::Render(class BatchRenderer& batcher, const glm::mat4& combinedTransform, Viewport* activeViewport) {
        if (!m_FontAtlas || m_Text.empty() || !activeViewport) return;

        // Grab your active viewport shader directly to manipulate states explicitly
        auto* activeShader = activeViewport->GetActiveShader();
        if (!activeShader) return;

        glm::mat4 componentMatrix = glm::mat4(1.0f);
        componentMatrix = glm::translate(componentMatrix, glm::vec3(m_LocalPosition, 0.0f));
        componentMatrix = glm::rotate(componentMatrix, m_LocalRotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        componentMatrix = glm::scale(componentMatrix, glm::vec3(m_LocalScale, m_LocalScale, 1.0f));
        
        glm::mat4 finalTextMatrix = combinedTransform * componentMatrix;

        float cursorX = 0.0f;
        float cursorY = 0.0f;
        float startAlignmentX = 0.0f; 
        float lineSpacingHeight = 28.0f; 

        // Shared static primitive shell stays completely alive throughout the rendering loop pass
        struct TextGlyphPrimitive : public Primitive {
            PrimitiveMesh localMesh;
            TextGlyphPrimitive(const PrimitiveMesh& mesh) 
                : Primitive(PrimitiveType::Rectangle), localMesh(mesh) {}
            
            [[nodiscard]] PrimitiveMesh GenerateGeometry() const override { return localMesh; }
            void BindUniforms(const Shader& shader) const override {} // Managed explicitly below
        };

        // ====================================================================
        // --- FIXED: FORCE PIPELINE FLUSH & ACTIVATE TEXT UNIFORMS NATIVELY --
        // ====================================================================
        // By flushing the old background rectangle primitives first and forcing the 
        // shader registers to swap to Type 3 right here, we prevent stack-destruction drops!
        batcher.Flush();
        activeShader->SetUniformInt("u_PrimitiveType", 3);
        activeShader->SetUniformFloat("u_BorderThickness", 0.0f);
        activeShader->SetUniformInt("u_IsFilled", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_FontAtlas->GetAtlasTextureID());

        for (char charCode : m_Text) {
            const Character& ch = m_FontAtlas->GetCharacter(charCode);

            if (charCode == '\n') {
                cursorX = startAlignmentX;    
                cursorY += lineSpacingHeight; 
                continue; 
            }
            if (charCode == '\r') {
                continue; 
            }

            float xpos = cursorX + static_cast<float>(ch.bearing.x);
            float ypos = cursorY - static_cast<float>(ch.bearing.y);

            float width  = static_cast<float>(ch.size.x);
            float height = static_cast<float>(ch.size.y);

            if (width > 0 && height > 0) {
                PrimitiveMesh letterMesh;

                Vertex topLeft     = { { xpos,         ypos },          m_TextColor, { ch.textureCoordMin.x, ch.textureCoordMin.y } };
                Vertex topRight    = { { xpos + width, ypos },          m_TextColor, { ch.textureCoordMax.x, ch.textureCoordMin.y } };
                Vertex bottomRight = { { xpos + width, ypos + height }, m_TextColor, { ch.textureCoordMax.x, ch.textureCoordMax.y } };
                Vertex bottomLeft  = { { xpos,         ypos + height }, m_TextColor, { ch.textureCoordMin.x, ch.textureCoordMax.y } };

                letterMesh.vertices = { topLeft, topRight, bottomRight, bottomLeft };
                letterMesh.indices = { 0, 1, 2, 2, 3, 0 };

                TextGlyphPrimitive glyphPrimitive(letterMesh);
                
                // Submit geometry vertices cleanly into the active text buffer queue
                batcher.Submit(glyphPrimitive, finalTextMatrix);
            }

            cursorX += static_cast<float>(ch.advance);
        }
        
        // Force draw the accumulated text layout glyphs instantly before leaving the component pass
        batcher.Flush();
    }
} // namespace Gui