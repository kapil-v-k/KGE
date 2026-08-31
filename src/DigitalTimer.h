#pragma once
#include <memory>
#include "UiObject.hpp"
#include "Font.h"
#include "RenderComponent.hpp"
#include "Color.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "Primitive.h"

namespace Gui
{
    class DigitalTimer
    {
    private:
        std::shared_ptr<UiObject> m_digitalTimer_widget;
        std::shared_ptr<UiObject> m_start_button_widget;
        std::shared_ptr<RectanglePrimitive> m_startButtonBodyMesh;
        std::shared_ptr<ButtonComponent> m_startbuttonActionScript;
        std::shared_ptr<TextComponent> m_startbtnLabelText;

    public:
        explicit DigitalTimer(Font* mainFont);
        ~DigitalTimer();
        
        [[nodiscard]] std::shared_ptr<UiObject> GetWidget() const;
        void Update(float deltaTime, float normalizedMouseX, float normalizedMouseY, bool isClickingNow);
    };
} // namespace Gui