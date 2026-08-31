#pragma once
#include<glm/glm.hpp>
#include<memory>
#include<vector>
//#include"BatchRenderer.hpp"
#include "Shader.hpp"

namespace Gui { class Shader; }

namespace Gui {

    // Pack structure tightly for GPU memory alignment
    struct Vertex {
        glm::vec2 position; // 8 bytes
        glm::vec4 color;    // 16 bytes
        glm::vec2 texCoord; // 8 bytes (Used for circles, text, or textured shapes)
    };

    class BatchRenderer;
    // --- ENFORCE TYPE SAFETY WITH A SCOPED ENUM ---
    enum class Topology : unsigned int {
        Lines     = 0x0001, // Maps directly to GL_LINES
        Triangles = 0x0004  // Maps directly to GL_TRIANGLES
    };


    // Lightweight representation of a completed draw batch
    struct PrimitiveMesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    enum class PrimitiveType {
        Point,
        Line,
        Circle,
        Rectangle,
        Text,
        Texture,
        Triangle,
    };

    class Primitive {
    protected:
        glm::vec4 m_Color{1.0f, 1.0f, 1.0f, 1.0f}; // Default White
        PrimitiveType m_Type;

        bool          m_IsFilled{ true };
        glm::vec4     m_OutlineColor{ 1.0f, 1.0f, 1.0f, 1.0f };
        float         m_BorderThickness{ 2.0f }; // Default thickness in physical screen pixels

    public:
        explicit Primitive(PrimitiveType type) : m_Type(type) {}
        virtual ~Primitive() = default;

        // User-friendly style APIs
        void SetColor(const glm::vec4& color) { m_Color = color; }
        [[nodiscard]] glm::vec4 GetColor() const { return m_Color; }
        [[nodiscard]] PrimitiveType GetType() const { return m_Type; }

        void SetFilled(bool filled) { m_IsFilled = filled; }
        void SetOutlineColor(const glm::vec4& color) { m_OutlineColor = color; }
        void SetBorderThickness(float thickness) { m_BorderThickness = thickness; }
    
        virtual void BindUniforms(const Shader& shader) const;
        // Pure virtual function: Every shape must implement its own geometry generation
        [[nodiscard]] virtual PrimitiveMesh GenerateGeometry() const = 0;
    };

    class PointPrimitive : public Primitive {
    private:
        glm::vec2 m_Position{0.0f,0.0f};

    public:
        PointPrimitive(const glm::vec2& position, const glm::vec4& color = glm::vec4(1.0f))
            : Primitive(PrimitiveType::Point), m_Position(position) {
            m_Color = color;
        }

        [[nodiscard]] static std::shared_ptr<PointPrimitive> Create(const glm::vec2& position) {
            return std::make_shared<PointPrimitive>(position);
        }

        void SetPosition(const glm::vec2& position) { m_Position = position; }

        [[nodiscard]] PrimitiveMesh GenerateGeometry() const override {
            PrimitiveMesh mesh;
            // --- FIXED: Ground the vertex to local origin (0,0) so the model matrix handles translation! ---
            mesh.vertices = { { m_Position, m_Color, {0.0f, 0.0f} } };
            mesh.indices = { 0 };
            return mesh;
        }
    };

    class LinePrimitive : public Primitive {
    private:
        glm::vec2 m_Start;
        glm::vec2 m_End;

    public:
        LinePrimitive(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color = glm::vec4(1.0f))
            : Primitive(PrimitiveType::Line), m_Start(start), m_End(end) {
            m_Color = color;
        }

        [[nodiscard]] static std::shared_ptr<LinePrimitive> Create(const glm::vec2& start, const glm::vec2& end) {
            return std::make_shared<LinePrimitive>(start, end);
        }

        // Clean user APIs
        void SetPoints(const glm::vec2& start, const glm::vec2& end) { m_Start = start; m_End = end; }

        [[nodiscard]] PrimitiveMesh GenerateGeometry() const override {
            PrimitiveMesh mesh;
            mesh.vertices = {
                // Position       Color       Texture Coordinates (Explicitly Zero for Lines)
                { m_Start,       m_Color,    { 0.0f, 0.0f } },
                { m_End,         m_Color,    { 0.0f, 0.0f } }
            };
            mesh.indices = { 0, 1 };
            return mesh;
        }
    };

    class CirclePrimitive : public Primitive {
    private:
        glm::vec2 m_Center;
        float m_Radius;

    public:
        CirclePrimitive(const glm::vec2& center, float radius, const glm::vec4& color = glm::vec4(1.0f))
            : Primitive(PrimitiveType::Circle), m_Center(center), m_Radius(radius) {
                m_IsFilled = false;
                m_BorderThickness = 1.0f; 
                m_OutlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                m_Color = color;
        }

        [[nodiscard]] static std::shared_ptr<CirclePrimitive> Create(const glm::vec2& center, float radius) {
            return std::make_shared<CirclePrimitive>(center, radius);
        }

        [[nodiscard]] static std::shared_ptr<CirclePrimitive> Create(float radius) {
            return std::make_shared<CirclePrimitive>(glm::vec2(0.0f, 0.0f), radius);
        }

        void SetProperties(const glm::vec2& center, float radius) {
            m_Center = center;
            m_Radius = radius;
        }

        [[nodiscard]] PrimitiveMesh GenerateGeometry() const override {
            PrimitiveMesh mesh;

            float r = m_Radius;

            // --- FIXED: PERFECT 1:1 SYMMETRIC BOUNDS MAP KEYS ---
            Vertex topLeft     = { { m_Center.x - r, m_Center.y - r }, m_Color, { 0.0f, 0.0f } };
            Vertex topRight    = { { m_Center.x + r, m_Center.y - r }, m_Color, { 1.0f, 0.0f } };
            Vertex bottomRight = { { m_Center.x + r, m_Center.y + r }, m_Color, { 1.0f, 1.0f } };
            Vertex bottomLeft  = { { m_Center.x - r, m_Center.y + r }, m_Color, { 0.0f, 1.0f } };

            mesh.vertices = { topLeft, topRight, bottomRight, bottomLeft };
            mesh.indices = { 0, 1, 2, 2, 3, 0 };

            return mesh;
        }
        void BindUniforms(const Shader& shader) const override;       
    };
    class RectanglePrimitive : public Primitive {
    private:
        glm::vec2 m_Size;
        float m_CornerRadius;

    public:
        // Position is handled by the parent UiObject via Move(), so we build local geometry around (0,0)
        RectanglePrimitive(const glm::vec2& size, float cornerRadius = 0.0f, const glm::vec4& color = glm::vec4(1.0f))
            : Primitive(PrimitiveType::Rectangle), m_Size(size), m_CornerRadius(cornerRadius) {
            m_Color = color;
        }

        [[nodiscard]] static std::shared_ptr<RectanglePrimitive> Create(const glm::vec2& size, float radius) {
            return std::make_shared<RectanglePrimitive>(size, radius);
        }

        void SetSize(const glm::vec2& size) { m_Size = size; }
        void SetCornerRadius(float radius) { m_CornerRadius = radius; }

        [[nodiscard]] PrimitiveMesh GenerateGeometry() const override {
            PrimitiveMesh mesh;

            float halfW = m_Size.x * 0.5f;
            float halfH = m_Size.y * 0.5f;

            // --- FIXED: Perfect 1:1 Symmetric Mappings ---
            mesh.vertices = {
                // Position            Color       Texture Coordinates 
                { { -halfW, -halfH }, m_Color,    { 0.0f, 0.0f } }, // 0: Top-Left
                { {  halfW, -halfH }, m_Color,    { 1.0f, 0.0f } }, // 1: Top-Right
                { {  halfW,  halfH }, m_Color,    { 1.0f, 1.0f } }, // 2: Bottom-Right
                { { -halfW,  halfH }, m_Color,    { 0.0f, 1.0f } }  // 3: Bottom-Left
            };

            mesh.indices = {
                0, 1, 2, 
                2, 3, 0  
            };
            return mesh;
        }

        void BindUniforms(const Shader& shader) const override;

        // Getter for the specialized shader logic to extract corner properties
        [[nodiscard]] float GetCornerRadius() const { return m_CornerRadius; }
        [[nodiscard]] glm::vec2 GetSize() const { return m_Size; }
    };

    class ThickLinePrimitive : public Primitive {
        private:
            float m_Length;
            float m_Thickness;

            // --- NEW STAGE TRACKING FOR POSITION-DRIVEN EDGES ---
            // If constructed via specific points, we calculate an angle and offset offset!
            bool      m_UseExplicitPoints{ false };
            glm::vec2 m_StartPoint{ 0.0f, 0.0f };
            glm::vec2 m_EndPoint{ 0.0f, 0.0f };

        public:
            // Retain your clean default length constructor for the sweeping clock needle!
            ThickLinePrimitive(float length, float thickness)
                : Primitive(PrimitiveType::Rectangle), m_Length(length), m_Thickness(thickness), m_UseExplicitPoints(false) {}

            // --- NEW OVERLOADED CONSTRUCTOR FOR TRIANGLE EDGES ---
            ThickLinePrimitive(const glm::vec2& start, const glm::vec2& end, float thickness)
                : Primitive(PrimitiveType::Rectangle), m_Thickness(thickness), m_UseExplicitPoints(true), m_StartPoint(start), m_EndPoint(end) 
            {
                // Compute the absolute length of this line thread bar element
                m_Length = glm::length(end - start);
            }

            [[nodiscard]] static std::shared_ptr<ThickLinePrimitive> Create(float length, float thickness) {
                return std::make_shared<ThickLinePrimitive>(length, thickness);
            }

            [[nodiscard]] static std::shared_ptr<ThickLinePrimitive> Create(const glm::vec2& start, const glm::vec2& end, float thickness) {
                return std::make_shared<ThickLinePrimitive>(start, end, thickness);
            }

            [[nodiscard]] PrimitiveMesh GenerateGeometry() const override {
                PrimitiveMesh mesh;
                float w = (m_Thickness + (m_BorderThickness * 2.0f)) * 0.5f;

                // CONDITION A: If explicitly using custom edge positions (Triangle Borders)
                if (m_UseExplicitPoints) {
                    glm::vec2 direction = m_EndPoint - m_StartPoint;
                    if (m_Length < 0.001f) return mesh;
                    
                    glm::vec2 normalizedDir = direction / m_Length;
                    glm::vec2 perpendicular(-normalizedDir.y, normalizedDir.x);
                    glm::vec2 halfWidthOffset = perpendicular * w;

                    Vertex topLeft     = { m_StartPoint + halfWidthOffset, m_Color, { 0.0f, 0.0f } };
                    Vertex bottomLeft  = { m_StartPoint - halfWidthOffset, m_Color, { 0.0f, 1.0f } };
                    Vertex bottomRight = { m_EndPoint   - halfWidthOffset, m_Color, { 1.0f, 1.0f } };
                    Vertex topRight    = { m_EndPoint   + halfWidthOffset, m_Color, { 1.0f, 0.0f } };

                    mesh.vertices = { topLeft, bottomLeft, bottomRight, topRight };
                    mesh.indices  = { 0, 1, 2, 2, 3, 0 };
                } 
                // CONDITION B: If using your standard axis-aligned local needle space (Clock Indicator)
                else {
                    float h = m_Length + (m_BorderThickness * 2.0f);
                    Vertex topLeft     = { { -w, -h }, m_Color, { 0.0f, 0.0f } };
                    Vertex topRight    = { {  w, -h }, m_Color, { 1.0f, 0.0f } };
                    Vertex bottomRight = { {  w, 0.0f }, m_Color, { 1.0f, 1.0f } };
                    Vertex bottomLeft  = { { -w, 0.0f }, m_Color, { 0.0f, 1.0f } };

                    mesh.vertices = { topLeft, topRight, bottomRight, bottomLeft };
                    mesh.indices  = { 0, 1, 2, 2, 3, 0 };
                }

                return mesh;
            }

            void BindUniforms(const Shader& shader) const override; 
        };

    class TrianglePrimitive : public Primitive {
    private:
        glm::vec2 m_P1;
        glm::vec2 m_P2;
        glm::vec2 m_P3;

    public:
        TrianglePrimitive(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3)
            : Primitive(PrimitiveType::Rectangle), m_P1(p1), m_P2(p2), m_P3(p3) {}

        [[nodiscard]] static std::shared_ptr<TrianglePrimitive> Create(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
            return std::make_shared<TrianglePrimitive>(p1, p2, p3);
        }

        PrimitiveMesh GenerateGeometry() const override {
            PrimitiveMesh quadShellMesh;

            float minX = glm::min(glm::min(m_P1.x, m_P2.x), m_P3.x);
            float maxX = glm::max(glm::max(m_P1.x, m_P2.x), m_P3.x);
            float minY = glm::min(glm::min(m_P1.y, m_P2.y), m_P3.y);
            float maxY = glm::max(glm::max(m_P1.y, m_P2.y), m_P3.y);

            // Add a generous 15px buffer zone around the tip coordinates
            float halfW = ((maxX - minX) * 0.5f) + 15.0f;
            float halfH = ((maxY - minY) * 0.5f) + 15.0f;

            float centerX = (minX + maxX) * 0.5f;
            float centerY = (minY + maxY) * 0.5f;

            // --- FIXED: FORCING ALL FOUR CORNERS TO PURE ZERO ALPHA TRANSPARENCY ---
            // Passing glm::vec4(0.0f) ensures the quad background canvas has absolutely 
            // no baseline color tint, preventing any transparent ghost rectangles!
            glm::vec4 transparentZero(0.0f);

            Vertex topLeft     = { { centerX - halfW, centerY - halfH }, transparentZero, { 0.0f, 0.0f } };
            Vertex topRight    = { { centerX + halfW, centerY - halfH }, transparentZero, { 1.0f, 0.0f } };
            Vertex bottomRight = { { centerX + halfW, centerY + halfH }, transparentZero, { 1.0f, 1.0f } };
            Vertex bottomLeft  = { { centerX - halfW, centerY + halfH }, transparentZero, { 0.0f, 1.0f } };

            quadShellMesh.vertices = { topLeft, topRight, bottomRight, bottomLeft };
            quadShellMesh.indices  = { 0, 1, 2, 2, 3, 0 }; 

            return quadShellMesh;
        }

        void BindUniforms(const Shader& shader) const override {
            shader.SetUniformInt("u_PrimitiveType", 6); 
            shader.SetUniformFloat("u_BorderThickness", m_BorderThickness);
            shader.SetUniformInt("u_IsFilled", m_IsFilled ? 1 : 0);
            
            // --- FIXED: PASS EXTRACTED BODY COLOR VIA A DEDICATED REGISTRATION UNIFORM ---
            // Since the vertices are zeroed out, we pass the hand's solid color down as a uniform!
            shader.SetUniformVec4("u_OutlineColor", m_OutlineColor);
            shader.SetUniformVec4("u_TriangleBodyColor", m_Color); // ◄── NEW UNIFORM FOR HAND FILL COLOR

            shader.SetUniformVec2("u_TriangleP1", m_P1);
            shader.SetUniformVec2("u_TriangleP2", m_P2);
            shader.SetUniformVec2("u_TriangleP3", m_P3);
        }
        
        void DrawBorders(BatchRenderer&, const glm::mat4&) const {}
    };
} // namespace Gui
