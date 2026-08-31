#version 330 core
in vec4 v_Color; 
in vec2 v_TexCoord;
in vec2 v_WorldPosition; 
in vec2 v_LocalPosition; 

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

// SDF Clipping Uniform Registers
uniform int       u_UseSdfClip;       
uniform mat4      u_InverseClipMatrix; 
uniform vec2      u_ClipHalfSize;     
uniform float     u_ClipCornerRadius; 

// Triangle Coordinate Uniforms
uniform vec2      u_TriangleP1;
uniform vec2      u_TriangleP2;
uniform vec2      u_TriangleP3;
uniform vec4      u_TriangleBodyColor;

// Mathematical SDF Evaluator for Rounded Rectangles
float SDFRoundedRect(vec2 p, vec2 size, float radius) {
    vec2 q = abs(p) - (size * 0.5) + vec2(radius);
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

// ====================================================================
// --- FIXED: HIGH-PRECISION BARYCENTRIC SIDE-DISTANCE FILTER ---------
// ====================================================================
// This evaluates absolute pixel distances to each of the three flat sides 
// and enforces a unified winding-immune edge sign.
float SDFArbitraryTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2) {
    vec2 e0 = v1 - v0; vec2 w0 = p - v0;
    vec2 e1 = v2 - v1; vec2 w1 = p - v1;
    vec2 e2 = v0 - v2; vec2 w2 = p - v2;

    float d0 = dot(w0 - e0 * clamp(dot(w0, e0) / dot(e0, e0), 0.0, 1.0), w0 - e0 * clamp(dot(w0, e0) / dot(e0, e0), 0.0, 1.0));
    float d1 = dot(w1 - e1 * clamp(dot(w1, e1) / dot(e1, e1), 0.0, 1.0), w1 - e1 * clamp(dot(w1, e1) / dot(e1, e1), 0.0, 1.0));
    float d2 = dot(w2 - e2 * clamp(dot(w2, e2) / dot(e2, e2), 0.0, 1.0), w2 - e2 * clamp(dot(w2, e2) / dot(e2, e2), 0.0, 1.0));

    // Determine inside/outside orientation via clean edge cross products
    float c0 = e0.x * w0.y - e0.y * w0.x;
    float c1 = e1.x * w1.y - e1.y * w1.x;
    float c2 = e2.x * w2.y - e2.y * w2.x;
    
    bool inside = (c0 >= 0.0 && c1 >= 0.0 && c2 >= 0.0) || (c0 <= 0.0 && c1 <= 0.0 && c2 <= 0.0);
    float s = inside ? -1.0 : 1.0;
    
    return sqrt(min(min(d0, d1), d2)) * s;
}

void main() {
    // ====================================================================
    // --- STRATEGY A: PRODUCTION-GRADE MATRIX-INVERTED SHADER CLIPPING ---
    // ====================================================================
    if (u_UseSdfClip == 1) {
        vec4 localSpacePos = u_InverseClipMatrix * vec4(v_WorldPosition, 0.0, 1.0);
        float clipDist = SDFRoundedRect(localSpacePos.xy, u_ClipHalfSize * 2.0, u_ClipCornerRadius);
        
        float clipPixelWidth = fwidth(clipDist);
        float clipAlpha = 1.0 - smoothstep(-clipPixelWidth, clipPixelWidth, clipDist);
        if (clipAlpha <= 0.0) discard;
    }

    // ====================================================================
    // --- STRATEGY B: ORIGINAL LOCAL MASK LAYER INTERCEPT ----------------
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

    // --- BRANCH 1: ANTI-ALIASED PROCEDURAL RADAR CIRCLES ---
    else if (u_PrimitiveType == 1) {
        vec2 localCoord = v_TexCoord * 2.0 - 1.0;
        float distFromCenter = length(localCoord);
        float edgeDist = distFromCenter - 1.0;

        float pixelWidth = fwidth(edgeDist);
        float outerAlpha = 1.0 - smoothstep(-pixelWidth, pixelWidth, edgeDist);
        if (outerAlpha <= 0.0) discard;

        if (u_BorderThickness > 0.0) {
            float innerDist = edgeDist + (u_BorderThickness * 0.01); 
            float innerAlpha = smoothstep(-pixelWidth, pixelWidth, innerDist);

            vec4 interiorColor = (u_IsFilled == 1) ? v_Color : vec4(v_Color.rgb, 0.0);
            vec4 finalStroke = mix(interiorColor, u_OutlineColor, innerAlpha);
            
            FragColor = vec4(finalStroke.rgb, finalStroke.a * outerAlpha);
        } else {
            if (u_IsFilled == 0) discard;
            FragColor = vec4(v_Color.rgb, v_Color.a * outerAlpha);
        }
        return;
    }

    // --- BRANCH 2: ANTI-ALIASED ROUNDED RECTANGLE PLATES ---
    else if (u_PrimitiveType == 2) {
        vec2 localPos = (v_TexCoord - vec2(0.5)) * u_RectSize;
        float dist = SDFRoundedRect(localPos, u_RectSize, u_CornerRadius);

        float pixelWidth = fwidth(dist);
        float outerAlpha = 1.0 - smoothstep(-pixelWidth, pixelWidth, dist);
        if (outerAlpha <= 0.0) discard;

        if (u_BorderThickness > 0.0) {
            float innerDist = dist + u_BorderThickness;
            float innerAlpha = smoothstep(-pixelWidth, pixelWidth, innerDist);

            vec4 interiorColor = (u_IsFilled == 1) ? v_Color : vec4(v_Color.rgb, 0.0);
            vec4 finalStroke = mix(interiorColor, u_OutlineColor, innerAlpha);
            
            FragColor = vec4(finalStroke.rgb, finalStroke.a * outerAlpha);
        } else {
            if (u_IsFilled == 0) discard;
            FragColor = vec4(v_Color.rgb, v_Color.a * outerAlpha);
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

    // --- BRANCH 5: ANTI-ALIASED SMOOTH ROUND POINTS ---
    else if (u_PrimitiveType == 5) {
        vec2 ptCoord = v_TexCoord - vec2(0.5);
        float dist = length(ptCoord) - 0.5;
        
        float pixelWidth = fwidth(dist);
        float alpha = 1.0 - smoothstep(-pixelWidth, pixelWidth, dist);
        
        if (alpha <= 0.0) discard;
        FragColor = vec4(v_Color.rgb, v_Color.a * alpha);
        return;
    }

    // ====================================================================
    // --- BRANCH 6: PERFECTLY ANTI-ALIASED QUAD-BOUNDED TRIANGLES -------
    // ====================================================================
    else if (u_PrimitiveType == 6) {
        vec2 localPos = v_LocalPosition; 

        // Compute the precise side alignment vector distance
        float dist = SDFArbitraryTriangle(localPos, u_TriangleP1, u_TriangleP2, u_TriangleP3);
        float pixelWidth = fwidth(dist);

        // ====================================================================
        // --- FIXED: BULLETPROOF EXPLICIT DISCARD DESTROYS GHOST OUTLINES ----
        // ====================================================================
        // Because the barycentric sign tracking is completely stable under rotations, 
        // calling discard here guarantees absolute transparency outside the triangle lines!
        if (dist > 0.0) {
            discard;
        }

        // Apply a smooth step inside the interior line threshold for sub-pixel anti-aliasing
        float outerAlpha = 1.0 - smoothstep(-pixelWidth, 0.0, dist);
        vec4 baseTriangleColor = u_TriangleBodyColor;

        if (u_BorderThickness > 0.0) {
            float innerDist = dist + u_BorderThickness;
            float innerAlpha = smoothstep(-pixelWidth, pixelWidth, innerDist);

            vec4 interiorColor = (u_IsFilled == 1) ? baseTriangleColor : vec4(baseTriangleColor.rgb, 0.0);
            vec4 finalStroke = mix(interiorColor, u_OutlineColor, innerAlpha);
            
            FragColor = vec4(finalStroke.rgb, finalStroke.a * outerAlpha);
        } else {
            if (u_IsFilled == 0) discard;
            FragColor = vec4(baseTriangleColor.rgb, baseTriangleColor.a * outerAlpha);
        }
        return;
    }
}