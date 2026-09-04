#include "DigitalTimer.h"
#include "UiObject.hpp"
#include "RenderComponent.hpp"
#include "Primitive.h"
#include "Color.h"
#include "ButtonComponent.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Gui
{
    DigitalTimer::DigitalTimer(Font* /*mainFont*/)
    {
        // 1. Initialize Root Base Container Panel
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

            // 2. Initialize Standalone Button Node Sibling Branch
            m_start_button_widget = Gui::UiObject::Create();
            m_start_button_widget->Move(100.0f, 45.0f); // Fully tracks any parent transformations fluidly!
            m_start_button_widget->Scale(2.0f);
            //m_start_button_widget->SetRotation(45.0f);

            auto buttonRenderer = Gui::RenderComponent::Create(m_start_button_widget.get());

            m_startButtonBodyMesh = Gui::RectanglePrimitive::Create(glm::vec2(100.0f, 45.0f), 0.0f);
            if (m_startButtonBodyMesh) {
                m_startButtonBodyMesh->SetColor(Gui::Color::CarbonGrey);
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::Cyan);
                m_startButtonBodyMesh->SetBorderThickness(3.0f);
                m_startButtonBodyMesh->SetFilled(true);
                buttonRenderer->AddPrimitives(m_startButtonBodyMesh); 
            }
            m_start_button_widget->AddComponents(buttonRenderer);

            // 3. Initialize the physics tracking component script straight onto the button node
            m_startbuttonActionScript = Gui::ButtonComponent::Create(m_start_button_widget.get(), glm::vec2(100.0f, 45.0f));
            m_startbuttonActionScript->OnClick([=]() {
                std::cout << "SUCCESS: Precision Aligned Recursive Sibling Button Clicked!" << std::endl;
            });
            m_start_button_widget->AddComponents(m_startbuttonActionScript);

            m_startbtnLabelText = nullptr; 
            
            // Build the clean hierarchical relationship cascade tree natively
            m_digitalTimer_widget->AddChild(m_start_button_widget);
        }
    }

    std::shared_ptr<UiObject> DigitalTimer::GetWidget() const
    {
        return m_digitalTimer_widget;
    }

    void DigitalTimer::Update(float deltaTime, float normalizedMouseX, float normalizedMouseY, bool isClickingNow, const glm::mat4& projectionMatrix)
    {
        // Driving inputs from the root container via a clean Identity Matrix baseline 
        // allows the scene graph to multiply nested coordinates correctly, destroying drift!
        if (m_digitalTimer_widget) {
            m_digitalTimer_widget->UpdateInput(
                deltaTime, 
                normalizedMouseX, 
                normalizedMouseY, 
                isClickingNow, 
                glm::mat4(1.0f), // Clean identity matrix baseline initialization
                projectionMatrix
            );
        }

        // Handle active border stroke highlights dynamically
        if (m_startbuttonActionScript && m_startButtonBodyMesh) {
            if (m_startbuttonActionScript->IsFlashing()) {
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::SafetyOrange);
                m_startButtonBodyMesh->SetBorderThickness(6.0f);
            } 
            else if (m_startbuttonActionScript->IsHovered()) {
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::NeonGreen);
                m_startButtonBodyMesh->SetBorderThickness(6.0f); 
            } 
            else {
                m_startButtonBodyMesh->SetOutlineColor(Gui::Color::Cyan);
                m_startButtonBodyMesh->SetBorderThickness(3.0f); 
            }
        }
    }

    DigitalTimer::~DigitalTimer() = default;
} // namespace Gui