#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Gui {
    class UiObject;
    class TextComponent;
    class Font;
}

namespace Gui {

    class Clock_Widget {
    private:
        // Core structural tree nodes
        std::shared_ptr<UiObject> m_clock_widget;
        std::shared_ptr<UiObject> m_seconds_hand_widget;
        std::shared_ptr<UiObject> m_minutes_hand_widget;
        std::shared_ptr<UiObject> m_hours_hand_widget;

        // Dynamic status sub-widgets
        std::shared_ptr<UiObject> m_seconds_display_widget;
        std::shared_ptr<UiObject> m_hours_highlight_widget;
        std::shared_ptr<UiObject> m_seconds_triangle_widget;
        std::shared_ptr<UiObject> m_minutes_display_widget;

        // ====================================================================
        // --- FIXED: ADDED REGISTER ENTRIES SAVES COMPILATION PATHS FOREVER ---
        // ====================================================================
        // These declarations give your Clock.cpp file stable, persistent access 
        // to your dynamic calendar labels, orbiting minute counters, and center seconds text!
        std::shared_ptr<Gui::TextComponent> m_DynamicCalendarText;
        std::shared_ptr<Gui::TextComponent> m_DynamicMinutesText;
        std::shared_ptr<Gui::TextComponent> m_DynamicSecondsText;

    public:
        Clock_Widget(Font* mainFont);
        ~Clock_Widget() = default;

        // Exposes the master container back to your Viewport context
        [[nodiscard]] std::shared_ptr<UiObject> GetWidget() const { return m_clock_widget; }

        // Drives smooth hands rotations in real-time sync
        void Update(float deltaTime);
    };

} // namespace Gui