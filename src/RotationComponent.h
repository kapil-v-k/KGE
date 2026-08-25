#pragma once
#include "Component.h"

namespace Gui {
    class RotationComponent : public Component {
        private:
            float m_Speed;
        public:
            RotationComponent(UiObject* owner, float speed) : Component(owner), m_Speed(speed) {}

            // --- APPROACH B BLUEPRINT FACTORY ---
            [[nodiscard]] static std::unique_ptr<RotationComponent> Create(float speed) {
                return std::make_unique<RotationComponent>(nullptr, speed);
            }
            ~RotationComponent() override = default;

            void setSpeed(float speed){ m_Speed = speed;}
            void Update(float deltaTime) override {
                if(m_Owner)
                {
                    m_Owner->Rotate(m_Speed * deltaTime);
                }
            }
    };
}