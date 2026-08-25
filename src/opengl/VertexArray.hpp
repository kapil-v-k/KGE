#pragma once
#include "VertexBuffer.hpp"
#include "../Primitive.h" // Ensure Vertex struct is fully visible
#include <cstddef>    // Required for standard offsetof macro mapping

namespace Gui {

    class VertexArray {
    private:
        uint32_t m_RendererID{0};

    public:
        VertexArray() {
            glGenVertexArrays(1, &m_RendererID);
        }

        ~VertexArray() {
            if (m_RendererID != 0) {
                glDeleteVertexArrays(1, &m_RendererID);
            }
        }

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept : m_RendererID(other.m_RendererID) {
            other.m_RendererID = 0;
        }

        VertexArray& operator=(VertexArray&& other) noexcept {
            if (this != &other) {
                if (m_RendererID != 0) glDeleteVertexArrays(1, &m_RendererID);
                m_RendererID = other.m_RendererID;
                other.m_RendererID = 0;
            }
            return *this;
        }

        void Bind() const { glBindVertexArray(m_RendererID); }
        void Unbind() const { glBindVertexArray(0); }

        void SetupLayout(const VertexBuffer& vbo, size_t vertexStride) const {
            Bind();
            vbo.Bind();

            // Attribute 0: Position (vec2 -> 2 floats)
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                0, 2, GL_FLOAT, GL_FALSE, 
                static_cast<GLsizei>(vertexStride), 
                reinterpret_cast<const void*>(offsetof(Vertex, position)) // --- FIXED OFFSET ---
            );

            // Attribute 1: Color (vec4 -> 4 floats)
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1, 4, GL_FLOAT, GL_FALSE, 
                static_cast<GLsizei>(vertexStride), 
                reinterpret_cast<const void*>(offsetof(Vertex, color))    // --- FIXED OFFSET ---
            );

            // Attribute 2: Texture Coordinate (vec2 -> 2 floats)
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(
                2, 2, GL_FLOAT, GL_FALSE, 
                static_cast<GLsizei>(vertexStride), 
                reinterpret_cast<const void*>(offsetof(Vertex, texCoord)) // --- FIXED OFFSET ---
            );
        }
    };

} // namespace Gui