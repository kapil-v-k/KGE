#include "Clock.h"
#include "UiObject.hpp"
#include "RenderComponent.hpp"
#include "TextComponent.h"
#include "ClipContainerComponent.h"
#include "Primitive.h"
#include "Color.h"
#include "Font.h"
#include <chrono>
#include <ctime>
#include <cmath>

namespace Gui
{
    Clock_Widget::Clock_Widget(Font* mainFont, const std::string& cityName)
    {
        m_clock_widget = Gui::UiObject::Create();
        m_clock_widget->Move(0.0f, 0.0f); 

        // 1. --- FIXED: INCREASE MASK SIZE TO 600x600 TO PREVENT BORDER CUTS ---
        // Expanding the scissor bounds guarantees the outer hour display ring 
        // never clips past the edge boundaries, eliminating cuts completely!
        auto clock_scissor = Gui::ClipContainerComponent::Create(600, 600);
        auto scissor_shape = std::make_unique<Gui::RectanglePrimitive>(glm::vec2(500.0f, 500.0f), 25.0f);
        clock_scissor->AddPrimitives(std::move(scissor_shape));
        m_clock_widget->AddComponents(clock_scissor);
        
        // 2. CHASSIS OUTER FRAME MESH BODIES
        auto background_rendrer_item = Gui::RenderComponent::Create(m_clock_widget.get());
        auto background = Gui::RectanglePrimitive::Create(glm::vec2(500.0f, 500.0f), 25.0f);
       // auto background = background_rendrer_item->AddPrimitives<Gui::RectanglePrimitive>(glm::vec2(500.0f, 500.0f), 25.0f);
        if (background) {
            background->SetColor(Gui::Color::CarbonGrey); 
            background->SetOutlineColor(Gui::Color::Blue);
            background->SetBorderThickness(4.0f);
            background_rendrer_item->AddPrimitives(background);
        }

        auto circle_item = Gui::RenderComponent::Create(m_clock_widget.get());
        auto circle_rendrer = Gui::CirclePrimitive::Create(glm::vec2(0.0f, 0.0f), 160.0f);
        //auto circle_rendrer = circle_item->template AddPrimitives<Gui::CirclePrimitive>(glm::vec2(0.0f, 0.0f), 160.0f);
        if (circle_rendrer) {
            circle_rendrer->SetColor(Gui::Color::WithAlpha(Gui::Color::Cyan, 0.1f));
            circle_rendrer->SetOutlineColor(Gui::Color::Green); 
            circle_rendrer->SetBorderThickness(4.0f);
            circle_rendrer->SetFilled(false);
            circle_item->AddPrimitives(circle_rendrer);
        }
        
        // Console Header Plate text overlay
        auto text_title = std::make_shared<Gui::TextComponent>();
        if (text_title != nullptr) {
            text_title->SetFont(mainFont);
            text_title->SetText("TIME DISPLAY CONSOLE");
            text_title->SetColor(Gui::Color::NeonGreen);
            text_title->SetLocalScale(1.0f);
            text_title->SetLocalPosition(-185.0f, -185.0f); 
            m_clock_widget->AddComponents(background_rendrer_item, circle_item, text_title);
        }

        // ====================================================================
        // --- FIXED FEATURE: INTERNAL HEADLINE CITY TEXT REGISTRATION ---
        // ====================================================================
        // Positioning it inside the clock_widget tree at (0.0f, -85.0f) centers
        // it inside the 500x500 box without shifting the core clock layout pivot!
        auto cityLabelWidget = Gui::UiObject::Create();
        cityLabelWidget->Move(-90.0f, -35.0f); 

        m_CityNameText = std::make_shared<Gui::TextComponent>();
        if (m_CityNameText) {
            m_CityNameText->SetFont(mainFont);
            m_CityNameText->SetText(cityName);
            m_CityNameText->SetColor(Gui::Color::Cyan); 
            m_CityNameText->SetLocalScale(1.0f); 
            
            // --- FIXED: ZEROING OUT LOCAL OFFSETS RESOLVES CRITICAL SHIFTING TRAPS ---
            m_CityNameText->SetLocalPosition(0.0f, 0.0f); 
            
            cityLabelWidget->AddComponents(m_CityNameText);
        }
        m_clock_widget->AddChild(cityLabelWidget);

        // --- FEATURE: CENTER DIGITAL DATE READOUT ---
        m_seconds_display_widget = Gui::UiObject::Create();
        m_seconds_display_widget->Move(0.0f, -50.0f); 
        m_DynamicCalendarText = std::make_shared<Gui::TextComponent>();
        if (m_DynamicCalendarText) {
            m_DynamicCalendarText->SetFont(mainFont);
            m_DynamicCalendarText->SetColor(Gui::Color::NeonGreen);
            m_DynamicCalendarText->SetLocalScale(1.0f);
            m_DynamicCalendarText->SetLocalPosition(-195.0f, 250.0f);
            m_seconds_display_widget->AddComponents(m_DynamicCalendarText);
        }
        m_clock_widget->AddChild(m_seconds_display_widget);

        // 3. GENERATE THE 12 HOURLY CORE TIME NUMERALS (NO NOTCHES OR TICK MARKS)
        float radius = 135.0f;
        float currentAngle = 0.0f; 
        int hourToken = 12;

        while (currentAngle < 360.0f) {
            if (std::fmodf(currentAngle, 30.0f) == 0.0f) {
                float radians = currentAngle * (3.14159265f / 180.0f);
                float textX = radius * glm::sin(radians);
                float textY = -(radius * glm::cos(radians));

                std::string timeStr = std::to_string(hourToken);
                float currentScale = 1.0f;
                float textHalfWidth  = -(((timeStr.length() == 1 ? 14.0f : 24.0f) * currentScale) * 0.5f);
                float textHalfHeight = -((22.0f * currentScale) * 0.2f);

                auto text_widget = Gui::UiObject::Create();
                text_widget->Move(textX, textY); 

                auto text_time = std::make_shared<Gui::TextComponent>();
                if (text_time != nullptr) {
                    text_time->SetFont(mainFont); 
                    text_time->SetText(timeStr);
                    text_time->SetColor(Gui::Color::Green);
                    text_time->SetLocalPosition(textHalfWidth, -textHalfHeight + 4.0f); 
                    text_time->SetLocalScale(currentScale);
                    text_widget->AddComponents(text_time);
                }
                m_clock_widget->AddChild(text_widget);
                currentAngle += 30.0f;
                hourToken = (hourToken == 12) ? 1 : hourToken + 1;
            }
        }

        

        // ====================================================================
        // 4. INDICATOR NEEDLES ASSEMBLY LAYOUT
        // ====================================================================
        m_hours_hand_widget = Gui::UiObject::Create();
        auto m_hours_hand = Gui::RenderComponent::Create(m_hours_hand_widget.get());
        auto h_needle = Gui::ThickLinePrimitive::Create(80.0f, 8.0f);
        //auto h_needle = m_hours_hand->template AddPrimitives<Gui::ThickLinePrimitive>(80.0f, 8.0f);
        if (h_needle) {
             h_needle->SetColor(Gui::Color::Platinum_100);
             m_hours_hand->AddPrimitives(h_needle);
        }
        m_hours_hand_widget->AddComponents(m_hours_hand);
        m_hours_hand_widget->Move(0.0f, 0.0f);
       // m_clock_widget->AddChild(m_hours_hand_widget);

        m_minutes_hand_widget = Gui::UiObject::Create();
        auto m_minutes_hand_comp = Gui::RenderComponent::Create(m_minutes_hand_widget.get());
        auto m_needle = Gui::ThickLinePrimitive::Create(120.0f, 5.0f);
        //auto m_needle = m_minutes_hand_comp->template AddPrimitives<Gui::ThickLinePrimitive>(120.0f, 5.0f);
        if (m_needle){
            m_needle->SetColor(Gui::Color::Sky_500);
            m_minutes_hand_comp->AddPrimitives(m_needle);
        }
        m_minutes_hand_widget->AddComponents(m_minutes_hand_comp);
        m_minutes_hand_widget->Move(0.0f, 0.0f);
       // m_clock_widget->AddChild(m_minutes_hand_widget);

        m_seconds_hand_widget = Gui::UiObject::Create();
        auto seconds_hand_rendrer = Gui::RenderComponent::Create(m_seconds_hand_widget.get());
        
        // --- FIXED: ADDED STANDARD TEMPLATE PARAMETER ANGLE BRACKETS ---
        auto s_needle = Gui::ThickLinePrimitive::Create(120.0f, 4.0f);
        //auto s_needle = seconds_hand_rendrer->template AddPrimitives<Gui::ThickLinePrimitive>(120.0f, 4.0f);
        if (s_needle) {
            s_needle->SetColor(Gui::Color::White);
            s_needle->SetOutlineColor(Gui::Color::White);
            seconds_hand_rendrer->AddPrimitives(s_needle);
        }
        
        // --- FIXED: ADDED STANDARD TEMPLATE PARAMETER ANGLE BRACKETS ---
        auto arrow = Gui::TrianglePrimitive::Create(glm::vec2(-15.0f, -125.0f), 
            glm::vec2(15.0f,  -125.0f), 
            glm::vec2(0.0f,   -155.0f));
        // auto arrow = seconds_hand_rendrer->template AddPrimitives<Gui::TrianglePrimitive>(
        //     glm::vec2(-15.0f, -125.0f), 
        //     glm::vec2(15.0f,  -125.0f), 
        //     glm::vec2(0.0f,   -155.0f)  
        // );
        if (arrow) {
            arrow->SetColor(Gui::Color::WithAlpha(Gui::Color::CyberOrange,0.2f));
            arrow->SetOutlineColor(Gui::Color::CyberOrange);
            arrow->SetBorderThickness(1.5f);
            seconds_hand_rendrer->AddPrimitives(arrow);
        }
        m_seconds_hand_widget->AddComponents(seconds_hand_rendrer);
        m_seconds_hand_widget->Move(0.0f, 0.0f);
        m_clock_widget->AddChild(m_seconds_hand_widget);

        // ====================================================================
        // --- SECONDS HUB ATTACHED AS THE ABSOLUTE LAST NODE PASS ---
        // ====================================================================
        // Registering this at the absolute end ensures the central seconds text circle
        // layers perfectly on top of all hands instead of being hidden behind them!
        m_clock_widget->AddChild(m_seconds_triangle_widget);

        // ====================================================================
        // --- FEATURE 1: DYNAMIC TRACKING HOUR_DISPLAY OVERLAY CIRCLE ---
        // ====================================================================
        m_hours_highlight_widget = Gui::UiObject::Create();
        auto hourDisplayRenderer = Gui::RenderComponent::Create(m_hours_highlight_widget.get());
        auto hCirclePrimitive = Gui::CirclePrimitive::Create(glm::vec2(0.0f, 0.0f), 24.0f);
        //auto hCirclePrimitive = hourDisplayRenderer->template AddPrimitives<Gui::CirclePrimitive>(glm::vec2(0.0f, 0.0f), 24.0f);
        if (hCirclePrimitive) {
            hCirclePrimitive->SetColor(Gui::Color::WithAlpha(Gui::Color::PureDark, 0.2f));
            hCirclePrimitive->SetOutlineColor(Gui::Color::WarningOrange);
            hCirclePrimitive->SetBorderThickness(2.5f);
            hourDisplayRenderer->AddPrimitives(hCirclePrimitive);
        }
        m_hours_highlight_widget->AddComponents(hourDisplayRenderer);

        // ====================================================================
        // --- FEATURE 2: FIXED DUAL-NESTED MINUTES OVERLAY CIRCLE & TEXT ---
        // ====================================================================
        m_minutes_display_widget = Gui::UiObject::Create();
        auto minDisplayRenderer = Gui::RenderComponent::Create(m_minutes_display_widget.get());
        
        // --- FIXED: ADDED THE MISSING MINUTE PRIMITIVE CIRCLE FRAME MESH ---
        auto minCirclePrim = Gui::CirclePrimitive::Create(glm::vec2(0.0f, 0.0f), 25.0f);
        //auto minCirclePrim = minDisplayRenderer->template AddPrimitives<Gui::CirclePrimitive>(glm::vec2(0.0f, 0.0f), 25.0f);
        if (minCirclePrim) {
            minCirclePrim->SetColor(Gui::Color::WithAlpha(Gui::Color::PureDark, 0.3f));
            minCirclePrim->SetOutlineColor(Gui::Color::WarningOrange);
            minCirclePrim->SetBorderThickness(2.5f);
            minDisplayRenderer->AddPrimitives(minCirclePrim);
        }
        m_minutes_display_widget->AddComponents(minDisplayRenderer);

        m_DynamicMinutesText = std::make_shared<Gui::TextComponent>();
        if (m_DynamicMinutesText) {
            m_DynamicMinutesText->SetFont(mainFont);
            m_DynamicMinutesText->SetColor(Gui::Color::GhostWhite);
            m_DynamicMinutesText->SetLocalScale(1.0f); // Enforced 1.0f scale
            m_DynamicMinutesText->SetLocalPosition(-16.0f, 13.0f); 
            m_minutes_display_widget->AddComponents(m_DynamicMinutesText);
        }
        
        // --- FIXED: RADIAL PLACEMENT MOVED INSIDE THE HOUR HIGHLIGHT RING ---
        // Shifting negatively along the Y-axis moves the minutes sub-circle inward,
        // locking it neatly inside the perimeter of your tracking hour circle!
        //m_minutes_display_widget->Move(0.0f, 34.0f); 
        
        //m_hours_highlight_widget->AddChild(m_minutes_display_widget);
        m_clock_widget->AddChild(m_hours_highlight_widget);
        m_clock_widget->AddChild(m_minutes_display_widget);

        // ====================================================================
        // --- FEATURE 3: DYNAMIC SECONDS TEXT DISPLAY IN A CENTER HUB CIRCLE ---
        // ====================================================================
        m_seconds_triangle_widget = Gui::UiObject::Create();
        m_seconds_triangle_widget->Move(0.0f, 0.0f); 
        auto centerHubRenderer = Gui::RenderComponent::Create(m_seconds_triangle_widget.get());
        auto hubCircle = Gui::CirclePrimitive::Create(glm::vec2(0.0f, 0.0f), 28.0f);
        //auto hubCircle = centerHubRenderer->template AddPrimitives<Gui::CirclePrimitive>(glm::vec2(0.0f, 0.0f), 28.0f);
        if (hubCircle) {
            hubCircle->SetColor(Gui::Color::WithAlpha(Gui::Color::PureDark, 0.4f));
            hubCircle->SetOutlineColor(Gui::Color::WarningOrange);
            hubCircle->SetBorderThickness(2.0f);
            centerHubRenderer->AddPrimitives(hubCircle);
        }
        
        m_DynamicSecondsText = std::make_shared<Gui::TextComponent>();
        if (m_DynamicSecondsText) {
            m_DynamicSecondsText->SetFont(mainFont);
            m_DynamicSecondsText->SetColor(Gui::Color::CyberOrange);
            m_DynamicSecondsText->SetLocalScale(1.0f); // Enforced 1.0f scale
            m_DynamicSecondsText->SetLocalPosition(-14.0f, 08.0f);
            
            // --- FIXED: SEPARATED VARIADIC METHOD ARGUMENTS TO PREVENT CORRUPTIONS ---
            // Attaching components sequentially guarantees your text object is successfully
            // registered to the center node tree pass instead of being stripped out!
            m_seconds_triangle_widget->AddComponents(centerHubRenderer);
            m_seconds_triangle_widget->AddComponents(m_DynamicSecondsText);
        }
        m_clock_widget->AddChild(m_seconds_triangle_widget);
    }

    void Clock_Widget::Update(float deltaTime) {
        auto now = std::chrono::system_clock::now();
        time_t currentTime = std::chrono::system_clock::to_time_t(now);
        
        float activeTimezoneOffsetHours = m_TimezoneOffsetHours; 
        
        time_t adjustedTime = currentTime + static_cast<time_t>(activeTimezoneOffsetHours * 3600.0f);
        struct tm* localTime = std::gmtime(&adjustedTime);

        // --- FIXED: DECLARED THE EXPLICIT TARGET CONVERSION DATA TYPES ---
        float smoothSeconds = static_cast<float>(localTime->tm_sec);
        float smoothMinutes = static_cast<float>(localTime->tm_min) + (smoothSeconds / 60.0f);
        float smoothHours   = static_cast<float>(localTime->tm_hour % 12) + (smoothMinutes / 60.0f);

        if (m_seconds_hand_widget) m_seconds_hand_widget->SetRotation(smoothSeconds * 6.0f);
        if (m_minutes_hand_widget) m_minutes_hand_widget->SetRotation(smoothMinutes * 6.0f);
        if (m_hours_hand_widget)   m_hours_hand_widget->SetRotation(smoothHours * 30.0f);

        // ====================================================================
        // --- FIXED: LONG LITERAL CALENDAR TIMELINE STRINGS GENERATION ---
        // ====================================================================
        if (m_DynamicCalendarText) {
            // Expanded lookup arrays translate raw indices into beautiful long word tokens
            const std::string fullDays[] = { 
                "   Sunday", "   Monday", "  Tuesday", "Wednesday", " Thursday", "   Friday", " Saturday" 
            };
            const std::string fullMonths[] = { 
                "January", "February", "March", "April", "May", "June", 
                "July", "August", "September", "October", "November", "December" 
            };

            std::string dayNameName = fullDays[localTime->tm_wday];
            std::string monthNameStr = fullMonths[localTime->tm_mon];
            
            // Keeps the raw numeral step for days without forced leading zeros
            std::string dayOfMonthStr = std::to_string(localTime->tm_mday);
            std::string yearValueStr  = std::to_string(localTime->tm_year + 1900);

            // --- FIXED: STRIP OUT INT SLASHES TO CLEANLY MATCH THE REQUESTED FORMAT ---
            // Compiles data blocks right-to-left: "Tuesday 25 August 2026"
            m_DynamicCalendarText->SetText(dayNameName + " " + dayOfMonthStr + " " + monthNameStr + " " + yearValueStr);
        }

        if (m_DynamicMinutesText) {
            int mins = localTime->tm_min;
            m_DynamicMinutesText->SetText((mins < 10 ? "0" : "") + std::to_string(mins));
        }

        if (m_DynamicSecondsText) {
            int secs = localTime->tm_sec;
            m_DynamicSecondsText->SetText((secs < 10 ? "0" : "") + std::to_string(secs));
        }

        if (m_hours_highlight_widget) {
            float hourRadius = 135.0f;
            
            // --- FIXED: DECLARED THE EXPLICIT TARGET CONVERSION DATA TYPES ---
            int currentHourInt = static_cast<int>(smoothHours);
            float hourAngleDegrees = static_cast<float>(currentHourInt * 30);
            float hourAngleRadians = hourAngleDegrees * (3.14159265f / 180.0f);
            
            float targetX = hourRadius * glm::sin(hourAngleRadians);
            float targetY = -(hourRadius * glm::cos(hourAngleRadians));
            
            m_hours_highlight_widget->Move(targetX, targetY);
        }

        if(m_minutes_display_widget)
        {
            float minutesRadius = 80.0f;
            int currentMinutesInt = static_cast<int>(smoothMinutes);
            float minutesAngleDegrees = static_cast<float>(currentMinutesInt * 6);
            float minutesAngleRadians = minutesAngleDegrees * (3.14159265f / 180.0f);
            
            float targetX = minutesRadius * glm::sin(minutesAngleRadians);
            float targetY = -(minutesRadius * glm::cos(minutesAngleRadians));
            
            m_minutes_display_widget->Move(targetX, targetY);
        }
    }
} // namespace Gui