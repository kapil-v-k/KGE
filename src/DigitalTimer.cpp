#include "DigitalTimer.h"
#include <iostream>
#include <utility>
#include <glm/glm.hpp>

namespace Gui
{
    DigitalTimer::DigitalTimer(Font* mainFont)
    {
        m_digitalTimer_widget = Gui::UiObject::Create();
        if (m_digitalTimer_widget) {
            auto m_render_component = Gui::RenderComponent::Create(m_digitalTimer_widget.get());
            auto m_background_rect = Gui::RectanglePrimitive::Create(glm::vec2(400.0f, 200.0f), 25.0f);
            if (m_background_rect)
            {
                m_background_rect->SetColor(Gui::Color::White);
                m_background_rect->SetOutlineColor(Gui::Color::Magenta);
                m_background_rect->SetBorderThickness(4.0f);
                m_render_component->AddPrimitives(m_background_rect);
            }
            
            m_digitalTimer_widget->AddComponents(m_render_component);

            // --- FIXED: REMOVED auto TO PREVENT LOCAL MEMORY VARIABLE SHADOWING ---
            m_start_button_widget = Gui::UiObject::Create();
            m_start_button_widget->Move(-125.0f, 70.0f); 

            auto buttonRenderer = Gui::RenderComponent::Create(m_start_button_widget.get());
            
            m_startButtonBodyMesh = Gui::RectanglePrimitive::Create(glm::vec2(100.0f, 45.0f), 15.0f);
            if (m_startButtonBodyMesh) {
                m_startButtonBodyMesh->SetColor(Gui::Color::CarbonGrey);
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::Cyan);
                m_startButtonBodyMesh->SetBorderThickness(3.0f);
                m_startButtonBodyMesh->SetFilled(true);
                buttonRenderer->AddPrimitives(m_startButtonBodyMesh);
            }
            m_start_button_widget->AddComponents(buttonRenderer);

            m_startbuttonActionScript = Gui::ButtonComponent::Create(m_start_button_widget.get(), glm::vec2(350.0f, 90.0f));
            
            // Clear out static hardcoded color calls from the callback pass to prevent loop overrides
            m_startbuttonActionScript->OnClick([=]() {
                std::cout << "SUCCESS: Digital Timer Button Node Traversed and Fired Callback Click!" << std::endl;
            });

            m_start_button_widget->AddComponents(m_startbuttonActionScript);

            m_startbtnLabelText = Gui::TextComponent::Create();
            if (m_startbtnLabelText) {
                m_startbtnLabelText->SetFont(mainFont);
                m_startbtnLabelText->SetText("Start");
                m_startbtnLabelText->SetColor(Gui::Color::GhostWhite);
                m_startbtnLabelText->SetLocalPosition(-35.0f, 10.0f); 
                m_start_button_widget->AddComponents(m_startbtnLabelText);
            }

            m_digitalTimer_widget->AddChild(m_start_button_widget);
        }
    }

    std::shared_ptr<UiObject> DigitalTimer::GetWidget() const
    {
        return m_digitalTimer_widget;
    }

    void DigitalTimer::Update(float deltaTime, float normalizedMouseX, float normalizedMouseY, bool isClickingNow)
    {
       // m_digitalTimer_widget->Scale(1.5f);
        if (m_start_button_widget) {
            m_start_button_widget->UpdateInput(deltaTime, normalizedMouseX, normalizedMouseY, isClickingNow, glm::mat4(1.0f));
           // m_digitalTimer_widget->Move(100.0f,100.0f);
        }

        // ====================================================================
        // --- CONSOLIDATED UNIFIED WIDGET STATE MACHINE INTERFACE ------------
        // ====================================================================
        if (m_startbuttonActionScript) {
            if (m_startbuttonActionScript->IsFlashing()) {
                // 1. ACTIVE COMPONENT FLASH BLINK PROFILE
                // SafetyOrange backdrop creates a stunning pop while keeping the Black text 100% visible!
                m_startButtonBodyMesh->SetColor(Gui::Color::SafetyOrange);
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::Black);
                m_startButtonBodyMesh->SetBorderThickness(3.0f);
                if (m_startbtnLabelText) m_startbtnLabelText->SetColor(Gui::Color::Black);
            } 
            else if (m_startbuttonActionScript->IsHovered()) {
                // 2. ACTIVE STABLE HOVER SELECTION PROFILE
                m_startButtonBodyMesh->SetColor(Gui::Color::CarbonGrey);
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::NeonGreen);
                m_startButtonBodyMesh->SetBorderThickness(6.0f); 
                if (m_startbtnLabelText) m_startbtnLabelText->SetColor(Gui::Color::GhostWhite);
            } 
            else {
                // 3. BASELINE CONSOLE STATE PROFILE
                m_startButtonBodyMesh->SetColor(Gui::Color::CarbonGrey);
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::Cyan);
                m_startButtonBodyMesh->SetBorderThickness(3.0f); 
                if (m_startbtnLabelText) m_startbtnLabelText->SetColor(Gui::Color::GhostWhite);
            }
        }
    }

    DigitalTimer::~DigitalTimer() = default;
} // namespace Gui