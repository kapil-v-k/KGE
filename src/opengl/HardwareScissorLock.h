#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Gui {

    // --- FIXED: FORWARD DECLARATION BREAKS PATH DEPENDENCIES FOREVER ---
    class BatchRenderer;

    struct HardwareScissorLock {
    private:
        BatchRenderer& m_Batcher;
        bool m_WasEnabled{ false };

    public:
        // Constructor declaration
        HardwareScissorLock(BatchRenderer& batcher, const glm::mat4& combinedTransformMatrix, const glm::vec2& containerSize);
        
        // Destructor declaration automatically handles automatic cleanup loops
        ~HardwareScissorLock();

        // Prohibit duplication to prevent accidental double-flush state corruptions
        HardwareScissorLock(const HardwareScissorLock&) = delete;
        HardwareScissorLock& operator=(const HardwareScissorLock&) = delete;
    };

} // namespace Gui