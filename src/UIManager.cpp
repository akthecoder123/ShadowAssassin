#include "UIManager.h"
#include <algorithm>

UIManager::UIManager(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth),
      m_screenHeight(screenHeight) {
}

void UIManager::addNotification(const std::string& text, Vector2 position, Color color, float lifetime) {
    m_notifications.push_back(UINotification{ text, position, color, lifetime, lifetime });
}

void UIManager::update(float deltaTime) {
    for (auto& notif : m_notifications) {
        notif.lifetime -= deltaTime;
        notif.position.y -= 24.0f * deltaTime; // Smooth gentle float upwards
    }

    // Erase expired notifications
    m_notifications.erase(
        std::remove_if(
            m_notifications.begin(),
            m_notifications.end(),
            [](const UINotification& n) { return n.lifetime <= 0.0f; }
        ),
        m_notifications.end()
    );
}

void UIManager::drawNotifications() const {
    if (!IsWindowReady()) return;

    for (const auto& notif : m_notifications) {
        float alpha = std::clamp(notif.lifetime / notif.maxLifetime, 0.0f, 1.0f);
        Color drawCol = ColorAlpha(notif.color, alpha);
        int textWidth = MeasureText(notif.text.c_str(), 14);
        DrawText(
            notif.text.c_str(),
            static_cast<int>(notif.position.x - textWidth / 2.0f),
            static_cast<int>(notif.position.y),
            14,
            drawCol
        );
    }
}

void UIManager::drawTacticalFloor(float mapWidth, float mapHeight) const {
    if (!IsWindowReady()) return;

    ClearBackground(COLOR_BG_DARK);

    // Subtle tactical floor grid
    int gridSize = 40;
    Color lineCol = Color{ 22, 26, 34, 255 };
    for (int x = 0; x < mapWidth; x += gridSize) {
        DrawLine(x, 0, x, static_cast<int>(mapHeight), lineCol);
    }
    for (int y = 0; y < mapHeight; y += gridSize) {
        DrawLine(0, y, static_cast<int>(mapWidth), y, lineCol);
    }

    // Stealth corner intersection dots
    for (int x = gridSize; x < mapWidth; x += gridSize) {
        for (int y = gridSize; y < mapHeight; y += gridSize) {
            DrawRectangle(x - 1, y - 1, 3, 3, Color{ 40, 50, 70, 160 });
        }
    }
}

void UIManager::drawHUD(
    int levelNumber,
    const std::string& levelName,
    int health,
    int maxHealth,
    int coins,
    int totalCoins,
    int remainingGuards,
    int totalGuards,
    int eliminations,
    int fps
) const {
    if (!IsWindowReady()) return;

    // -----------------------------------------------------------------
    // 1. Top Header Bar (Level & Mission Briefing)
    // -----------------------------------------------------------------
    DrawRectangle(0, 0, m_screenWidth, 48, Color{ 12, 14, 18, 240 });
    DrawLine(0, 48, m_screenWidth, 48, COLOR_ACCENT_CYAN);

    DrawText("SHADOW ASSASSIN", 25, 13, 22, RAYWHITE);

    // Level indicator
    DrawText(
        TextFormat("SECTOR %d: %s", levelNumber, levelName.c_str()),
        250, 16, 16, COLOR_ACCENT_CYAN
    );

    // Right-aligned engine telemetry
    DrawText(TextFormat("FPS: %d", fps), m_screenWidth - 85, 17, 13, GRAY);

    // -----------------------------------------------------------------
    // 2. Bottom Tactical Status Dock (Compact, non-obstructive)
    // -----------------------------------------------------------------
    int dockY = m_screenHeight - 65;
    DrawRectangle(0, dockY, m_screenWidth, 65, Color{ 12, 14, 18, 245 });
    DrawLine(0, dockY, m_screenWidth, dockY, Color{ 40, 50, 70, 255 });

    // Section A: Health Bar
    DrawText("HEALTH", 30, dockY + 12, 11, LIGHTGRAY);
    float healthRatio = (maxHealth > 0) ? std::clamp(static_cast<float>(health) / static_cast<float>(maxHealth), 0.0f, 1.0f) : 0.0f;
    DrawRectangle(30, dockY + 28, 140, 18, Color{ 25, 30, 40, 255 });
    Color healthBarCol = (healthRatio > 0.35f) ? COLOR_ACCENT_GREEN : COLOR_ACCENT_RED;
    DrawRectangle(30, dockY + 28, static_cast<int>(140.0f * healthRatio), 18, healthBarCol);
    DrawRectangleLines(30, dockY + 28, 140, 18, Color{ 60, 75, 95, 255 });
    DrawText(TextFormat("%d / %d", health, maxHealth), 72, dockY + 31, 12, WHITE);

    // Section B: Coins
    DrawCircle(220, dockY + 37, 8, COLOR_ACCENT_GOLD);
    DrawCircle(220, dockY + 37, 5, Color{ 255, 235, 120, 255 });
    DrawText("INTEL COINS", 238, dockY + 12, 11, LIGHTGRAY);
    DrawText(TextFormat("%d / %d", coins, totalCoins), 238, dockY + 28, 16, COLOR_ACCENT_GOLD);

    // Section C: Guards Remaining
    DrawText("GUARDS REMAINING", 380, dockY + 12, 11, LIGHTGRAY);
    Color guardCol = (remainingGuards > 0) ? COLOR_ACCENT_RED : COLOR_ACCENT_GREEN;
    DrawText(TextFormat("%d / %d", remainingGuards, totalGuards), 380, dockY + 28, 16, guardCol);

    // Section D: Eliminations
    DrawText("ELIMINATIONS", 560, dockY + 12, 11, LIGHTGRAY);
    DrawText(TextFormat("%d Kills", eliminations), 560, dockY + 28, 16, Color{ 255, 120, 120, 255 });

    // Section E: Concise Controls Hint
    DrawText("CONTROLS: [WASD] Move  [SPACE] Assassinate  [P/ESC] Pause  [R] Reset  [F11] Fullscreen",
             640, dockY + 28, 11, Color{ 140, 155, 175, 255 });
}

bool UIManager::drawButton(
    Rectangle bounds,
    const char* text,
    int fontSize,
    Color baseCol,
    Color hoverCol,
    Color textCol,
    Vector2 mousePos,
    bool mouseClicked
) {
    bool isHovered = CheckCollisionPointRec(mousePos, bounds);
    Color bg = isHovered ? hoverCol : baseCol;

    if (IsWindowReady()) {
        DrawRectangleRounded(bounds, 0.22f, 4, bg);
        DrawRectangleRoundedLinesEx(bounds, 0.22f, 4, 1.5f, isHovered ? WHITE : ColorAlpha(WHITE, 0.35f));

        int textW = MeasureText(text, fontSize);
        int textX = static_cast<int>(bounds.x + (bounds.width - textW) / 2.0f);
        int textY = static_cast<int>(bounds.y + (bounds.height - fontSize) / 2.0f);
        DrawText(text, textX, textY, fontSize, isHovered ? WHITE : textCol);
    }

    return isHovered && mouseClicked;
}

UIAction UIManager::drawMainMenu(Vector2 mousePos, bool mouseClicked, bool showControlsModal) const {
    if (IsWindowReady()) {
        ClearBackground(COLOR_BG_DARK);

        // Animated/Tactical Background Floor Grid
        int gridSize = 40;
        Color lineCol = Color{ 22, 28, 38, 255 };
        for (int x = 0; x < m_screenWidth; x += gridSize) {
            DrawLine(x, 0, x, m_screenHeight, lineCol);
        }
        for (int y = 0; y < m_screenHeight; y += gridSize) {
            DrawLine(0, y, m_screenWidth, y, lineCol);
        }

        // Glowing Decorative Frame
        DrawRectangleLines(30, 30, m_screenWidth - 60, m_screenHeight - 60, Color{ 40, 60, 90, 150 });
        DrawRectangleLines(34, 34, m_screenWidth - 68, m_screenHeight - 68, Color{ 25, 35, 55, 100 });

        // Title Block
        const char* title = "SHADOW ASSASSIN";
        int titleW = MeasureText(title, 52);
        DrawText(title, (m_screenWidth - titleW) / 2, 140, 52, RAYWHITE);

        const char* subtitle = "TACTICAL TOP-DOWN 2D STEALTH INFILTRATION";
        int subW = MeasureText(subtitle, 16);
        DrawText(subtitle, (m_screenWidth - subW) / 2, 205, 16, COLOR_ACCENT_CYAN);

        DrawLine((m_screenWidth - 420) / 2, 235, (m_screenWidth + 420) / 2, 235, Color{ 0, 180, 220, 160 });

        // Footer Info
        const char* footer = "C++17 OOP Academic Project  |  raylib 5.5  |  Hunter Assassin Inspired";
        int footerW = MeasureText(footer, 13);
        DrawText(footer, (m_screenWidth - footerW) / 2, m_screenHeight - 75, 13, Color{ 110, 125, 145, 255 });
    }

    // Buttons
    int btnW = 280;
    int btnH = 50;
    int btnX = (m_screenWidth - btnW) / 2;

    if (drawButton(Rectangle{ static_cast<float>(btnX), 280.0f, static_cast<float>(btnW), static_cast<float>(btnH) },
                   "START CAMPAIGN", 18,
                   Color{ 20, 50, 85, 255 }, Color{ 30, 90, 160, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::START_GAME;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), 350.0f, static_cast<float>(btnW), static_cast<float>(btnH) },
                   "HOW TO PLAY / INTEL", 18,
                   Color{ 25, 32, 45, 255 }, Color{ 45, 60, 85, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::SHOW_CONTROLS;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), 420.0f, static_cast<float>(btnW), static_cast<float>(btnH) },
                   "EXIT GAME", 18,
                   Color{ 50, 22, 28, 255 }, Color{ 95, 30, 42, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::EXIT_GAME;
    }

    // Controls Modal Display if toggled
    if (showControlsModal) {
        return drawControlsModal(mousePos, mouseClicked);
    }

    return UIAction::NONE;
}

UIAction UIManager::drawControlsModal(Vector2 mousePos, bool mouseClicked) const {
    int cardW = 600;
    int cardH = 490;
    int cardX = (m_screenWidth - cardW) / 2;
    int cardY = (m_screenHeight - cardH) / 2;

    if (IsWindowReady()) {
        DrawRectangle(0, 0, m_screenWidth, m_screenHeight, Color{ 0, 0, 0, 195 });
        DrawRectangle(cardX, cardY, cardW, cardH, COLOR_PANEL_BG);
        DrawRectangleLines(cardX, cardY, cardW, cardH, COLOR_ACCENT_CYAN);

        DrawText("FIELD MANUAL & CONTROLS", cardX + 130, cardY + 28, 22, COLOR_ACCENT_CYAN);
        DrawLine(cardX + 40, cardY + 60, cardX + cardW - 40, cardY + 60, Color{ 50, 150, 200, 150 });

        int startY = cardY + 80;
        int lineSpacing = 30;

        DrawText("[W, A, S, D]  or  [Arrows]", cardX + 50, startY, 15, COLOR_ACCENT_GOLD);
        DrawText("Fluid 8-directional movement with normalized speed", cardX + 245, startY, 14, RAYWHITE);

        DrawText("[SPACEBAR]", cardX + 50, startY + lineSpacing, 15, COLOR_ACCENT_RED);
        DrawText("Assassinate target when within 65px from behind", cardX + 245, startY + lineSpacing, 14, RAYWHITE);

        DrawText("OBSTACLES & WALLS", cardX + 50, startY + lineSpacing * 2, 15, Color{ 100, 200, 255, 255 });
        DrawText("Physically occlude vision cones and enemy patrol paths", cardX + 245, startY + lineSpacing * 2, 14, RAYWHITE);

        DrawText("GUARD VISION CONES", cardX + 50, startY + lineSpacing * 3, 15, Color{ 255, 140, 60, 255 });
        DrawText("Yellow: Patrol  |  Orange: Alert  |  Red: Chasing", cardX + 245, startY + lineSpacing * 3, 14, RAYWHITE);

        DrawText("INTEL COINS", cardX + 50, startY + lineSpacing * 4, 15, COLOR_ACCENT_GOLD);
        DrawText("Harvest scattered bonus coins to maximize score", cardX + 245, startY + lineSpacing * 4, 14, RAYWHITE);

        DrawText("[P] or [ESC] / [R]", cardX + 50, startY + lineSpacing * 5, 15, LIGHTGRAY);
        DrawText("Pause game session / Quick restart current sector", cardX + 245, startY + lineSpacing * 5, 14, RAYWHITE);

        DrawText("[F11] or [ALT+ENTER]", cardX + 50, startY + lineSpacing * 6, 15, COLOR_ACCENT_CYAN);
        DrawText("Toggle between Fullscreen and Windowed mode", cardX + 245, startY + lineSpacing * 6, 14, RAYWHITE);

        DrawText("PRIMARY OBJECTIVE:", cardX + 50, startY + lineSpacing * 7 + 10, 15, COLOR_ACCENT_GREEN);
        DrawText("Eliminate all guards in each sector to progress to victory!", cardX + 50, startY + lineSpacing * 8 + 10, 14, RAYWHITE);
    }

    int btnW = 200;
    int btnH = 42;
    int btnX = (m_screenWidth - btnW) / 2;
    int btnY = cardY + cardH - 65;

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(btnY), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "CLOSE INTEL [ESC]", 16,
                   Color{ 30, 45, 65, 255 }, Color{ 45, 75, 110, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::HIDE_CONTROLS;
    }

    return UIAction::NONE;
}

UIAction UIManager::drawPauseMenu(Vector2 mousePos, bool mouseClicked, int currentLevel) const {
    int cardW = 440;
    int cardH = 360;
    int cardX = (m_screenWidth - cardW) / 2;
    int cardY = (m_screenHeight - cardH) / 2;

    if (IsWindowReady()) {
        DrawRectangle(0, 0, m_screenWidth, m_screenHeight, Color{ 0, 0, 0, 180 });
        DrawRectangle(cardX, cardY, cardW, cardH, COLOR_PANEL_BG);
        DrawRectangleLines(cardX, cardY, cardW, cardH, COLOR_ACCENT_CYAN);

        DrawText("MISSION PAUSED", cardX + 125, cardY + 28, 24, COLOR_ACCENT_CYAN);
        DrawText(TextFormat("Sector %d Deployment Frozen", currentLevel), cardX + 115, cardY + 62, 14, LIGHTGRAY);
        DrawLine(cardX + 40, cardY + 85, cardX + cardW - 40, cardY + 85, Color{ 50, 150, 200, 140 });
    }

    int btnW = 260;
    int btnH = 48;
    int btnX = (m_screenWidth - btnW) / 2;

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 110), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "RESUME MISSION [P/ESC]", 16,
                   Color{ 20, 50, 85, 255 }, Color{ 30, 85, 150, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::RESUME_GAME;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 175), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "RESTART SECTOR [R]", 16,
                   Color{ 30, 40, 55, 255 }, Color{ 50, 70, 95, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::RESTART_LEVEL;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 240), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "ABORT TO MAIN MENU", 16,
                   Color{ 50, 25, 30, 255 }, Color{ 90, 35, 45, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::MAIN_MENU;
    }

    return UIAction::NONE;
}

UIAction UIManager::drawLevelComplete(
    Vector2 mousePos,
    bool mouseClicked,
    int currentLevel,
    const std::string& levelName,
    int coinsCollected,
    int totalCoins,
    int health,
    int maxHealth
) const {
    int cardW = 540;
    int cardH = 390;
    int cardX = (m_screenWidth - cardW) / 2;
    int cardY = (m_screenHeight - cardH) / 2;

    if (IsWindowReady()) {
        DrawRectangle(0, 0, m_screenWidth, m_screenHeight, Color{ 0, 0, 0, 190 });
        DrawRectangle(cardX, cardY, cardW, cardH, COLOR_PANEL_BG);
        DrawRectangleLines(cardX, cardY, cardW, cardH, COLOR_ACCENT_CYAN);

        DrawText("SECTOR CLEARED!", cardX + 160, cardY + 28, 26, COLOR_ACCENT_CYAN);
        DrawLine(cardX + 40, cardY + 68, cardX + cardW - 40, cardY + 68, Color{ 50, 180, 255, 160 });

        DrawText(TextFormat("Sector %d (%s) Neutralized", currentLevel, levelName.c_str()),
                 cardX + 60, cardY + 90, 16, RAYWHITE);

        DrawText(TextFormat("Intel Coins Harvested: %d / %d", coinsCollected, totalCoins),
                 cardX + 60, cardY + 125, 15, COLOR_ACCENT_GOLD);

        DrawText(TextFormat("Operative Health: %d / %d", health, maxHealth),
                 cardX + 60, cardY + 155, 15, COLOR_ACCENT_GREEN);

        DrawText("All sentries eliminated without compromising mission.",
                 cardX + 60, cardY + 190, 14, LIGHTGRAY);
    }

    int btnW = 280;
    int btnH = 46;
    int btnX = (m_screenWidth - btnW) / 2;

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 225), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "PROCEED TO NEXT SECTOR [ENTER]", 15,
                   Color{ 20, 75, 55, 255 }, Color{ 30, 120, 85, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::NEXT_LEVEL;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 280), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "REPLAY SECTOR [R]", 15,
                   Color{ 25, 35, 50, 255 }, Color{ 45, 60, 85, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::RESTART_LEVEL;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 335), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "MAIN MENU", 15,
                   Color{ 40, 25, 30, 255 }, Color{ 75, 35, 45, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::MAIN_MENU;
    }

    return UIAction::NONE;
}

UIAction UIManager::drawGameOver(
    Vector2 mousePos,
    bool mouseClicked,
    int currentLevel,
    const std::string& levelName
) const {
    int cardW = 500;
    int cardH = 340;
    int cardX = (m_screenWidth - cardW) / 2;
    int cardY = (m_screenHeight - cardH) / 2;

    if (IsWindowReady()) {
        DrawRectangle(0, 0, m_screenWidth, m_screenHeight, Color{ 0, 0, 0, 195 });
        DrawRectangle(cardX, cardY, cardW, cardH, Color{ 25, 14, 16, 250 });
        DrawRectangleLines(cardX, cardY, cardW, cardH, COLOR_ACCENT_RED);

        DrawText("MISSION FAILED", cardX + 150, cardY + 30, 28, COLOR_ACCENT_RED);
        DrawLine(cardX + 40, cardY + 70, cardX + cardW - 40, cardY + 70, Color{ 220, 50, 50, 160 });

        DrawText(TextFormat("Operative compromised in Sector %d (%s).", currentLevel, levelName.c_str()),
                 cardX + 50, cardY + 95, 15, RAYWHITE);
        DrawText("Stealth cover was breached. Sentry guards neutralized operative.",
                 cardX + 50, cardY + 125, 13, LIGHTGRAY);
    }

    int btnW = 260;
    int btnH = 48;
    int btnX = (m_screenWidth - btnW) / 2;

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 175), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "RETRY SECTOR [R]", 16,
                   Color{ 75, 25, 30, 255 }, Color{ 130, 35, 45, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::RESTART_LEVEL;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 245), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "RETURN TO MAIN MENU", 16,
                   Color{ 30, 35, 45, 255 }, Color{ 50, 60, 80, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::MAIN_MENU;
    }

    return UIAction::NONE;
}

UIAction UIManager::drawVictory(
    Vector2 mousePos,
    bool mouseClicked,
    int totalCoins,
    int totalEliminations
) const {
    int cardW = 580;
    int cardH = 400;
    int cardX = (m_screenWidth - cardW) / 2;
    int cardY = (m_screenHeight - cardH) / 2;

    if (IsWindowReady()) {
        DrawRectangle(0, 0, m_screenWidth, m_screenHeight, Color{ 0, 0, 0, 205 });
        DrawRectangle(cardX, cardY, cardW, cardH, Color{ 12, 28, 20, 250 });
        DrawRectangleLines(cardX, cardY, cardW, cardH, COLOR_ACCENT_GREEN);

        DrawText("CAMPAIGN VICTORY!", cardX + 155, cardY + 30, 28, COLOR_ACCENT_GREEN);
        DrawLine(cardX + 40, cardY + 70, cardX + cardW - 40, cardY + 70, Color{ 40, 220, 120, 160 });

        DrawText("All 5 high-security sectors successfully neutralized!",
                 cardX + 90, cardY + 95, 16, RAYWHITE);
        DrawText("Shadow Assassin operative completed all infiltration contracts.",
                 cardX + 75, cardY + 125, 14, LIGHTGRAY);

        // Stats summary card
        DrawRectangle(cardX + 50, cardY + 160, cardW - 100, 65, Color{ 18, 40, 28, 255 });
        DrawRectangleLines(cardX + 50, cardY + 160, cardW - 100, 65, Color{ 40, 150, 85, 200 });

        DrawText(TextFormat("Total Assassinations: %d Guards Neutralized", totalEliminations),
                 cardX + 70, cardY + 175, 14, Color{ 255, 130, 130, 255 });
        DrawText(TextFormat("Total Intel Coins Harvested: %d Gold Coins", totalCoins),
                 cardX + 70, cardY + 198, 14, COLOR_ACCENT_GOLD);
    }

    int btnW = 280;
    int btnH = 48;
    int btnX = (m_screenWidth - btnW) / 2;

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 245), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "PLAY AGAIN (FROM LEVEL 1) [R]", 15,
                   Color{ 20, 75, 45, 255 }, Color{ 35, 125, 75, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::START_GAME;
    }

    if (drawButton(Rectangle{ static_cast<float>(btnX), static_cast<float>(cardY + 310), static_cast<float>(btnW), static_cast<float>(btnH) },
                   "MAIN MENU", 15,
                   Color{ 30, 40, 50, 255 }, Color{ 50, 65, 80, 255 },
                   RAYWHITE, mousePos, mouseClicked)) {
        return UIAction::MAIN_MENU;
    }

    return UIAction::NONE;
}
