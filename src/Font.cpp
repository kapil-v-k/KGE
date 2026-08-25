// src/Font.cpp
#include "Font.h"
#include <iostream>
#include <stdexcept>

// --- FREETYPE CORE ENGINE HEADERS ---
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Gui {

        Font::Font(const std::string& fontFilePath, unsigned int fontSizePixelHeight) {
        // 1. Initialize FreeType infrastructure handles
        FT_Library freeTypeLib;
        if (FT_Init_FreeType(&freeTypeLib)) {
            throw std::runtime_error("FREETYPE ERROR: Failed to instantiate library context registers.");
        }

        // 2. Load the specific vector Font Face definitions from your .ttf disk asset
        FT_Face fontFace;
        if (FT_New_Face(freeTypeLib, fontFilePath.c_str(), 0, &fontFace)) {
            FT_Done_FreeType(freeTypeLib);
            throw std::runtime_error("FREETYPE ERROR: Failed to unpack vector data mapping for font layout: " + fontFilePath);
        }

        // 3. Set target layout dimensions (Width: 0 tells FreeType to compute width dynamically based on height)
        FT_Set_Pixel_Sizes(fontFace, 0, fontSizePixelHeight);

        // 4. Request an empty, monochrome data storage canvas from your graphics card
        glGenTextures(1, &m_AtlasTextureID);
        glBindTexture(GL_TEXTURE_2D, m_AtlasTextureID);
        
        // Allocate a 512x512 grid block using GL_RED (1 byte per pixel format)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_AtlasWidth, m_AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

        // Configure smooth sampling parameters for text curves scaling operations
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Enforce 1-byte row packing insulation
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // 5. --- THE ATLAS TILING PIPELINE ---
        // We will organize characters sequentially left-to-right into neat layout rows.
        int currentTilingX = 0;
        int currentTilingY = 0;
        int maximumRowHeightTracker = 0;

        // Step through standard printable ASCII character maps (32 = Space, 126 = Tilde (~))
        for (unsigned char asciiChar = 32; asciiChar <= 126; ++asciiChar) {
            // Load and render the specific character vector outline map
            if (FT_Load_Char(fontFace, asciiChar, FT_LOAD_RENDER)) {
                std::cerr << "FREETYPE WARNING: Failed to process glyph layout slot: " << (int)asciiChar << "\n";
                continue;
            }

            // Extract shortcuts to FreeType's internal rasterization outcomes
            unsigned int glyphWidth  = fontFace->glyph->bitmap.width;
            unsigned int glyphHeight = fontFace->glyph->bitmap.rows;

            // Check if we hit the right boundary edge of our 512px texture box...
            if (currentTilingX + glyphWidth + 2 >= static_cast<unsigned int>(m_AtlasWidth)) {
                // Wrap to a brand new row below!
                currentTilingX = 0;
                currentTilingY += maximumRowHeightTracker + 2; // Add a tiny 2-pixel safety margin padding
                maximumRowHeightTracker = 0;
            }

            // If the glyph has physical pixel contents (skip spaces which have zero width/height dimensions)
            if (glyphWidth > 0 && glyphHeight > 0) {
                // Copy FreeType's raw bitmap array straight into a sub-section of our pre-allocated GPU texture sheet!
                glTexSubImage2D(
                    GL_TEXTURE_2D, 0, 
                    currentTilingX, currentTilingY, 
                    glyphWidth, glyphHeight, 
                    GL_RED, GL_UNSIGNED_BYTE, 
                    fontFace->glyph->bitmap.buffer
                );
            }

            // 6. --- BAKE METRIC DATA FOR CACHE LOOKUPS ---
            Character character;
            character.size    = glm::ivec2(glyphWidth, glyphHeight);
            character.bearing = glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top);
            
            // FreeType stores advances using a fixed-point precision scale (1/64th of a pixel)
            // We bit-shift right by 6 (which divides by 64) to get absolute pixel metrics!
            character.advance = fontFace->glyph->advance.x >> 6;

            // Calculate exact UV coordinates mapped within standard [0.0 to 1.0] texture space coordinates
            character.textureCoordMin = glm::vec2(
                static_cast<float>(currentTilingX) / m_AtlasWidth,
                static_cast<float>(currentTilingY) / m_AtlasHeight
            );
            character.textureCoordMax = glm::vec2(
                static_cast<float>(currentTilingX + glyphWidth) / m_AtlasWidth,
                static_cast<float>(currentTilingY + glyphHeight) / m_AtlasHeight
            );

            // Save the baked module inside our constant-time Hash Table dictionary map cache
            m_CharacterCache[asciiChar] = character;

            // Shift our horizontal tiling coordinate slider forward to prepare for the next letter
            currentTilingX += glyphWidth + 2;

            if (glyphHeight > static_cast<unsigned int>(maximumRowHeightTracker)) {
                maximumRowHeightTracker = glyphHeight;
            }
        }

        // 7. Pipeline Housekeeping: Teardown FreeType contexts safely from memory
        FT_Done_Face(fontFace);
        FT_Done_FreeType(freeTypeLib);
        
        std::cout << "FONT ENGINE MODULE: Successfully generated uniform 512x512 text atlas texture context.\n";
    }

    float Font::GetStringWidth(const std::string& text) const {
        float totalWidth = 0.0f;
        for (char c : text) {
            auto it = m_CharacterCache.find(c);
            if (it != m_CharacterCache.end()) {
                // --- FIXED: ACCUMULATE PRE-SCALED RAW PIXEL ADVANCES NATIVELY ---
                // Removing '/ 64.0f' stops text components from flattening into micro-widths!
                totalWidth += static_cast<float>(it->second.advance); 
            }
        }
        return totalWidth;
    }
    
    Font::~Font() {
        if (m_AtlasTextureID != 0) {
            glDeleteTextures(1, &m_AtlasTextureID); // Erase the texture sheet from VRAM
        }
    }

} // namespace Gui