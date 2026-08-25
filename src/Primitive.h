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

            // Helper to calculate the extruded normal of a corner joint
            glm::vec2 GetCornerNormal(const glm::vec2& curr, const glm::vec2& prev, const glm::vec2& next) const {
                glm::vec2 dir1 = glm::normalize(curr - prev);
                glm::vec2 dir2 = glm::normalize(next - curr);
                
                // Compute perpendicular normals for both connected edges
                glm::vec2 n1(-dir1.y, dir1.x);
                glm::vec2 n2(-dir2.y, dir2.x);
                
                // Average them to find the perfect miter-joint heading vector
                glm::vec2 miter = glm::normalize(n1 + n2);
                
                // Scale the length so the border stays uniform along the angled slopes
                float length = m_BorderThickness / glm::dot(miter, n1);
                if (length != length || length > m_BorderThickness * 5.0f) {
                    return n1 * m_BorderThickness; // Fallback for edge cases
                }
                return miter * length;
            }

        public:
            TrianglePrimitive(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3)
                : Primitive(PrimitiveType::Rectangle), m_P1(p1), m_P2(p2), m_P3(p3) {}

            [[nodiscard]] static std::shared_ptr<TrianglePrimitive> Create(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
                return std::make_shared<TrianglePrimitive>(p1, p2, p3);
            }

            [[nodiscard]] PrimitiveMesh GenerateGeometry() const override {
                PrimitiveMesh mesh;

                // 1. GENERATE THE FILLED CORE FACE (If active)
                if (m_IsFilled) {
                    Vertex v1 = { m_P1, m_Color, {0,0} };
                    Vertex v2 = { m_P2, m_Color, {0,0} };
                    Vertex v3 = { m_P3, m_Color, {0,0} };
                    mesh.vertices = { v1, v2, v3 };
                    mesh.indices  = { 0, 1, 2 };
                }

                // 2. --- FIXED: EXTRACTED CORNER STITCHING PASS ---
                // If an outline is requested, we expand the mesh outwards to form seamless mitered borders!
                if (m_BorderThickness > 0.0f) {
                    uint32_t borderStartIdx = static_cast<uint32_t>(mesh.vertices.size());

                    // Calculate the outer pushing vectors for each of the 3 sharp corners
                    glm::vec2 ext1 = m_P1 + GetCornerNormal(m_P1, m_P3, m_P2);
                    glm::vec2 ext2 = m_P2 + GetCornerNormal(m_P2, m_P1, m_P3);
                    glm::vec2 ext3 = m_P3 + GetCornerNormal(m_P3, m_P2, m_P1);

                    // Add the 3 inner core points using your Outline Color variable
                    mesh.vertices.push_back({ m_P1, m_OutlineColor, {0,0} }); // index + 0
                    mesh.vertices.push_back({ m_P2, m_OutlineColor, {0,0} }); // index + 1
                    mesh.vertices.push_back({ m_P3, m_OutlineColor, {0,0} }); // index + 2

                    // Add the 3 matching extruded outer points
                    mesh.vertices.push_back({ ext1, m_OutlineColor, {0,0} }); // index + 3
                    mesh.vertices.push_back({ ext2, m_OutlineColor, {0,0} }); // index + 4
                    mesh.vertices.push_back({ ext3, m_OutlineColor, {0,0} }); // index + 5

                    // Stitch the outer border rings together into 3 seamless rectangular quads
                    uint32_t i0 = borderStartIdx;
                    
                    // Wall 1: P1 to P2
                    mesh.indices.insert(mesh.indices.end(), { i0+0, i0+1, i0+4, i0+4, i0+3, i0+0 });
                    // Wall 2: P2 to P3
                    mesh.indices.insert(mesh.indices.end(), { i0+1, i0+2, i0+5, i0+5, i0+4, i0+1 });
                    // Wall 3: P3 to P1
                    mesh.indices.insert(mesh.indices.end(), { i0+2, i0+0, i0+3, i0+3, i0+5, i0+2 });
                }

                return mesh;
            }

            void BindUniforms(const Shader& shader) const override;
            
            // Prototype stays clean and empty now!
            void DrawBorders(BatchRenderer&, const glm::mat4&) const {}
        };
} // namespace Gui
