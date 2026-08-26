#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "UiObject.hpp"

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

        // Persistent unique component text registers
        std::shared_ptr<Gui::TextComponent> m_DynamicCalendarText;
        std::shared_ptr<Gui::TextComponent> m_DynamicMinutesText;
        std::shared_ptr<Gui::TextComponent> m_DynamicSecondsText;
        std::shared_ptr<Gui::TextComponent> m_CityNameText;
        // --- FIXED: ADDED TRACKING FIELDS FOR DYNAMIC VIEWPORT SCALING ---
        float m_TimezoneOffsetHours = 5.5f; 
        float m_LocalScaleFactor    = 1.0f; 

    public:
        Clock_Widget(Font* mainFont, const std::string& cityName);
        ~Clock_Widget() = default;

        // Exposes the master container back to your Viewport context
        [[nodiscard]] std::shared_ptr<UiObject> GetWidget() const { return m_clock_widget; }

        // --- NEW PUBLIC INITIALIZATION HOOK METHODS ---
        void SetTimezoneOffset(float offsetHours) { m_TimezoneOffsetHours = offsetHours; }
        
        // Updates both the internal scale property and the root node transform symmetrically
        void SetScale(float scaleFactor) { 
            m_LocalScaleFactor = scaleFactor; 
            if (m_clock_widget) m_clock_widget->Scale(scaleFactor);
        }

        // Drives smooth hands rotations in real-time sync per frame tick
        void Update(float deltaTime);
    };

} // namespace Gui