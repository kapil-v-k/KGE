#pragma once
#include "opengl/VertexArray.hpp"
#include "opengl/VertexBuffer.hpp"
#include "opengl/IndexBuffer.hpp"
#include "Primitive.h"
#include "Shader.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Gui {

    class BatchRenderer {
    private:
        static constexpr size_t MAX_VERTICES = 40000;
        static constexpr size_t MAX_INDICES  = 60000;

        std::unique_ptr<VertexArray>  m_Vao;
        std::unique_ptr<VertexBuffer> m_Vbo;
        std::unique_ptr<IndexBuffer>  m_Ibo;

        std::vector<Vertex>   m_VertexBatch;
        std::vector<uint32_t> m_IndexBatch;

        GLenum m_ActiveTopology{ GL_TRIANGLES }; 
        const Gui::Shader* m_ActiveShader{ nullptr };
        glm::mat4 m_CachedProjection{ 1.0f };
        
        // --- ADDED MODEL TRACKING REGISTER ---
        glm::mat4 m_ActiveModel{ 1.0f };
        PrimitiveType m_ActivePrimitiveType{ PrimitiveType::Point };
        bool m_IsFirstPrimitiveInBatch{ true };
        const Primitive* m_ActivePrimitiveAddress{ nullptr };
    public:
        BatchRenderer() {
            m_Vbo = std::make_unique<VertexBuffer>(nullptr, MAX_VERTICES * sizeof(Vertex), true);
            m_Ibo = std::make_unique<IndexBuffer>(nullptr, MAX_INDICES * sizeof(uint32_t), true);
            
            m_Vao = std::make_unique<VertexArray>();
            m_Vao->SetupLayout(*m_Vbo, sizeof(Vertex));
        }

        ~BatchRenderer() = default;

        void BeginBatch(Topology topology, const Gui::Shader& shader, const glm::mat4& projectionMatrix) {
            m_VertexBatch.clear();
            m_IndexBatch.clear();
            m_ActiveTopology = static_cast<GLenum>(topology);
            m_ActiveShader = &shader;
            m_CachedProjection = projectionMatrix;
            
            m_IsFirstPrimitiveInBatch = true; 
            m_ActivePrimitiveType = PrimitiveType::Point;
            m_ActivePrimitiveAddress = nullptr; // Clear the instance pointer tracking link

            if (m_ActiveShader) {
                m_ActiveShader->Bind();
                m_ActiveShader->SetUniformInt("u_PrimitiveType", 0);
                m_ActiveShader->SetUniformFloat("u_BorderThickness", 0.0f);
                m_ActiveShader->SetUniformInt("u_IsFilled", 1);
                m_ActiveShader->SetUniformVec4("u_OutlineColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }

        void EndBatch() {
            // --- FIXED: Guarantee that any trailing shapes left sitting in your buffers 
            // at the end of a viewport loop pass are cleanly drawn onto the screen buffer!
            if (!m_IndexBatch.empty()) {
                Flush();
            }
        }

        // --- UPDATED SUBMIT METHOD TO ACCEPT UNIQUE MODEL MATRICES ---
        void Submit(const Primitive& primitive, const glm::mat4& modelMatrix) {
            GLenum requiredTopology = GL_TRIANGLES;
            if (primitive.GetType() == PrimitiveType::Line) {
                requiredTopology = GL_LINES;
            } 
            else if (primitive.GetType() == PrimitiveType::Point) {
                requiredTopology = GL_POINTS;
            }

            // --- FIXED: Standardize our multi-variate check using our safe distinct types ---
            if (!m_IsFirstPrimitiveInBatch) {
                // The batcher will now automatically recognize PrimitiveType::Text as a separate type,
                // triggering a clean hardware Flush when transitioning between shapes and font layers!
                if (modelMatrix != m_ActiveModel || requiredTopology != m_ActiveTopology || 
                    primitive.GetType() != m_ActivePrimitiveType || &primitive != m_ActivePrimitiveAddress) {
                    Flush();
                }
            } else {
                m_IsFirstPrimitiveInBatch = false;
            }
            
            m_ActiveModel = modelMatrix;
            m_ActiveTopology = requiredTopology;
            m_ActivePrimitiveType = primitive.GetType();
            m_ActivePrimitiveAddress = &primitive;

            if (m_ActiveShader) {
                primitive.BindUniforms(*m_ActiveShader);
            }

            PrimitiveMesh mesh = primitive.GenerateGeometry();

            if (m_VertexBatch.size() + mesh.vertices.size() >= MAX_VERTICES ||
                m_IndexBatch.size() + mesh.indices.size() >= MAX_INDICES) {
                Flush();
            }

            uint32_t indexOffset = static_cast<uint32_t>(m_VertexBatch.size());
            m_VertexBatch.insert(m_VertexBatch.end(), mesh.vertices.begin(), mesh.vertices.end());

            for (uint32_t index : mesh.indices) {
                m_IndexBatch.push_back(index + indexOffset);
            }
        }

        void Flush() {
            if (m_IndexBatch.empty() || !m_ActiveShader) return;

            m_Vbo->UpdateData(m_VertexBatch.data(), m_VertexBatch.size() * sizeof(Vertex));
            m_Ibo->UpdateData(m_IndexBatch.data(), m_IndexBatch.size() * sizeof(uint32_t));

            m_Vao->Bind();
            m_Ibo->Bind();

            // --- RESTORED: REMOVED THE STATIC CAST COMPONENTIAL TYPE UNIFORM ---
            // This stops your shapes from scrambling into random character atlas sheets!
            m_ActiveShader->SetUniformMat4("u_Projection", m_CachedProjection);
            m_ActiveShader->SetUniformMat4("u_Model", m_ActiveModel); 

            glDrawElements(m_ActiveTopology, static_cast<int>(m_IndexBatch.size()), GL_UNSIGNED_INT, nullptr);

            m_Vao->Unbind();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
            m_VertexBatch.clear();
            m_IndexBatch.clear();
        }

    };

} // namespace Gui