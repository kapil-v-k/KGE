#include "HardwareScissorLock.h"

// --- FIXED: Step up one directory layer safely to find your core batcher header! ---
#include "../BatchRenderer.hpp" 

namespace Gui {

    HardwareScissorLock::HardwareScissorLock(BatchRenderer& batcher, const glm::mat4& combinedTransformMatrix, const glm::vec2& containerSize)
        : m_Batcher(batcher) 
    {
        m_Batcher.Flush();
        m_WasEnabled = glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE;

        float absoluteScreenX = combinedTransformMatrix[3][0];
        float absoluteScreenY = combinedTransformMatrix[3][1];

        GLint activeViewportBounds[4];
        glGetIntegerv(GL_VIEWPORT, activeViewportBounds);

        float viewportOffsetX = static_cast<float>(activeViewportBounds[0]);
        float viewportOffsetY = static_cast<float>(activeViewportBounds[1]);
        
        // --- FIXED: READ WINDOW DIMENSIONS DYNAMICALLY TO RESOLVE 1080P VIEWPORTS ---
        // Replacing hardcoded 720.0f scaling guarantees correct coordinates at all resolutions!
        float targetWindowHeight = static_cast<float>(activeViewportBounds[3]); 

        float absoluteX = absoluteScreenX + viewportOffsetX;
        float absoluteY = absoluteScreenY + viewportOffsetY;

        int scissorX = static_cast<int>(absoluteX - (containerSize.x * 0.5f));
        int scissorY = static_cast<int>(targetWindowHeight - (absoluteY + (containerSize.y * 0.5f)));
        int scissorW = static_cast<int>(containerSize.x);
        int scissorH = static_cast<int>(containerSize.y);

        if (scissorW < 0) scissorW = 0;
        if (scissorH < 0) scissorH = 0;

        glEnable(GL_SCISSOR_TEST);
        glScissor(scissorX, scissorY, scissorW, scissorH);
    }

    HardwareScissorLock::~HardwareScissorLock() {
        // Force the active children meshes to paint safely within current scissor bounds
        m_Batcher.Flush();

        // Restore the graphics state machine back to its pre-existing baseline rule layout
        if (!m_WasEnabled) {
            glDisable(GL_SCISSOR_TEST);
        }
    }

} // namespace Gui