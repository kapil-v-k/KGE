#version 330 core
layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;

uniform mat4 u_Projection; 
uniform mat4 u_Model; // --- ADDED MODEL MATRIX FOR HARDWARE MOVEMENT ---

void main() {
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    
    // Matrix math reads right-to-left: Projection * Model * Position Vector
    gl_Position = u_Projection * u_Model * vec4(a_Position, 0.0, 1.0);
}