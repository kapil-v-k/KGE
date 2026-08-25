#pragma once
#include <glad/glad.h> // Or your platform's OpenGL/GLES loader
#include <cstddef>

namespace Gui {

    class VertexBuffer {
    private:
        uint32_t m_RendererID{0};

    public:
        // Accept raw data or pre-allocate sizes for dynamic GUI streams
        VertexBuffer(const void* data, size_t size, bool isDynamic = false) {
            glGenBuffers(1, &m_RendererID);
            glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
            
            GLenum usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
            glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        }

        ~VertexBuffer() {
            if (m_RendererID != 0) {
                glDeleteBuffers(1, &m_RendererID);
            }
        }

        // Prevent copying to dodge accidental double-freeing of the GPU ID
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        // Allow explicit structural moving
        VertexBuffer(VertexBuffer&& other) noexcept : m_RendererID(other.m_RendererID) {
            other.m_RendererID = 0;
        }

        VertexBuffer& operator=(VertexBuffer&& other) noexcept {
            if (this != &other) {
                if (m_RendererID != 0) glDeleteBuffers(1, &m_RendererID);
                m_RendererID = other.m_RendererID;
                other.m_RendererID = 0;
            }
            return *this;
        }

        void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); }
        void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

        // Update sections of the buffer dynamically without full re-allocations
        void UpdateData(const void* data, size_t size, size_t offset = 0) const {
            glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        }
    };

} // namespace Gui