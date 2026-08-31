#include "Primitive.h"
#include "BatchRenderer.hpp"
#include "Shader.hpp" 

namespace Gui {

    // 1. Base Class Default
    void Primitive::BindUniforms(const Shader& shader) const {
        shader.SetUniformInt("u_PrimitiveType", 0); // Lines/Points Mode
    }

    // 2. Circle Overrides
    void CirclePrimitive::BindUniforms(const Shader& shader) const {
        shader.SetUniformInt("u_PrimitiveType", 1); // Forces Branch 1 (Procedural Radar Circles)
        shader.SetUniformFloat("u_BorderThickness", m_BorderThickness);
        shader.SetUniformInt("u_IsFilled", m_IsFilled ? 1 : 0);
        shader.SetUniformVec4("u_OutlineColor", m_OutlineColor);
    }

    // 3. Rectangle Overrides
    void RectanglePrimitive::BindUniforms(const Shader& shader) const {
        shader.SetUniformInt("u_PrimitiveType", 2); // Forces Branch 2 (Rounded Plates)
        shader.SetUniformVec2("u_RectSize", m_Size);
        shader.SetUniformFloat("u_CornerRadius", m_CornerRadius);
        shader.SetUniformFloat("u_BorderThickness", m_BorderThickness);
        shader.SetUniformVec4("u_OutlineColor", m_OutlineColor);
    }

    //4. ThickLine Overrides
    void ThickLinePrimitive::BindUniforms(const Shader& shader) const{
        shader.SetUniformInt("u_PrimitiveType", 2); // Rounded Rectangle Mode 2
                
        float totalWidth  = m_Thickness + (m_BorderThickness * 2.0f);
        float totalHeight = m_Length + (m_BorderThickness * 2.0f);
                
        shader.SetUniformVec2("u_RectSize", glm::vec2(totalWidth, totalHeight));
        shader.SetUniformFloat("u_CornerRadius", totalWidth * 0.5f); // Smooth caps
                
        shader.SetUniformInt("u_IsFilled", m_IsFilled ? 1 : 0);
        shader.SetUniformVec4("u_OutlineColor", m_OutlineColor);
        shader.SetUniformFloat("u_BorderThickness", m_BorderThickness);
    }

    // void TrianglePrimitive::BindUniforms(const Shader& shader) const {
    //     // Tell the fragment shader to parse through the quad triangle SDF logic
    //     shader.SetUniformInt("u_PrimitiveType", 6);
    //     shader.SetUniformFloat("u_BorderThickness", m_BorderThickness);
    //     shader.SetUniformInt("u_IsFilled", m_IsFilled ? 1 : 0);
    //     shader.SetUniformVec4("u_OutlineColor", m_OutlineColor);
    //     shader.SetUniformVec2("u_RectSize", m_Size); // Passes bounding metrics
    // }
} // namespace Gui