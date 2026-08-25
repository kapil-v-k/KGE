#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace Gui {

    class IndexBuffer {
    private:
        uint32_t m_RendererID{0};
        uint32_t m_Count{0};

    public:
        IndexBuffer(const uint32_t* indices, uint32_t count, bool isDynamic = false) 
            : m_Count(count) {
            glGenBuffers(1, &m_RendererID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
            
            GLenum usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, usage);
        }

        ~IndexBuffer() {
            if (m_RendererID != 0) {
                glDeleteBuffers(1, &m_RendererID);
            }
        }

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&& other) noexcept 
            : m_RendererID(other.m_RendererID), m_Count(other.m_Count) {
            other.m_RendererID = 0;
            other.m_Count = 0;
        }

        IndexBuffer& operator=(IndexBuffer&& other) noexcept {
            if (this != &other) {
                if (m_RendererID != 0) glDeleteBuffers(1, &m_RendererID);
                m_RendererID = other.m_RendererID;
                m_Count = other.m_Count;
                other.m_RendererID = 0;
                other.m_Count = 0;
            }
            return *this;
        }

        void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); }
        void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
        
        [[nodiscard]] uint32_t GetCount() const { return m_Count; }
        
        void UpdateData(const void* data, size_t size, size_t offset = 0) const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
        }
    };

} // namespace Gui