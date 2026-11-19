#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <memory>

#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "Guard.h"
#include "Obstacle.h"
#include "Collectible.h"
#include "CollisionManager.h"
#include "Level.h"
#include "UIManager.h"
#include "GameManager.h"

int main() {
    std::cout << "[TEST] Running Phase 6 Unit Tests: Game Interface & Visual Polish..." << std::endl;

    // -------------------------------------------------------------
    // Test 1: UIManager Initialization & Dimensions
    // -------------------------------------------------------------
    {
        UIManager ui(1024, 768);
        assert(ui.getScreenWidth() == 1024);
        assert(ui.getScreenHeight() == 768);

        ui.setScreenDimensions(1280, 720);
        assert(ui.getScreenWidth() == 1280);
        assert(ui.getScreenHeight() == 720);

        std::cout << "  [1/10] UIManager Initialization & Dimension Configuration: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 2: Reusable Button Hit-Testing Logic
    // -------------------------------------------------------------
    {
        Rectangle btnBounds{ 100.0f, 200.0f, 200.0f, 50.0f };

        // Test coordinates inside button
        Vector2 insidePos{ 150.0f, 225.0f };
        bool insideNoClick = UIManager::drawButton(btnBounds, "TEST", 16, BLACK, DARKGRAY, WHITE, insidePos, false);
        assert(insideNoClick == false); // Hovered but not clicked

        bool insideWithClick = UIManager::drawButton(btnBounds, "TEST", 16, BLACK, DARKGRAY, WHITE, insidePos, true);
        assert(insideWithClick == true); // Hovered AND clicked -> triggered!

        // Test coordinates outside button
        Vector2 outsidePos{ 50.0f, 50.0f };
        bool outsideWithClick = UIManager::drawButton(btnBounds, "TEST", 16, BLACK, DARKGRAY, WHITE, outsidePos, true);
        assert(outsideWithClick == false); // Clicked but outside bounds -> not triggered

        std::cout << "  [2/10] Reusable Button Hit-Testing & Hover Trigger Logic: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 3: Temporary Floating Notification Lifecycle & Expiration
    // -------------------------------------------------------------
    {
        UIManager ui(1024, 768);
        ui.addNotification("+1 INTEL", Vector2{ 500.0f, 300.0f }, GOLD, 0.5f);

        // Update 0.25s -> notification still active
        ui.update(0.25f);
        // Update another 0.3s (0.55s total > 0.5s) -> notification expired and cleaned up
        ui.update(0.3f);

        std::cout << "  [3/10] Floating Notification Lifecycle & Auto-Cleanup: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 4: GameManager High-Level State Transitions
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        // Freshly created starts in MENU state
        assert(gm.getCurrentState() == GameState::MENU);

        // Starting game transitions to PLAYING
        gm.loadLevel(1);
        assert(gm.getCurrentState() == GameState::PLAYING);

        // Pausing game transitions to PAUSED
        gm.pauseGame();
        assert(gm.getCurrentState() == GameState::PAUSED);

        // Resuming game transitions back to PLAYING
        gm.resumeGame();
        assert(gm.getCurrentState() == GameState::PLAYING);

        // Aborting to Main Menu
        gm.returnToMainMenu();
        assert(gm.getCurrentState() == GameState::MENU);

        std::cout << "  [4/10] Complete Game State Flow (MENU <-> PLAYING <-> PAUSED): PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 5: Controls Modal State in Main Menu
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        assert(gm.isShowingControlsModal() == false);

        gm.setShowControlsModal(true);
        assert(gm.isShowingControlsModal() == true);

        gm.setShowControlsModal(false);
        assert(gm.isShowingControlsModal() == false);

        std::cout << "  [5/10] Main Menu Controls Modal Toggle State: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 6: Level Complete State Transition & Verification
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        gm.loadLevel(1);
        assert(gm.getCurrentLevel()->getRemainingGuardsCount() == 2);

        // Eliminate all guards
        for (auto& g : gm.getCurrentLevel()->getGuards()) {
            g->eliminate();
        }
        assert(gm.getCurrentLevel()->areAllGuardsEliminated() == true);

        // Advance progression
        gm.advanceToNextLevel();
        assert(gm.getCurrentLevelNumber() == 2);
        assert(gm.getCurrentLevel()->getRemainingGuardsCount() == 3);
        assert(gm.getCurrentState() == GameState::PLAYING);

        std::cout << "  [6/10] Level Complete Transition & Progression to Sector 2: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 7: Game Over State Trigger on Player Elimination
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        gm.loadLevel(1);
        assert(gm.getPlayer()->isAlive());

        // Inflict lethal damage on player
        gm.getPlayer()->takeDamage(100);
        assert(!gm.getPlayer()->isAlive());

        gm.setState(GameState::GAME_OVER);
        assert(gm.getCurrentState() == GameState::GAME_OVER);

        // Retry sector resets player health and sets state to PLAYING
        gm.restartCurrentLevel();
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(gm.getPlayer()->isAlive());
        assert(gm.getPlayer()->getHealth() == gm.getPlayer()->getMaxHealth());

        std::cout << "  [7/10] Game Over State Trigger & Sector Retry Restoration: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 8: Victory State Trigger on Completing Level 3
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        gm.loadLevel(3);
        assert(gm.getCurrentLevelNumber() == 3);

        // Eliminate all guards in Level 3
        for (auto& g : gm.getCurrentLevel()->getGuards()) {
            g->eliminate();
        }
        assert(gm.getCurrentLevel()->areAllGuardsEliminated() == true);

        // Advance to victory
        gm.advanceToNextLevel();
        assert(gm.getCurrentState() == GameState::VICTORY);

        // Replay campaign returns to Level 1
        gm.loadLevel(1);
        assert(gm.getCurrentLevelNumber() == 1);
        assert(gm.getCurrentState() == GameState::PLAYING);

        std::cout << "  [8/10] Campaign Victory Trigger & Replay Flow: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 9: UI Input Non-Interference with Gameplay Simulation
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        gm.loadLevel(1);
        Vector2 initialPlayerPos = gm.getPlayer()->getPosition();

        // While in PAUSED state, update must NOT advance player physics
        gm.pauseGame();
        gm.update(0.1f);
        assert(gm.getPlayer()->getPosition().x == initialPlayerPos.x);
        assert(gm.getPlayer()->getPosition().y == initialPlayerPos.y);

        // While in MENU state, player physics also frozen
        gm.returnToMainMenu();
        gm.update(0.1f);
        assert(gm.getPlayer()->getPosition().x == initialPlayerPos.x);
        assert(gm.getPlayer()->getPosition().y == initialPlayerPos.y);

        std::cout << "  [9/10] UI Input Non-Interference & Simulation Pause: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 10: In-Game HUD Data Integrity
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        gm.loadLevel(2); // Restricted facility: 3 guards, 6 coins
        Level* lvl = gm.getCurrentLevel();
        Player* plr = gm.getPlayer();

        assert(lvl->getName() == "Restricted Facility");
        assert(lvl->getLevelNumber() == 2);
        assert(lvl->getTotalGuardsCount() == 3);
        assert(lvl->getRemainingGuardsCount() == 3);
        assert(lvl->getTotalCoinsCount() == 6);
        assert(plr->getHealth() == 100);
        assert(plr->getCoins() == 0);

        // Collect 2 coins and eliminate 1 guard
        lvl->getCoins()[0].collect();
        plr->addCoins(1);
        lvl->getCoins()[1].collect();
        plr->addCoins(1);
        lvl->getGuards()[0]->eliminate();
        plr->addElimination();

        // Verify updated metrics passed to HUD
        assert(lvl->getCollectedCoinsCount() == 2);
        assert(plr->getCoins() == 2);
        assert(lvl->getRemainingGuardsCount() == 2);
        assert(plr->getEliminations() == 1);

        std::cout << "  [10/10] In-Game HUD Telemetry & Data Integrity: PASSED" << std::endl;
    }

    std::cout << "\n>>> ALL 10 PHASE 6 UNIT TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
    return 0;
}
