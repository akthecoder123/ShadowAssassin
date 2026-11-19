#include "CharacterRenderer.h"
#include "Guard.h"
#include <cmath>
#include <iostream>
#include <algorithm>

// Static Texture Storage
Texture2D CharacterRenderer::s_playerTexture = {};
Texture2D CharacterRenderer::s_guardTexture  = {};
Texture2D CharacterRenderer::s_shadowTexture = {};
Texture2D CharacterRenderer::s_bloodTexture  = {};
bool CharacterRenderer::s_loaded = false;

std::string CharacterRenderer::resolveAssetPath(const std::string& relativePath) {
    // 1. Direct path from working directory
    if (FileExists(relativePath.c_str())) {
        return relativePath;
    }
    // 2. Parent directory fallback (if running from build/ subdirectory)
    std::string parentPath = "../" + relativePath;
    if (FileExists(parentPath.c_str())) {
        return parentPath;
    }
    // 3. Application directory
    const char* appDir = GetApplicationDirectory();
    if (appDir) {
        std::string appPath = std::string(appDir) + relativePath;
        if (FileExists(appPath.c_str())) {
            return appPath;
        }
    }
    return relativePath;
}

void CharacterRenderer::init() {
    if (!IsWindowReady()) {
        return; // Headless environment safety
    }

    if (s_loaded) return;

    std::string playerPath = resolveAssetPath("assets/images/player_spritesheet.png");
    std::string guardPath  = resolveAssetPath("assets/images/guard_spritesheet.png");
    std::string shadowPath = resolveAssetPath("assets/images/shadow.png");
    std::string bloodPath  = resolveAssetPath("assets/images/blood_splatter.png");

    if (FileExists(playerPath.c_str())) {
        s_playerTexture = LoadTexture(playerPath.c_str());
        SetTextureFilter(s_playerTexture, TEXTURE_FILTER_BILINEAR);
    }
    if (FileExists(guardPath.c_str())) {
        s_guardTexture = LoadTexture(guardPath.c_str());
        SetTextureFilter(s_guardTexture, TEXTURE_FILTER_BILINEAR);
    }
    if (FileExists(shadowPath.c_str())) {
        s_shadowTexture = LoadTexture(shadowPath.c_str());
        SetTextureFilter(s_shadowTexture, TEXTURE_FILTER_BILINEAR);
    }
    if (FileExists(bloodPath.c_str())) {
        s_bloodTexture = LoadTexture(bloodPath.c_str());
        SetTextureFilter(s_bloodTexture, TEXTURE_FILTER_BILINEAR);
    }

    s_loaded = (s_playerTexture.id > 0 && s_guardTexture.id > 0);
    if (s_loaded) {
        std::cout << "[CharacterRenderer] Successfully loaded top-down character sprite assets." << std::endl;
    } else {
        std::cout << "[CharacterRenderer] Sprite assets not found, using enhanced procedural fallback." << std::endl;
    }
}

void CharacterRenderer::shutdown() {
    if (s_loaded && IsWindowReady()) {
        if (s_playerTexture.id > 0) UnloadTexture(s_playerTexture);
        if (s_guardTexture.id > 0)  UnloadTexture(s_guardTexture);
        if (s_shadowTexture.id > 0) UnloadTexture(s_shadowTexture);
        if (s_bloodTexture.id > 0)  UnloadTexture(s_bloodTexture);
    }
    s_playerTexture = {};
    s_guardTexture  = {};
    s_shadowTexture = {};
    s_bloodTexture  = {};
    s_loaded = false;
}

void CharacterRenderer::drawGroundShadow(Vector2 center, float radius, float stretch) {
    if (s_shadowTexture.id > 0) {
        float shadowW = radius * 2.5f * stretch;
        float shadowH = radius * 1.9f;
        Rectangle dest = { center.x, center.y + 3.0f, shadowW, shadowH };
        Vector2 orig = { shadowW * 0.5f, shadowH * 0.5f };
        DrawTexturePro(s_shadowTexture, Rectangle{ 0, 0, 64, 64 }, dest, orig, 0.0f, WHITE);
    } else {
        DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y + 3), radius, Color{ 0, 0, 0, 80 });
    }
}

void CharacterRenderer::drawBloodSplatter(Vector2 center, float radius) {
    if (s_bloodTexture.id > 0) {
        float bSize = radius * 2.8f;
        Rectangle dest = { center.x + 2.0f, center.y + 1.0f, bSize, bSize };
        Vector2 orig = { bSize * 0.5f, bSize * 0.5f };
        DrawTexturePro(s_bloodTexture, Rectangle{ 0, 0, 64, 64 }, dest, orig, 0.0f, WHITE);
    } else {
        DrawCircleV(center, radius * 1.2f, Color{ 140, 20, 20, 200 });
        DrawCircleV(Vector2{ center.x + 4, center.y - 2 }, radius * 0.6f, Color{ 120, 10, 10, 180 });
    }
}

void CharacterRenderer::drawAssassinationSlash(Vector2 center, float facingAngle, float timer, float maxDuration) {
    float progress = 1.0f - (timer / maxDuration);
    float slashRadius = 24.0f + progress * 24.0f;
    float slashStart = facingAngle - 65.0f + progress * 35.0f;
    float slashEnd = slashStart + 115.0f;

    unsigned char alpha = static_cast<unsigned char>((1.0f - progress) * 255);
    Color outerSlash = Color{ 255, 50, 50, alpha };
    Color innerSlash = Color{ 255, 230, 230, alpha };
    Color cyanSpark  = Color{ 0, 230, 255, static_cast<unsigned char>(alpha * 0.7f) };

    DrawCircleSectorLines(center, slashRadius, slashStart, slashEnd, 20, outerSlash);
    DrawCircleSectorLines(center, slashRadius * 0.88f, slashStart, slashEnd, 20, innerSlash);
    DrawCircleSectorLines(center, slashRadius * 0.75f, slashStart, slashEnd, 20, cyanSpark);

    // Dynamic sparks along the blade arc
    float midAngle = (slashStart + slashEnd) * 0.5f * DEG2RAD;
    Vector2 sparkPos = {
        center.x + std::cos(midAngle) * slashRadius,
        center.y + std::sin(midAngle) * slashRadius
    };
    DrawCircleV(sparkPos, 3.5f * (1.0f - progress), Color{ 255, 255, 255, alpha });
}

void CharacterRenderer::drawAlertBadge(Vector2 center, float radius, float pulseTimer) {
    float pulse = 1.0f + 0.12f * std::sin(pulseTimer * 10.0f);
    int badgeW = static_cast<int>(18 * pulse);
    int badgeH = static_cast<int>(20 * pulse);
    int alertY = static_cast<int>(center.y - radius - 38);

    // Glowing amber background
    DrawRectangle(static_cast<int>(center.x - badgeW * 0.5f), alertY, badgeW, badgeH, Color{ 245, 140, 15, 240 });
    DrawRectangleLines(static_cast<int>(center.x - badgeW * 0.5f), alertY, badgeW, badgeH, RAYWHITE);
    DrawText("!", static_cast<int>(center.x - 3 * pulse), alertY + 2, static_cast<int>(15 * pulse), RAYWHITE);
}

void CharacterRenderer::drawVulnerabilityReticle(Vector2 center, float radius) {
    float bSize = radius + 7.0f;
    Color targetColor = Color{ 255, 35, 35, 235 };
    int bracketLen = 7;

    // Top-Left
    DrawLine(static_cast<int>(center.x - bSize), static_cast<int>(center.y - bSize), static_cast<int>(center.x - bSize + bracketLen), static_cast<int>(center.y - bSize), targetColor);
    DrawLine(static_cast<int>(center.x - bSize), static_cast<int>(center.y - bSize), static_cast<int>(center.x - bSize), static_cast<int>(center.y - bSize + bracketLen), targetColor);
    // Top-Right
    DrawLine(static_cast<int>(center.x + bSize), static_cast<int>(center.y - bSize), static_cast<int>(center.x + bSize - bracketLen), static_cast<int>(center.y - bSize), targetColor);
    DrawLine(static_cast<int>(center.x + bSize), static_cast<int>(center.y - bSize), static_cast<int>(center.x + bSize), static_cast<int>(center.y - bSize + bracketLen), targetColor);
    // Bottom-Left
    DrawLine(static_cast<int>(center.x - bSize), static_cast<int>(center.y + bSize), static_cast<int>(center.x - bSize + bracketLen), static_cast<int>(center.y + bSize), targetColor);
    DrawLine(static_cast<int>(center.x - bSize), static_cast<int>(center.y + bSize), static_cast<int>(center.x - bSize), static_cast<int>(center.y + bSize - bracketLen), targetColor);
    // Bottom-Right
    DrawLine(static_cast<int>(center.x + bSize), static_cast<int>(center.y + bSize), static_cast<int>(center.x + bSize - bracketLen), static_cast<int>(center.y + bSize), targetColor);
    DrawLine(static_cast<int>(center.x + bSize), static_cast<int>(center.y + bSize), static_cast<int>(center.x + bSize), static_cast<int>(center.y + bSize - bracketLen), targetColor);

    // Interactive Prompt
    const char* prompt = "[SPACE] ASSASSINATE";
    int pWidth = MeasureText(prompt, 11);
    DrawRectangle(static_cast<int>(center.x - pWidth * 0.5f - 4), static_cast<int>(center.y - radius - 30), pWidth + 8, 14, Color{ 180, 20, 20, 220 });
    DrawText(prompt, static_cast<int>(center.x - pWidth * 0.5f), static_cast<int>(center.y - radius - 29), 11, RAYWHITE);
}

void CharacterRenderer::drawPlayer(
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
) {
    if (!IsWindowReady()) return;

    // 1. Defeated Operative
    if (!isAlive) {
        drawBloodSplatter(center, radius);
        if (s_loaded) {
            float renderSize = radius * 2.7f;
            Rectangle src = { 64.0f, 128.0f, 64.0f, 64.0f }; // Fallen corpse frame
            Rectangle dst = { center.x, center.y, renderSize, renderSize };
            Vector2 orig = { renderSize * 0.5f, renderSize * 0.5f };
            DrawTexturePro(s_playerTexture, src, dst, orig, facingAngle, WHITE);
        } else {
            DrawCircleV(center, radius, Color{ 120, 30, 30, 150 });
            DrawLineV(Vector2{ center.x - 8, center.y - 8 }, Vector2{ center.x + 8, center.y + 8 }, RED);
            DrawLineV(Vector2{ center.x + 8, center.y - 8 }, Vector2{ center.x - 8, center.y + 8 }, RED);
        }
        return;
    }

    // 2. Ground Shadow
    float stretch = isMoving ? 1.15f : 1.0f;
    drawGroundShadow(center, radius, stretch);

    // 3. Active Assassination Slash Blade Effect
    if (assassinationTimer > 0.0f) {
        drawAssassinationSlash(lastAssassinationPos, facingAngle, assassinationTimer, 0.35f);
    }

    // 4. Select Sprite Frame
    int frameCol = 0;
    int frameRow = 0;

    if (assassinationTimer > 0.0f) {
        // Attack Animation Row
        frameRow = 1;
        float progress = 1.0f - (assassinationTimer / 0.35f);
        frameCol = std::clamp(static_cast<int>(progress * 4.0f), 0, 3);
    } else if (isMoving) {
        // Walk Animation Row (cycles every 14 pixels traveled)
        frameRow = 0;
        frameCol = (static_cast<int>(animDistance / 14.0f)) % 4;
    } else {
        // Idle Animation Row
        frameRow = 0;
        frameCol = 0;
    }

    // 5. Draw Player Character Sprite (with subtle idle breathing scale)
    float breathe = (!isMoving && isAlive) ? (1.0f + 0.025f * std::sin(animTimer * 3.5f)) : 1.0f;
    if (s_loaded) {
        float renderSize = radius * 2.75f * breathe;
        Rectangle src = { frameCol * 64.0f, frameRow * 64.0f, 64.0f, 64.0f };
        Rectangle dst = { center.x, center.y, renderSize, renderSize };
        Vector2 orig = { renderSize * 0.5f, renderSize * 0.5f };
        Color tint = isHitFlashing ? Color{ 255, 110, 110, 255 } : WHITE;

        DrawTexturePro(s_playerTexture, src, dst, orig, facingAngle, tint);
    } else {
        // Procedural Fallback
        Color bodyColor = isHitFlashing ? Color{ 255, 100, 100, 255 } : Color{ 37, 99, 235, 255 }; // Rich Cobalt Blue
        DrawCircleV(center, radius, bodyColor);
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, Color{ 8, 12, 20, 255 }); // Dark outline
        DrawCircleV(center, radius * 0.45f, Color{ 96, 165, 250, 255 }); // Bright blue armor ridge highlight

        float rad = facingAngle * DEG2RAD;
        Vector2 visorEnd = { center.x + std::cos(rad) * (radius + 5.0f), center.y + std::sin(rad) * (radius + 5.0f) };
        DrawLineEx(center, visorEnd, 3.5f, Color{ 0, 240, 255, 255 });
    }
}

void CharacterRenderer::drawGuard(
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
) {
    if (!IsWindowReady()) return;

    // 1. Draw Visible Vision Cone Sector (lying on the floor)
    if (state != GuardState::DEAD) {
        Color coneColor;
        switch (state) {
            case GuardState::PATROLLING: coneColor = Color{ 255, 235, 120, 38 }; break;
            case GuardState::ALERTED:    coneColor = Color{ 255, 160, 40, 80 };  break;
            case GuardState::CHASING:    coneColor = Color{ 255, 50, 50, 90 };   break;
            case GuardState::ATTACKING:  coneColor = Color{ 255, 15, 15, 125 };  break;
            default:                     coneColor = Color{ 200, 200, 200, 30 }; break;
        }

        float halfAngle = visionAngle * 0.5f;
        float startAngle = facingAngle - halfAngle;
        float endAngle   = facingAngle + halfAngle;

        DrawCircleSector(center, visionRadius, startAngle, endAngle, 24, coneColor);
        DrawCircleSectorLines(center, visionRadius, startAngle, endAngle, 24, Color{ coneColor.r, coneColor.g, coneColor.b, 150 });
    }

    // 2. Dead Guard Corpse
    if (state == GuardState::DEAD) {
        drawBloodSplatter(center, radius);
        if (s_loaded) {
            float renderSize = radius * 2.7f;
            Rectangle src = { 192.0f, 128.0f, 64.0f, 64.0f }; // Corpse frame
            Rectangle dst = { center.x, center.y, renderSize, renderSize };
            Vector2 orig = { renderSize * 0.5f, renderSize * 0.5f };
            DrawTexturePro(s_guardTexture, src, dst, orig, facingAngle, WHITE);
        } else {
            DrawCircleV(center, radius, Color{ 80, 80, 85, 180 });
            DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, Color{ 40, 40, 45, 200 });
            DrawLineV(Vector2{ center.x - 6, center.y - 6 }, Vector2{ center.x + 6, center.y + 6 }, Color{ 180, 40, 40, 200 });
            DrawLineV(Vector2{ center.x + 6, center.y - 6 }, Vector2{ center.x - 6, center.y + 6 }, Color{ 180, 40, 40, 200 });
        }
        return;
    }

    // 3. Ground Shadow
    float stretch = (state == GuardState::CHASING) ? 1.25f : 1.0f;
    drawGroundShadow(center, radius, stretch);

    // 4. Attack Strike Visual Feedback
    if (attackSlashTimer > 0.0f) {
        float rad = facingAngle * DEG2RAD;
        Vector2 strikeEnd = {
            center.x + std::cos(rad) * (radius + 20.0f),
            center.y + std::sin(rad) * (radius + 20.0f)
        };
        DrawLineEx(center, strikeEnd, 4.0f, Color{ 255, 255, 255, 230 });
        DrawCircleV(strikeEnd, 5.0f, Color{ 255, 50, 50, 240 });
    }

    // 5. Select Sprite Frame by FSM State
    int frameCol = 0;
    int frameRow = 0;

    if (state == GuardState::ATTACKING) {
        frameRow = 2;
        frameCol = (attackSlashTimer > 0.1f) ? 0 : 1;
    } else if (state == GuardState::ALERTED) {
        frameRow = 1;
        frameCol = 0; // Braced alert stance
    } else if (state == GuardState::CHASING) {
        frameRow = 1;
        // Sprint frames (cols 1, 2, 3) cycled every 10px traveled
        frameCol = 1 + (static_cast<int>(animDistance / 10.0f) % 3);
    } else {
        // PATROLLING
        frameRow = 0;
        frameCol = (static_cast<int>(animDistance / 14.0f)) % 4;
    }

    // 6. Draw Guard Character Sprite
    if (s_loaded) {
        float renderSize = radius * 2.75f;
        Rectangle src = { frameCol * 64.0f, frameRow * 64.0f, 64.0f, 64.0f };
        Rectangle dst = { center.x, center.y, renderSize, renderSize };
        Vector2 orig = { renderSize * 0.5f, renderSize * 0.5f };

        DrawTexturePro(s_guardTexture, src, dst, orig, facingAngle, WHITE);
    } else {
        // Procedural Fallback
        DrawCircleV(center, radius, Color{ 200, 45, 45, 255 });
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, Color{ 80, 10, 10, 255 });
        DrawCircleV(center, radius * 0.45f, Color{ 40, 40, 45, 255 });

        float rad = facingAngle * DEG2RAD;
        Vector2 weaponEnd = { center.x + std::cos(rad) * (radius + 6.0f), center.y + std::sin(rad) * (radius + 6.0f) };
        DrawLineEx(center, weaponEnd, 3.5f, Color{ 240, 80, 80, 255 });
    }

    // 7. State Badge / Indicator
    if (stateName) {
        int fontSize = 10;
        int textWidth = MeasureText(stateName, fontSize);
        Color badgeBg = (state == GuardState::CHASING || state == GuardState::ATTACKING) ? Color{ 50, 10, 10, 220 } : Color{ 15, 15, 20, 200 };
        Color badgeTxt = (state == GuardState::ALERTED) ? Color{ 255, 180, 50, 255 } :
                         (state == GuardState::CHASING) ? Color{ 255, 80, 80, 255 } :
                         (state == GuardState::ATTACKING) ? Color{ 255, 30, 30, 255 } : Color{ 230, 230, 180, 255 };

        DrawRectangle(static_cast<int>(center.x - textWidth * 0.5f - 3), static_cast<int>(center.y - radius - 14), textWidth + 6, 12, badgeBg);
        DrawText(stateName, static_cast<int>(center.x - textWidth * 0.5f), static_cast<int>(center.y - radius - 13), fontSize, badgeTxt);
    }

    // 8. Visual Alert Exclamation Badge
    if (state == GuardState::ALERTED) {
        drawAlertBadge(center, radius, animTimer);
    }

    // 9. Visual Assassination Vulnerability Feedback
    if (isVulnerable) {
        drawVulnerabilityReticle(center, radius);
    }
}
