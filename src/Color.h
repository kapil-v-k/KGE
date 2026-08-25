#pragma once
#include <glm/glm.hpp>

namespace Gui {

    struct Color {
        // ====================================================================
        // 1. STANDARD CORE CRITERIA
        // ====================================================================
        static constexpr glm::vec4 White         { 1.0f, 1.0f, 1.0f, 1.0f };
        static constexpr glm::vec4 Black         { 0.0f, 0.0f, 0.0f, 1.0f };
        static constexpr glm::vec4 Red           { 1.0f, 0.0f, 0.0f, 1.0f };
        static constexpr glm::vec4 Green         { 0.0f, 1.0f, 0.0f, 1.0f };
        static constexpr glm::vec4 Blue          { 0.0f, 0.0f, 1.0f, 1.0f };
        static constexpr glm::vec4 Cyan          { 0.0f, 1.0f, 1.0f, 1.0f };
        static constexpr glm::vec4 Yellow        { 1.0f, 1.0f, 0.0f, 1.0f };
        static constexpr glm::vec4 Magenta       { 1.0f, 0.0f, 1.0f, 1.0f };

        // ====================================================================
        // 2. INDUSTRIAL GREY SHADES (From Deep Obsidian to Light Silver)
        // ====================================================================
        static constexpr glm::vec4 PureDark      { 0.05f, 0.05f, 0.06f, 1.0f }; // Abyss
        static constexpr glm::vec4 MidnightGrey  { 0.09f, 0.09f, 0.11f, 1.0f }; // Deepest framework backdrop
        static constexpr glm::vec4 ObsidianGrey  { 0.12f, 0.12f, 0.15f, 1.0f }; // Master card container panel [8]
        static constexpr glm::vec4 CharcoalGrey  { 0.15f, 0.15f, 0.18f, 1.0f }; // Muted widgets base
        static constexpr glm::vec4 CarbonGrey    { 0.17f, 0.17f, 0.20f, 1.0f }; // Secondary layout components [8]
        static constexpr glm::vec4 Graphite_600  { 0.20f, 0.20f, 0.23f, 1.0f }; // Secondary utility steps
        static constexpr glm::vec4 BorderGrey    { 0.30f, 0.30f, 0.35f, 1.0f }; // Subtle unselected framing wires [8]
        static constexpr glm::vec4 Slate_500     { 0.38f, 0.44f, 0.52f, 1.0f }; // Cool industrial mid-grey [8]
        static constexpr glm::vec4 Steel_400     { 0.58f, 0.64f, 0.71f, 1.0f }; // Bright unselected handles
        static constexpr glm::vec4 Silver_300    { 0.75f, 0.75f, 0.78f, 1.0f }; // Metallic wireframe accents
        static constexpr glm::vec4 Platinum_100  { 0.88f, 0.88f, 0.90f, 1.0f }; // High-contrast sub-labels

        // ====================================================================
        // 3. METRIC WHITE SHADES (Eliminates Blearing on Dark Displays)
        // ====================================================================
        static constexpr glm::vec4 GhostWhite    { 0.97f, 0.97f, 1.00f, 1.0f }; // Standard crisp readout font
        static constexpr glm::vec4 SnowWhite     { 0.96f, 0.96f, 0.96f, 1.0f }; // Solid text flat accent
        static constexpr glm::vec4 IvoryWhite    { 0.93f, 0.93f, 0.88f, 1.0f }; // Vintage warmer text readout
        static constexpr glm::vec4 BoneWhite     { 0.89f, 0.89f, 0.86f, 1.0f }; // Soft non-glare dashboard data

        // ====================================================================
        // 4. THE COMMAND BLUE SHADES (Deep Space to High-Speed Electrical Blue)
        // ====================================================================
        static constexpr glm::vec4 DeepSpaceNavy { 0.04f, 0.08f, 0.18f, 1.0f }; // Command center background [8]
        static constexpr glm::vec4 AbyssalNavy   { 0.01f, 0.03f, 0.10f, 1.0f }; // Bottom structural fill backdrop
        static constexpr glm::vec4 SubmarineTeal { 0.00f, 0.35f, 0.40f, 1.0f }; // Classic analog tactical radar [8]
        static constexpr glm::vec4 OceanBlue     { 0.07f, 0.34f, 0.72f, 1.0f }; // High-contrast structural tabs
        static constexpr glm::vec4 Sky_500       { 0.05f, 0.65f, 0.93f, 1.0f }; // Crisp modern layout accent [8]
        static constexpr glm::vec4 NeonCyan      { 0.00f, 0.80f, 1.00f, 1.0f }; // System telemetry lines [8]
        static constexpr glm::vec4 IceBlue       { 0.65f, 0.90f, 1.00f, 1.0f }; // Sub-zero vector tracking lines

        // ====================================================================
        // 5. CAUTION ORANGE SHADES (Alert Status Matrices)
        // ====================================================================
        static constexpr glm::vec4 DarkAmber     { 0.50f, 0.20f, 0.00f, 1.0f }; // Background caution matrix fill
        static constexpr glm::vec4 RustOrange    { 0.72f, 0.26f, 0.04f, 1.0f }; // Secondary alert containers
        static constexpr glm::vec4 WarningOrange { 1.00f, 0.40f, 0.00f, 1.0f }; // Standard caution warning nodes [8]
        static constexpr glm::vec4 SafetyOrange  { 1.00f, 0.52f, 0.10f, 1.0f }; // Active perimeter scan blocks
        static constexpr glm::vec4 CyberOrange   { 1.00f, 0.65f, 0.00f, 1.0f }; // High-intensity pointer widgets
        static constexpr glm::vec4 PeachOrange   { 1.00f, 0.75f, 0.55f, 1.0f }; // Secondary auxiliary data text

        // ====================================================================
        // 6. SCI-FI YELLOW SHADES (Auxiliary Tracking & Target Locks)
        // ====================================================================
        static constexpr glm::vec4 MustardGold   { 0.65f, 0.50f, 0.05f, 1.0f }; // Ground grid auxiliary maps
        static constexpr glm::vec4 GoldAccent    { 0.85f, 0.68f, 0.25f, 1.0f }; // Premium console bezel markings [8]
        static constexpr glm::vec4 PlasmaYellow  { 1.00f, 0.85f, 0.00f, 1.0f }; // Active targeted locked vectors [8]
        static constexpr glm::vec4 LaserYellow   { 1.00f, 0.95f, 0.10f, 1.0f }; // Ultra-bright lock trackers
        static constexpr glm::vec4 NeonYellow    { 0.92f, 1.00f, 0.00f, 1.0f }; // High-visibility scope threads

        // ====================================================================
        // 7. MISC STANDARD ACCENTS (Tailwind / Modern Framework Elements)
        // ====================================================================
        static constexpr glm::vec4 Zinc_700      { 0.24f, 0.24f, 0.27f, 1.0f }; // Neutral layout blocks [8]
        static constexpr glm::vec4 Emerald_500   { 0.06f, 0.78f, 0.48f, 1.0f }; // Modern dashboard online state [8]
        static constexpr glm::vec4 Indigo_500    { 0.38f, 0.36f, 0.93f, 1.0f }; // Primary component accents [8]
        static constexpr glm::vec4 Violet_600    { 0.49f, 0.18f, 0.88f, 1.0f }; // Secondary panel toggles [8]
        static constexpr glm::vec4 Rose_500      { 0.95f, 0.18f, 0.36f, 1.0f }; // Soft errors / structural deletes [8]
        static constexpr glm::vec4 Amber_500     { 0.96f, 0.59f, 0.04f, 1.0f }; // Base yellow-orange state [8]
        static constexpr glm::vec4 NeonGreen     { 0.00f, 1.00f, 0.60f, 1.0f }; // Medical tracking sweep lines [8]
        static constexpr glm::vec4 NeonPurple    { 0.74f, 0.00f, 1.00f, 1.0f }; // Anomalous reading grids [8]
        static constexpr glm::vec4 LaserRed      { 1.00f, 0.05f, 0.20f, 1.0f }; // Critical layout exceptions [8]
        static constexpr glm::vec4 ElectricPink  { 1.00f, 0.00f, 0.50f, 1.0f }; // High-voltage signal tracking [8]
        static constexpr glm::vec4 ToxicLime      { 0.65f, 1.00f, 0.00f, 1.0f }; // Night vision display nodes [8]
        static constexpr glm::vec4 CrimsonBlood   { 0.60f, 0.00f, 0.05f, 1.0f }; // Core compromise hull signals [8]

        // ====================================================================
        // 8. GLASSMORPHISM TRANSLUCENT OVERLAYS (Pre-Baked Mixers)
        // ====================================================================
        static constexpr glm::vec4 TranslucentCyan  { 0.00f, 0.80f, 1.00f, 0.25f }; // Active scan circle fills [8]
        static constexpr glm::vec4 TranslucentGreen { 0.00f, 1.00f, 0.60f, 0.30f }; // 30% alpha tracking fills [8]
        static constexpr glm::vec4 GlassBlack       { 0.00f, 0.00f, 0.00f, 0.50f }; // UI modal shadow panels [8]
        static constexpr glm::vec4 FrostWhite       { 1.00f, 1.00f, 1.00f, 0.15f }; // Subtle glint overlay cards [8]

        // ====================================================================
        // 9. DYNAMIC ALPHA MUTATOR DEFINITIONS
        // ====================================================================
        [[nodiscard]] static constexpr glm::vec4 WithAlpha(const glm::vec4& baseColor, float alpha) {
            return glm::vec4(baseColor.r, baseColor.g, baseColor.b, alpha); // Returns perfectly re-mapped vec4 structure [8]
        }
    };

} // namespace Gui