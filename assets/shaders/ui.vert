#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;
out vec2 v_WorldPosition;

// ====================================================================
// --- NEW VARYING LAYER: LOCAL OBJECT COORDINATES --------------------
// ====================================================================
// Passing raw layout attributes ensures the clipping math is evaluated 
// inside the clock's space, keeping shapes 100% round and visible!
out vec2 v_LocalPosition;

uniform mat4 u_Projection;
uniform mat4 u_Model;

void main() {
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    
    // Track pure local model coords before any matrix transforms run
    v_LocalPosition = a_Position.xy;
    
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_WorldPosition = worldPos.xy;
    
    gl_Position = u_Projection * worldPos;
}