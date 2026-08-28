#version 330 core
in vec4 v_Color; 
in vec2 v_TexCoord;

// ====================================================================
// --- NEW INPUT VARYING: SCREEN INTERPOLATED COORDINATES -------------
// ====================================================================
// This reads the raw pixel position matrix directly from your vertex program, 
// allowing flawless node-tree clipping independent of local shape UV masks!
in vec2 v_WorldPosition; 

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

// ====================================================================
// --- NEW UNIFORMS: SIGNED DISTANCE FIELD SHADER CLIPPING CONTROLS --
// ====================================================================
uniform int       u_UseSdfClip;       // 1 = Active, 0 = Bypassed
uniform vec2      u_ClipCenter;       // Center point of your clock widget (X, Y)
uniform vec2      u_ClipHalfSize;     // Half-Width and Half-Height box constraints
uniform float     u_ClipCornerRadius; // Corner curve radius (e.g., 25.0f chassis frame)

float SDFRoundedRect(vec2 p, vec2 size, float radius) {
    vec2 q = abs(p) - (size * 0.5) + vec2(radius);
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

void main() {
    // ====================================================================
    // --- STRATEGY A: MODERN HIGH-TECH SHADER CLIPPING SYSTEM -----------
    // ====================================================================
    // If the active scene layout node belongs to an SdfClipComponent branch,
    // we evaluate its world coordinates against your parent boundaries instantly!
    if (u_UseSdfClip == 1) {
        vec2 localClipPos = v_WorldPosition - u_ClipCenter;
        float clipDist = SDFRoundedRect(localClipPos, u_ClipHalfSize * 2.0, u_ClipCornerRadius);
        
        // PURE MATH CLIP: If pixel is outside the rounded bounding frame container, drop it!
        if (clipDist > 0.0) {
            discard;
        }

        // OPTIONAL ULTRA-SMOOTH ANTI-ALIASED EDGE BLENDING:
        // float edgeAlpha = 1.0 - smoothstep(-0.5, 0.5, clipDist);
        // if (edgeAlpha <= 0.0) discard;
    }

    // ====================================================================
    // --- STRATEGY B: YOUR ORIGINAL LOCAL MASK LAYER INTERCEPT ----------
    // ====================================================================
    if (u_IsMaskContainer == 1) {
        vec2 uv = v_TexCoord - vec2(0.5);
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