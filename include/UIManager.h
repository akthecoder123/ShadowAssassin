#pragma once

#include "raylib.h"
#include <string>
#include <vector>

/**
 * @brief Represents a temporary floating visual notification in the game world.
 */
struct UINotification {
    std::string text;
    Vector2 position;
    Color color;
    float lifetime;
    float maxLifetime;
};

/**
 * @brief Actions returned by UI screen interactions.
 */
enum class UIAction {
    NONE,
    START_GAME,
    RESUME_GAME,
    NEXT_LEVEL,
    RESTART_LEVEL,
    SHOW_CONTROLS,
    HIDE_CONTROLS,
    MAIN_MENU,
    EXIT_GAME
};

/**
 * @brief UIManager centralizes rendering of menus, HUD, modal dialogs, and visual feedback.
 * 
 * Demonstrates:
 * - Single Responsibility Principle (SRP): Isolates visual styling, widget positioning,
 *   button hit-testing, and HUD drawing from gameplay simulation and engine lifecycle.
 * - Encapsulation: Internal UI colors, button layouts, and notification pools are managed internally.
 */
class UIManager {
public:
    UIManager(int screenWidth = 1024, int screenHeight = 768);
    ~UIManager() = default;

    // Disallow copying to enforce clean instance management
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    // Notifications & Floating Text
    void addNotification(const std::string& text, Vector2 position, Color color, float lifetime = 1.0f);
    void update(float deltaTime);
    void drawNotifications() const;

    // Background & Floor Grid Visuals
    void drawTacticalFloor(float mapWidth, float mapHeight) const;

    // In-Game Tactical HUD
    void drawHUD(
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
    ) const;

    // Menus and Modal Overlays (Return selected UIAction)
    UIAction drawMainMenu(Vector2 mousePos, bool mouseClicked, bool showControlsModal) const;
    UIAction drawControlsModal(Vector2 mousePos, bool mouseClicked) const;
    UIAction drawPauseMenu(Vector2 mousePos, bool mouseClicked, int currentLevel) const;
    UIAction drawLevelComplete(
        Vector2 mousePos,
        bool mouseClicked,
        int currentLevel,
        const std::string& levelName,
        int coinsCollected,
        int totalCoins,
        int health,
        int maxHealth
    ) const;
    UIAction drawGameOver(
        Vector2 mousePos,
        bool mouseClicked,
        int currentLevel,
        const std::string& levelName
    ) const;
    UIAction drawVictory(
        Vector2 mousePos,
        bool mouseClicked,
        int totalCoins,
        int totalEliminations
    ) const;

    // General Reusable Widget Helpers
    static bool drawButton(
        Rectangle bounds,
        const char* text,
        int fontSize,
        Color baseCol,
        Color hoverCol,
        Color textCol,
        Vector2 mousePos,
        bool mouseClicked
    );

    int getScreenWidth() const { return m_screenWidth; }
    int getScreenHeight() const { return m_screenHeight; }
    void setScreenDimensions(int w, int h) { m_screenWidth = w; m_screenHeight = h; }

private:
    int m_screenWidth;
    int m_screenHeight;
    std::vector<UINotification> m_notifications;

    // Theme Palette Constants
    const Color COLOR_BG_DARK = Color{ 14, 16, 20, 255 };
    const Color COLOR_PANEL_BG = Color{ 18, 22, 30, 245 };
    const Color COLOR_ACCENT_CYAN = Color{ 0, 210, 255, 255 };
    const Color COLOR_ACCENT_GOLD = Color{ 255, 205, 50, 255 };
    const Color COLOR_ACCENT_RED = Color{ 230, 50, 60, 255 };
    const Color COLOR_ACCENT_GREEN = Color{ 40, 215, 120, 255 };
};
