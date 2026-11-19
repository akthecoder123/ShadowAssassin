#pragma once

#include "raylib.h"
#include <string>

// Forward declarations
enum class GuardState;

/**
 * @brief CharacterRenderer encapsulates sprite textures, frame slicing,
 *        and top-down 2D rendering for the player assassin and enemy guards.
 * 
 * Demonstrates:
 * - Single Responsibility Principle (SRP): Decouples texture management,
 *   animation frame mapping, and character visual effects from entity logic.
 * - Encapsulation & RAII: Manages Raylib Texture2D GPU resources with safe
 *   initialization and deterministic cleanup.
 * - Headless Safety: Gracefully falls back to procedural drawing if IsWindowReady()
 *   is false (enabling automated headless testing).
 */
class CharacterRenderer {
public:
    CharacterRenderer() = delete; // Static utility / manager

    // Lifecycle Management
    static void init();
    static void shutdown();
    static bool isLoaded() { return s_loaded; }

    // Player Drawing
    static void drawPlayer(
        Vector2 center,
        float radius,
        float facingAngle,
        bool isMoving,
        bool isAlive,
        float animDistance,
        float animTimer,
        bool isHitFlashing,
        float assassinationTimer,
        Vector2 lastAssassinationPos
    );

    // Guard Drawing
    static void drawGuard(
        Vector2 center,
        float radius,
        float facingAngle,
        GuardState state,
        const char* stateName,
        float animDistance,
        float animTimer,
        bool isVulnerable,
        float attackSlashTimer,
        float visionRadius,
        float visionAngle
    );

    // Visual Overlays & Effects
    static void drawGroundShadow(Vector2 center, float radius, float stretch = 1.0f);
    static void drawBloodSplatter(Vector2 center, float radius);
    static void drawAssassinationSlash(Vector2 center, float facingAngle, float timer, float maxDuration = 0.35f);
    static void drawAlertBadge(Vector2 center, float radius, float pulseTimer);
    static void drawVulnerabilityReticle(Vector2 center, float radius);

private:
    static Texture2D s_playerTexture;
    static Texture2D s_guardTexture;
    static Texture2D s_shadowTexture;
    static Texture2D s_bloodTexture;
    static bool s_loaded;

    static std::string resolveAssetPath(const std::string& relativePath);
};
