#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Gui {

    // --- STRUCTURAL VALUE PROPERTY FOR GLYPH TRACKING ---
    struct Character {
        glm::vec2 textureCoordMin; // Top-Left UV coordinate mapping on the atlas sheet [0.0 to 1.0]
        glm::vec2 textureCoordMax; // Bottom-Right UV coordinate mapping on the atlas sheet [0.0 to 1.0]
        glm::ivec2 size;           // Source pixel size dimensions (width and height) of the letter
        glm::ivec2 bearing;        // Top-left alignment offset distance relative to the typing baseline
        unsigned int advance;      // Horizontal pixel cursor jump distance to the next character element
    };

    class Font {
    private:
        unsigned int m_AtlasTextureID{ 0 };
        int m_AtlasWidth{ 512 };  // Baseline layout sheet grid scale width
        int m_AtlasHeight{ 512 }; // Baseline layout sheet grid scale height
        
        // Rapid lookup dictionary mapping raw ASCII chars to their metric blocks
        std::unordered_map<char, Character> m_CharacterCache;

    public:
        // Pass the absolute file path to the TrueType font and target pixel height resolution
        Font(const std::string& fontFilePath, unsigned int fontSizePixelHeight);
        ~Font();

        // Prevent resource copies to prevent accidental GPU double-free bugs
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        // Resource accessors for the TextComponent quad loops
        [[nodiscard]] const Character& GetCharacter(char asciiCode) const { 
            auto it = m_CharacterCache.find(asciiCode);
            if (it == m_CharacterCache.end()) {
                // Fallback: If character is missing, return a simple space symbol glyph safely!
                return m_CharacterCache.at(' '); 
            }
            return it->second;
         }
        [[nodiscard]] float GetStringWidth(const std::string& text) const;
        [[nodiscard]] unsigned int GetAtlasTextureID() const { return m_AtlasTextureID; }
        [[nodiscard]] int GetAtlasWidth() const { return m_AtlasWidth; }
        [[nodiscard]] int GetAtlasHeight() const { return m_AtlasHeight; }
    };

} // namespace Gui