#pragma once
#include <glad/glad.h>
#include <string>

namespace Gui {

    class Texture {
    private:
        unsigned int m_RendererID{ 0 };
        std::string m_FilePath;
        int m_Width{ 0 };
        int m_Height{ 0 };
        int m_BPP{ 0 }; // Bits per pixel (channels)

    public:
        Texture(const std::string& path);
        ~Texture();

        // Prevent copying resources to eliminate accidental double-free bugs on VRAM
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void Bind(unsigned int slot = 0) const {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
        }

        void Unbind() const {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        [[nodiscard]] unsigned int GetID() const { return m_RendererID; }
        [[nodiscard]] int GetWidth() const { return m_Width; }
        [[nodiscard]] int GetHeight() const { return m_Height; }
    };

} // namespace Gui