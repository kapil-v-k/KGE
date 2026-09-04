#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace Gui {
    class UiObject;
    class ButtonComponent;
    class RectanglePrimitive;
    class TextComponent;
    class Font;
}

namespace Gui
{
    class DigitalTimer
    {
    private:
        std::shared_ptr<Gui::UiObject> m_digitalTimer_widget;
        std::shared_ptr<Gui::UiObject> m_start_button_widget;
        std::shared_ptr<Gui::ButtonComponent> m_startbuttonActionScript;
        std::shared_ptr<Gui::RectanglePrimitive> m_startButtonBodyMesh;
        std::shared_ptr<Gui::TextComponent> m_startbtnLabelText;

    public:
        DigitalTimer(Gui::Font* mainFont);
        ~DigitalTimer();

        [[nodiscard]] std::shared_ptr<Gui::UiObject> GetWidget() const;

        // --- FIXED SYNCHRONIZED UPDATE DECLARATION ---
        void Update(float deltaTime, float normalizedMouseX, float normalizedMouseY, bool isClickingNow, const glm::mat4& projectionMatrix);
    };
} // namespace Gui