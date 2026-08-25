#pragma once
#include "Component.h"
#include "Texture.h"
#include "BatchRenderer.hpp"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

namespace Gui {

    // Clean enum wrapper to define human-readable masking rules
    enum class TextureMask {
        Rectangle = 0,
        Circle    = 1,
        Rounded   = 2
    };

    class TextureComponent : public Component {
    private:
        const Texture* m_TextureResource{ nullptr };
        glm::vec2      m_Size{ 100.0f, 100.0f };
        glm::vec4      m_Tint{ 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec2      m_LocalPosition{ 0.0f, 0.0f }; 
        
        // --- FIXED: Track mask configurations natively via our new scoped enum ---
        TextureMask    m_MaskType{ TextureMask::Rectangle }; 

    public:
        explicit TextureComponent(UiObject* owner) : Component(owner) {}
        ~TextureComponent() override = default;

        void SetTexture(const Texture* texture) { 
            m_TextureResource = texture; 
            if (texture) {
                m_Size = glm::vec2(static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()));
            }
        }
        
        void SetCustomSize(float width, float height) { m_Size = { width, height }; }
        void SetTint(const glm::vec4& tintColor) { m_Tint = tintColor; }
        void SetLocalPosition(float x, float y) { m_LocalPosition = { x, y }; }
        
        // --- FIXED: Public API setter for choosing your texture shape format ---
        void SetMaskType(TextureMask mask) { m_MaskType = mask; }

        void Render(class BatchRenderer& batcher, const glm::mat4& combinedTransform, Viewport* activeViewport) override {
            if (!m_TextureResource) return;

            glm::mat4 finalImageMatrix = glm::translate(combinedTransform, glm::vec3(m_LocalPosition, 0.0f));

            float halfW = m_Size.x * 0.5f;
            float halfH = m_Size.y * 0.5f;

            PrimitiveMesh textureMesh;

            Vertex topLeft     = { { -halfW, -halfH }, m_Tint, { 0.0f, 1.0f } }; 
            Vertex topRight    = { {  halfW, -halfH }, m_Tint, { 1.0f, 1.0f } }; 
            Vertex bottomRight = { {  halfW,  halfH }, m_Tint, { 1.0f, 0.0f } }; 
            Vertex bottomLeft  = { { -halfW,  halfH }, m_Tint, { 0.0f, 0.0f } }; 

            textureMesh.vertices = { topLeft, topRight, bottomRight, bottomLeft };
            textureMesh.indices  = { 0, 1, 2, 2, 3, 0 };

            struct ImageTexturePrimitive : public Primitive {
                PrimitiveMesh localMesh;
                unsigned int textureID;
                glm::vec2 boxSize;
                TextureMask maskMode;
                
                ImageTexturePrimitive(const PrimitiveMesh& mesh, unsigned int texID, const glm::vec2& size, TextureMask mask) 
                    : Primitive(PrimitiveType::Texture), localMesh(mesh), textureID(texID), boxSize(size), maskMode(mask) {}
                
                [[nodiscard]] PrimitiveMesh GenerateGeometry() const override { return localMesh; }
                
                void BindUniforms(const Shader& shader) const override {
                    shader.SetUniformInt("u_PrimitiveType", 4); 
                    shader.SetUniformVec2("u_RectSize", boxSize);
                    
                    // Convert the scoped enum value back to its backing integer representation (0, 1, or 2)
                    shader.SetUniformInt("u_TextureMaskType", static_cast<int>(maskMode)); 
                    
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, textureID);
                }
            };

            // Pass our internal m_MaskType setting down down into the layout wrapper
            ImageTexturePrimitive imgPrimitive(textureMesh, m_TextureResource->GetID(), m_Size, m_MaskType);
            batcher.Submit(imgPrimitive, finalImageMatrix);
        }
    };

} // namespace Gui