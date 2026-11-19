#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Guard.h"
#include "Obstacle.h"
#include "Collectible.h"
#include "Level.h"
#include "UIManager.h"

/**
 * @brief High-level game states for Shadow Assassin.
 */
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    LEVEL_COMPLETE,
    GAME_OVER,
    VICTORY
};

/**
 * @brief GameManager controls the overall game lifecycle, UI state orchestration, and level progression.
 * 
 * Demonstrates:
 * - Encapsulation: Internal state, active level, and UI components are strictly private.
 * - Composition: Strongly owns the Player operative, active Level, and UIManager.
 * - Coordination: Orchestrates transitions across MENU, PLAYING, PAUSED, LEVEL_COMPLETE, GAME_OVER, VICTORY.
 */
class GameManager {
public:
    GameManager(int width = 1024, int height = 768, const std::string& title = "Shadow Assassin - Stealth Infiltration");
    ~GameManager();

    // Prevent copying to enforce unique lifecycle ownership
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // Core lifecycle methods
    bool init();
    void run();
    void update(float deltaTime);
    void draw();
    void shutdown();

    // Level progression & management
    void loadLevel(int levelNumber);
    void advanceToNextLevel();
    void restartCurrentLevel();
    void resetDemoEntities(); // Backwards compatibility for Level 1 demo

    // UI state transitions
    void pauseGame();
    void resumeGame();
    void returnToMainMenu();

    // Accessors and Mutators (Encapsulation)
    bool isRunning() const { return m_isRunning; }
    GameState getCurrentState() const { return m_currentState; }
    void setState(GameState newState) { m_currentState = newState; }
    int getScreenWidth() const { return m_screenWidth; }
    int getScreenHeight() const { return m_screenHeight; }
    int getCurrentLevelNumber() const { return m_currentLevelIndex; }

    Player* getPlayer() { return m_player.get(); }
    const Player* getPlayer() const { return m_player.get(); }

    Level* getCurrentLevel() { return m_currentLevel.get(); }
    const Level* getCurrentLevel() const { return m_currentLevel.get(); }

    UIManager* getUIManager() { return m_uiManager.get(); }
    const UIManager* getUIManager() const { return m_uiManager.get(); }

    bool isShowingControlsModal() const { return m_showControlsModal; }
    void setShowControlsModal(bool show) { m_showControlsModal = show; }

    bool isDebugMode() const { return m_debugMode; }
    void toggleDebugMode() { m_debugMode = !m_debugMode; }

private:
    int m_screenWidth;
    int m_screenHeight;
    std::string m_title;
    bool m_isRunning;
    GameState m_currentState;
    bool m_showControlsModal;
    bool m_debugMode;

    // Checkpoint values for level restarts
    int m_checkpointCoins;
    int m_checkpointEliminations;

    // Composed Entities & Systems
    int m_currentLevelIndex;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Level> m_currentLevel;
    std::unique_ptr<UIManager> m_uiManager;
};
