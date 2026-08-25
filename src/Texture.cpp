#include "Texture.h"
#include <iostream>
#include <stdexcept>

// Force stb_image implementation macros to activate
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Gui {

    Texture::Texture(const std::string& path) : m_FilePath(path) {
        // OpenGL expects textures to start from the bottom-left corner, 
        // but image files store pixels starting from the top-left. 
        // We tell stb_image to flip the image vertically during loading!
        stbi_set_flip_vertically_on_load(1);

        // Load image data from disk into a raw unsigned byte array channel pointer
        unsigned char* localBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4); // Force 4 channels (RGBA)

        if (!localBuffer) {
            throw std::runtime_error("TEXTURE ERROR: Failed to load pixel data from file path: " + path);
        }

        // Generate an empty texture container slot on your graphics hardware
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        // Configure smooth sampling parameters for dynamic stretching and scaling operations
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Upload the raw CPU pixel byte buffer straight to your GPU's VRAM registers
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Housekeeping: Completely free the temporary CPU RAM buffer since it lives safely in VRAM now
        stbi_image_free(localBuffer);
        
        std::cout << "TEXTURE SYSTEM: Successfully allocated " << m_Width << "x" << m_Height << " GPU image slot.\n";
    }

    Texture::~Texture() {
        if (m_RendererID != 0) {
            glDeleteTextures(1, &m_RendererID); // Erase from graphics card RAM safely
        }
    }

} // namespace Gui