#include "GameManager.h"
#include "CollisionManager.h"
#include "CharacterRenderer.h"
#include "raylib.h"
#include <iostream>
#include <cmath>
#include <algorithm>

GameManager::GameManager(int width, int height, const std::string& title)
    : m_screenWidth(width),
      m_screenHeight(height),
      m_title(title),
      m_isRunning(false),
      m_currentState(GameState::MENU),
      m_showControlsModal(false),
      m_debugMode(false),
      m_checkpointCoins(0),
      m_checkpointEliminations(0),
      m_currentLevelIndex(1) {
}

GameManager::~GameManager() {
    shutdown();
}

void GameManager::loadLevel(int levelNumber) {
    m_currentLevelIndex = std::clamp(levelNumber, 1, 5);
    m_currentLevel = std::make_unique<Level>(m_currentLevelIndex);

    // Reset campaign totals when starting fresh from Level 1
    if (m_currentLevelIndex == 1) {
        m_checkpointCoins = 0;
        m_checkpointEliminations = 0;
        if (m_player) {
            m_player->setCoins(0);
            m_player->setEliminations(0);
        }
    } else if (m_player) {
        // Save checkpoint values when progressing into Level 2, 3, 4, or 5
        m_checkpointCoins = m_player->getCoins();
        m_checkpointEliminations = m_player->getEliminations();
    }

    // Spawn player at designated level spawn location
    if (!m_player) {
        m_player = std::make_unique<Player>(m_currentLevel->getPlayerSpawn(), 16.0f, 100, 190.0f);
    } else {
        m_player->setPosition(m_currentLevel->getPlayerSpawn());
        m_player->resetHealth();
    }
    m_player->setFacingAngle(0.0f);
    m_currentState = GameState::PLAYING;

    std::cout << "[GameManager] Loaded Level " << m_currentLevelIndex
              << ": " << m_currentLevel->getName() << std::endl;
}

void GameManager::advanceToNextLevel() {
    if (m_currentLevelIndex < 5) {
        loadLevel(m_currentLevelIndex + 1);
    } else {
        m_currentState = GameState::VICTORY;
    }
}

void GameManager::restartCurrentLevel() {
    if (m_currentLevel) {
        m_currentLevel->reset();
        if (m_player) {
            m_player->setPosition(m_currentLevel->getPlayerSpawn());
            m_player->resetHealth();
            m_player->setFacingAngle(0.0f);
            m_player->setCoins(m_checkpointCoins);
            m_player->setEliminations(m_checkpointEliminations);
        }
        m_currentState = GameState::PLAYING;
    } else {
        loadLevel(1);
    }
}

void GameManager::resetDemoEntities() {
    loadLevel(1);
}

void GameManager::pauseGame() {
    if (m_currentState == GameState::PLAYING) {
        m_currentState = GameState::PAUSED;
    }
}

void GameManager::resumeGame() {
    if (m_currentState == GameState::PAUSED) {
        m_currentState = GameState::PLAYING;
    }
}

void GameManager::returnToMainMenu() {
    m_currentState = GameState::MENU;
    m_showControlsModal = false;
}

bool GameManager::init() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(m_screenWidth, m_screenHeight, m_title.c_str());
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        std::cerr << "[GameManager] Error: Failed to initialize Raylib window!" << std::endl;
        m_isRunning = false;
        return false;
    }

    m_uiManager = std::make_unique<UIManager>(m_screenWidth, m_screenHeight);
    CharacterRenderer::init();

    // Pre-initialize Level 1
    m_currentLevel = std::make_unique<Level>(1);
    m_player = std::make_unique<Player>(m_currentLevel->getPlayerSpawn(), 16.0f, 100, 190.0f);
    m_currentState = GameState::MENU;

    m_isRunning = true;
    std::cout << "[GameManager] Phase 6 UI and Visual Polish System initialized." << std::endl;
    return true;
}

void GameManager::run() {
    if (!m_isRunning && !init()) {
        return;
    }

    while (m_isRunning && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        update(deltaTime);
        draw();
    }

    shutdown();
}

void GameManager::update(float deltaTime) {
    // -------------------------------------------------------------
    // Global & State Keyboard Shortcuts
    // -------------------------------------------------------------
    if (m_currentState == GameState::MENU) {
        if (m_showControlsModal) {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                m_showControlsModal = false;
            }
        } else {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                loadLevel(1);
            }
            if (IsKeyPressed(KEY_C)) {
                m_showControlsModal = true;
            }
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)) {
                m_isRunning = false;
                return;
            }
        }
        return;
    }

    if (m_currentState == GameState::PAUSED) {
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
            resumeGame();
        } else if (IsKeyPressed(KEY_R)) {
            restartCurrentLevel();
        } else if (IsKeyPressed(KEY_M)) {
            returnToMainMenu();
        }
        return;
    }

    if (m_currentState == GameState::LEVEL_COMPLETE) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            advanceToNextLevel();
        } else if (IsKeyPressed(KEY_R)) {
            restartCurrentLevel();
        } else if (IsKeyPressed(KEY_M)) {
            returnToMainMenu();
        }
        return;
    }

    if (m_currentState == GameState::GAME_OVER) {
        if (IsKeyPressed(KEY_R)) {
            restartCurrentLevel();
        } else if (IsKeyPressed(KEY_M)) {
            returnToMainMenu();
        } else if (IsKeyPressed(KEY_ESCAPE)) {
            returnToMainMenu();
        }
        return;
    }

    if (m_currentState == GameState::VICTORY) {
        if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            loadLevel(1);
        } else if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) {
            returnToMainMenu();
        }
        return;
    }

    // -------------------------------------------------------------
    // Active Gameplay Simulation (GameState::PLAYING)
    // -------------------------------------------------------------
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
        pauseGame();
        return;
    }

    if (IsKeyPressed(KEY_R)) {
        restartCurrentLevel();
        if (m_uiManager) {
            m_uiManager->addNotification("SECTOR RESET", Vector2{ m_screenWidth / 2.0f, m_screenHeight / 2.0f }, YELLOW);
        }
        return;
    }

    // Toggle Developer Debug Mode via F1
    if (IsKeyPressed(KEY_F1)) {
        toggleDebugMode();
        if (m_uiManager) {
            m_uiManager->addNotification(
                m_debugMode ? "DEVELOPER DEBUG: ENABLED" : "DEVELOPER DEBUG: DISABLED",
                Vector2{ m_screenWidth / 2.0f, 100.0f },
                m_debugMode ? ORANGE : GRAY
            );
        }
    }

    // Developer Debug Hotkeys (Only active when F1 toggle is ON)
    if (m_debugMode) {
        if (IsKeyPressed(KEY_ONE))   loadLevel(1);
        if (IsKeyPressed(KEY_FOUR))  loadLevel(2);
        if (IsKeyPressed(KEY_FIVE))  loadLevel(3);
        if (IsKeyPressed(KEY_SIX))   loadLevel(4);
        if (IsKeyPressed(KEY_SEVEN)) loadLevel(5);

        if (IsKeyPressed(KEY_TWO) && m_player) {
            m_player->takeDamage(20);
        }
        if (IsKeyPressed(KEY_THREE) && m_player) {
            m_player->heal(20);
        }
    }

    if (!m_currentLevel || !m_player) return;

    // 1. Process Player Input & Obstacle Collision
    if (m_player->isAlive()) {
        m_player->handleInput();
        m_player->moveAndSlide(
            deltaTime,
            m_currentLevel->getObstacles(),
            static_cast<float>(m_screenWidth),
            m_currentLevel->getMapHeight()
        );

        // 2. Check Collectible Overlaps
        for (auto& coin : m_currentLevel->getCoins()) {
            if (!coin.isCollected() && coin.checkCollision(m_player->getCenter(), m_player->getRadius())) {
                coin.collect();
                m_player->addCoins(coin.getValue());
                if (m_uiManager) {
                    m_uiManager->addNotification("+1 INTEL", coin.getPosition(), Color{ 255, 215, 50, 255 });
                }
            }
        }

        // 3. Evaluate Assassination Vulnerability across living guards
        bool assassinationExecuted = false;
        for (auto& guard : m_currentLevel->getGuards()) {
            if (guard->isAlive()) {
                bool canStrike = guard->isVulnerableTo(*m_player, 65.0f, 70.0f);
                guard->setVulnerable(canStrike);

                if (canStrike && IsKeyPressed(KEY_SPACE) && !assassinationExecuted) {
                    guard->eliminate();
                    m_player->addElimination();
                    m_player->triggerAssassinationEffect(guard->getCenter());
                    if (m_uiManager) {
                        m_uiManager->addNotification("ASSASSINATED!", guard->getCenter(), Color{ 255, 60, 60, 255 });
                    }
                    assassinationExecuted = true;
                }
            } else {
                guard->setVulnerable(false);
            }
        }
    } else {
        m_currentState = GameState::GAME_OVER;
    }

    // 4. Update Level Entities (Guard AI FSM & Coin animations)
    m_currentLevel->update(deltaTime, *m_player);

    // 5. Update Player internal timers
    m_player->update(deltaTime);

    // 6. Update UI floating feedback notifications
    if (m_uiManager) {
        m_uiManager->update(deltaTime);
    }

    // 7. Check Level Completion Criteria
    if (m_player->isAlive() && m_currentLevel->areAllGuardsEliminated()) {
        if (m_currentLevelIndex >= 5) {
            m_currentState = GameState::VICTORY;
        } else {
            m_currentState = GameState::LEVEL_COMPLETE;
        }
    }
}

void GameManager::draw() {
    BeginDrawing();

    Vector2 mousePos = GetMousePosition();
    bool mouseClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    switch (m_currentState) {
        case GameState::MENU: {
            if (m_uiManager) {
                UIAction action = m_uiManager->drawMainMenu(mousePos, mouseClicked, m_showControlsModal);
                if (action == UIAction::START_GAME) {
                    loadLevel(1);
                } else if (action == UIAction::SHOW_CONTROLS) {
                    m_showControlsModal = true;
                } else if (action == UIAction::HIDE_CONTROLS) {
                    m_showControlsModal = false;
                } else if (action == UIAction::EXIT_GAME) {
                    m_isRunning = false;
                }
            }
            break;
        }

        case GameState::PLAYING: {
            if (m_uiManager && m_currentLevel) {
                m_uiManager->drawTacticalFloor(m_currentLevel->getMapWidth(), m_currentLevel->getMapHeight());
            }

            // Draw Level (Obstacles, Collectibles, Guards)
            if (m_currentLevel) {
                m_currentLevel->draw();
            }

            // Draw Player
            if (m_player) {
                m_player->draw();
            }

            // Draw Floating Notifications
            if (m_uiManager) {
                m_uiManager->drawNotifications();
            }

            // Draw In-Game HUD
            if (m_uiManager && m_currentLevel && m_player) {
                m_uiManager->drawHUD(
                    m_currentLevelIndex,
                    m_currentLevel->getName(),
                    m_player->getHealth(),
                    m_player->getMaxHealth(),
                    m_player->getCoins(),
                    m_currentLevel->getTotalCoinsCount(),
                    m_currentLevel->getRemainingGuardsCount(),
                    m_currentLevel->getTotalGuardsCount(),
                    m_player->getEliminations(),
                    GetFPS()
                );
            }
            break;
        }

        case GameState::PAUSED: {
            // Draw frozen gameplay scene underneath
            if (m_uiManager && m_currentLevel) {
                m_uiManager->drawTacticalFloor(m_currentLevel->getMapWidth(), m_currentLevel->getMapHeight());
            }
            if (m_currentLevel) m_currentLevel->draw();
            if (m_player) m_player->draw();
            if (m_uiManager && m_currentLevel && m_player) {
                m_uiManager->drawHUD(
                    m_currentLevelIndex,
                    m_currentLevel->getName(),
                    m_player->getHealth(),
                    m_player->getMaxHealth(),
                    m_player->getCoins(),
                    m_currentLevel->getTotalCoinsCount(),
                    m_currentLevel->getRemainingGuardsCount(),
                    m_currentLevel->getTotalGuardsCount(),
                    m_player->getEliminations(),
                    GetFPS()
                );
                UIAction action = m_uiManager->drawPauseMenu(mousePos, mouseClicked, m_currentLevelIndex);
                if (action == UIAction::RESUME_GAME) {
                    resumeGame();
                } else if (action == UIAction::RESTART_LEVEL) {
                    restartCurrentLevel();
                } else if (action == UIAction::MAIN_MENU) {
                    returnToMainMenu();
                }
            }
            break;
        }

        case GameState::LEVEL_COMPLETE: {
            // Draw completed gameplay scene underneath
            if (m_uiManager && m_currentLevel) {
                m_uiManager->drawTacticalFloor(m_currentLevel->getMapWidth(), m_currentLevel->getMapHeight());
            }
            if (m_currentLevel) m_currentLevel->draw();
            if (m_player) m_player->draw();
            if (m_uiManager && m_currentLevel && m_player) {
                m_uiManager->drawHUD(
                    m_currentLevelIndex,
                    m_currentLevel->getName(),
                    m_player->getHealth(),
                    m_player->getMaxHealth(),
                    m_player->getCoins(),
                    m_currentLevel->getTotalCoinsCount(),
                    m_currentLevel->getRemainingGuardsCount(),
                    m_currentLevel->getTotalGuardsCount(),
                    m_player->getEliminations(),
                    GetFPS()
                );
                UIAction action = m_uiManager->drawLevelComplete(
                    mousePos,
                    mouseClicked,
                    m_currentLevelIndex,
                    m_currentLevel->getName(),
                    m_currentLevel->getCollectedCoinsCount(),
                    m_currentLevel->getTotalCoinsCount(),
                    m_player->getHealth(),
                    m_player->getMaxHealth()
                );
                if (action == UIAction::NEXT_LEVEL) {
                    advanceToNextLevel();
                } else if (action == UIAction::RESTART_LEVEL) {
                    restartCurrentLevel();
                } else if (action == UIAction::MAIN_MENU) {
                    returnToMainMenu();
                }
            }
            break;
        }

        case GameState::GAME_OVER: {
            // Draw gameplay scene underneath
            if (m_uiManager && m_currentLevel) {
                m_uiManager->drawTacticalFloor(m_currentLevel->getMapWidth(), m_currentLevel->getMapHeight());
            }
            if (m_currentLevel) m_currentLevel->draw();
            if (m_player) m_player->draw();
            if (m_uiManager && m_currentLevel && m_player) {
                m_uiManager->drawHUD(
                    m_currentLevelIndex,
                    m_currentLevel->getName(),
                    m_player->getHealth(),
                    m_player->getMaxHealth(),
                    m_player->getCoins(),
                    m_currentLevel->getTotalCoinsCount(),
                    m_currentLevel->getRemainingGuardsCount(),
                    m_currentLevel->getTotalGuardsCount(),
                    m_player->getEliminations(),
                    GetFPS()
                );
                UIAction action = m_uiManager->drawGameOver(
                    mousePos,
                    mouseClicked,
                    m_currentLevelIndex,
                    m_currentLevel->getName()
                );
                if (action == UIAction::RESTART_LEVEL) {
                    restartCurrentLevel();
                } else if (action == UIAction::MAIN_MENU) {
                    returnToMainMenu();
                }
            }
            break;
        }

        case GameState::VICTORY: {
            // Draw final victorious scene underneath
            if (m_uiManager && m_currentLevel) {
                m_uiManager->drawTacticalFloor(m_currentLevel->getMapWidth(), m_currentLevel->getMapHeight());
            }
            if (m_currentLevel) m_currentLevel->draw();
            if (m_player) m_player->draw();
            if (m_uiManager && m_player) {
                UIAction action = m_uiManager->drawVictory(
                    mousePos,
                    mouseClicked,
                    m_player->getCoins(),
                    m_player->getEliminations()
                );
                if (action == UIAction::START_GAME) {
                    loadLevel(1);
                } else if (action == UIAction::MAIN_MENU) {
                    returnToMainMenu();
                }
            }
            break;
        }
    }

    EndDrawing();
}

void GameManager::shutdown() {
    if (m_isRunning) {
        std::cout << "[GameManager] Shutting down application cleanly..." << std::endl;
        CharacterRenderer::shutdown();
        m_uiManager.reset();
        m_currentLevel.reset();
        m_player.reset();
        m_isRunning = false;
        if (IsWindowReady()) {
            CloseWindow();
        }
    }
}
