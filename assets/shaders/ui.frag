#version 330 core
in vec4 v_Color; 
in vec2 v_TexCoord;

out vec4 FragColor;

uniform int       u_PrimitiveType; 
uniform vec2      u_RectSize;
uniform float     u_CornerRadius;
uniform int       u_IsFilled;
uniform vec4      u_OutlineColor;
uniform float     u_BorderThickness;
uniform sampler2D u_TextureAtlas;
uniform int       u_TextureMaskType;
uniform int       u_IsMaskContainer;

float SDFRoundedRect(vec2 p, vec2 size, float radius) {
    vec2 q = abs(p) - (size * 0.5) + vec2(radius);
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

void main() {
    // ====================================================================
    // --- FIXED: INTERCEPT MASKS AT THE VERY TOP OF THE MAIN FUNCTION ---
    // ====================================================================
    // This allows the mask engine to run BEFORE any type branch hits an early return!
    if (u_IsMaskContainer == 1) {
        vec2 uv = v_TexCoord - vec2(0.5);
        
        // For a circular mask frame, discard fragments outside the inner radius bounds
        if (length(uv) > 0.5) {
            discard;
        }
    }

    // --- BRANCH 0: DIRECT VERTEX COLORING ---
    if (u_PrimitiveType == 0) {
        FragColor = v_Color;
        return;
    }

    // --- BRANCH 1: PROCEDURAL RADAR CIRCLES ---
    else if (u_PrimitiveType == 1) {
        vec2 localCoord = v_TexCoord * 2.0 - 1.0;
        float distSquared = dot(localCoord, localCoord);

        if (distSquared > 1.0) discard;

        float uvPixelSize = fwidth(length(localCoord)); 
        float pixelDistToEdge = (1.0 - length(localCoord)) / uvPixelSize;

        if (u_BorderThickness > 0.0 && pixelDistToEdge < (u_BorderThickness * 0.5)) {
            FragColor = u_OutlineColor;
        } else {
            if (u_IsFilled == 0) discard;
            FragColor = v_Color;
        }
        return;
    }

    // --- BRANCH 2: SOLID ROUNDED RECTANGLE PLATES ---
    else if (u_PrimitiveType == 2) {
        vec2 localPos = (v_TexCoord - vec2(0.5)) * u_RectSize;
        float dist = SDFRoundedRect(localPos, u_RectSize, u_CornerRadius);

        if (dist > 0.0) discard;

        if (u_BorderThickness > 0.0 && dist > -u_BorderThickness) {
            FragColor = u_OutlineColor;
        } else {
            FragColor = v_Color; 
        }
        return;
    }

    // --- BRANCH 3: TEXT TYPOGRAPHY CHARACTERS ---
    else if (u_PrimitiveType == 3) {
        float sampled = texture(u_TextureAtlas, v_TexCoord).r;
        FragColor = vec4(v_Color.rgb, v_Color.a * sampled);
        return;
    }

    // --- BRANCH 4: TEXTURE IMAGES MASKING ---
    else if (u_PrimitiveType == 4) {
        FragColor = texture(u_TextureAtlas, v_TexCoord) * v_Color;
        return;
    }
}