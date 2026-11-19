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
    std::cout << "[TEST] Running Phase 7 Comprehensive Campaign Integration Test Suite..." << std::endl;

    // =========================================================================
    // 1. FULL CAMPAIGN END-TO-END PLAYTHROUGH
    // =========================================================================
    {
        std::cout << "  --> Stage 1: Initial Launch & Main Menu State" << std::endl;
        GameManager gm(1024, 768, "Integration Test Engine");
        assert(gm.getCurrentState() == GameState::MENU);

        // --- Start Campaign -> Level 1 ---
        std::cout << "  --> Stage 2: Infiltration of Sector 1 (Training Ground)" << std::endl;
        gm.loadLevel(1);
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(gm.getCurrentLevelNumber() == 1);
        Level* lvl1 = gm.getCurrentLevel();
        Player* player = gm.getPlayer();

        assert(lvl1->getName() == "Training Ground");
        assert(lvl1->getTotalGuardsCount() == 2);
        assert(lvl1->getRemainingGuardsCount() == 2);
        assert(lvl1->getObstacles().size() == 5);
        assert(lvl1->getTotalCoinsCount() == 4);
        assert(player->getHealth() == 100);
        assert(player->getCoins() == 0);
        assert(player->getEliminations() == 0);

        // Collect all 4 coins in Level 1
        for (auto& coin : lvl1->getCoins()) {
            player->setPosition(coin.getPosition());
            assert(coin.checkCollision(player->getCenter(), player->getRadius()));
            coin.collect();
            player->addCoins(coin.getValue());
        }
        assert(lvl1->getCollectedCoinsCount() == 4);
        assert(player->getCoins() == 4);

        // Eliminate Guard 0 from behind
        Guard& g1_0 = *lvl1->getGuards()[0];
        g1_0.setFacingAngle(0.0f); // Facing East
        player->setPosition(g1_0.getCenter().x - 40.0f, g1_0.getCenter().y);
        assert(g1_0.isVulnerableTo(*player, 65.0f, 70.0f));
        g1_0.eliminate();
        player->addElimination();
        assert(lvl1->getRemainingGuardsCount() == 1);

        // Eliminate Guard 1 from behind
        Guard& g1_1 = *lvl1->getGuards()[1];
        g1_1.setFacingAngle(90.0f); // Facing South
        player->setPosition(g1_1.getCenter().x, g1_1.getCenter().y - 40.0f);
        assert(g1_1.isVulnerableTo(*player, 65.0f, 70.0f));
        g1_1.eliminate();
        player->addElimination();

        assert(lvl1->areAllGuardsEliminated() == true);
        assert(player->getEliminations() == 2);

        // Advance progression to Level 2
        gm.advanceToNextLevel();
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(gm.getCurrentLevelNumber() == 2);

        // --- Level 2: Restricted Facility ---
        std::cout << "  --> Stage 3: Infiltration of Sector 2 (Restricted Facility)" << std::endl;
        Level* lvl2 = gm.getCurrentLevel();
        assert(lvl2->getName() == "Restricted Facility");
        assert(lvl2->getTotalGuardsCount() == 3);
        assert(lvl2->getRemainingGuardsCount() == 3);
        assert(lvl2->getObstacles().size() == 8);
        assert(lvl2->getTotalCoinsCount() == 6);

        // Verified: Previous level coins and eliminations are safely retained!
        assert(player->getCoins() == 4);
        assert(player->getEliminations() == 2);
        assert(player->getHealth() == 100);

        // Collect coins in Level 2
        for (auto& coin : lvl2->getCoins()) {
            player->setPosition(coin.getPosition());
            coin.collect();
            player->addCoins(coin.getValue());
        }
        assert(lvl2->getCollectedCoinsCount() == 6);
        assert(player->getCoins() == 10); // 4 + 6 = 10 coins

        // Eliminate all 3 guards in Level 2
        for (size_t i = 0; i < lvl2->getGuards().size(); ++i) {
            Guard& g = *lvl2->getGuards()[i];
            g.setFacingAngle(0.0f);
            player->setPosition(g.getCenter().x - 40.0f, g.getCenter().y);
            assert(g.isVulnerableTo(*player, 65.0f, 70.0f));
            g.eliminate();
            player->addElimination();
        }
        assert(lvl2->areAllGuardsEliminated() == true);
        assert(player->getEliminations() == 5); // 2 + 3 = 5 kills

        // Advance progression to Level 3
        gm.advanceToNextLevel();
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(gm.getCurrentLevelNumber() == 3);

        // --- Level 3: Final Mission ---
        std::cout << "  --> Stage 4: Infiltration of Sector 3 (Final Mission)" << std::endl;
        Level* lvl3 = gm.getCurrentLevel();
        assert(lvl3->getName() == "Final Mission");
        assert(lvl3->getTotalGuardsCount() == 4);
        assert(lvl3->getRemainingGuardsCount() == 4);
        assert(lvl3->getObstacles().size() == 9);
        assert(lvl3->getTotalCoinsCount() == 7);

        assert(player->getCoins() == 10);
        assert(player->getEliminations() == 5);

        // Collect all 7 coins in Level 3
        for (auto& coin : lvl3->getCoins()) {
            player->setPosition(coin.getPosition());
            coin.collect();
            player->addCoins(coin.getValue());
        }
        assert(lvl3->getCollectedCoinsCount() == 7);
        assert(player->getCoins() == 17); // 10 + 7 = 17 coins total

        // Eliminate all 4 elite guards in Level 3
        for (size_t i = 0; i < lvl3->getGuards().size(); ++i) {
            Guard& g = *lvl3->getGuards()[i];
            g.setFacingAngle(0.0f);
            player->setPosition(g.getCenter().x - 40.0f, g.getCenter().y);
            assert(g.isVulnerableTo(*player, 65.0f, 70.0f));
            g.eliminate();
            player->addElimination();
        }
        assert(lvl3->areAllGuardsEliminated() == true);
        assert(player->getEliminations() == 9); // 2 + 3 + 4 = 9 total kills

        // Advance progression to Level 4
        gm.advanceToNextLevel();
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(gm.getCurrentLevelNumber() == 4);

        // --- Level 4: Underground Bunker ---
        std::cout << "  --> Stage 5: Infiltration of Sector 4 (Underground Bunker)" << std::endl;
        Level* lvl4 = gm.getCurrentLevel();
        assert(lvl4->getName() == "Underground Bunker");
        assert(lvl4->getTotalGuardsCount() == 5);
        assert(lvl4->getRemainingGuardsCount() == 5);
        assert(lvl4->getObstacles().size() == 11);
        assert(lvl4->getTotalCoinsCount() == 8);

        assert(player->getCoins() == 17);
        assert(player->getEliminations() == 9);

        // Collect all 8 coins in Level 4
        for (auto& coin : lvl4->getCoins()) {
            player->setPosition(coin.getPosition());
            coin.collect();
            player->addCoins(coin.getValue());
        }
        assert(lvl4->getCollectedCoinsCount() == 8);
        assert(player->getCoins() == 25); // 17 + 8 = 25 coins total

        // Eliminate all 5 guards in Level 4
        for (size_t i = 0; i < lvl4->getGuards().size(); ++i) {
            Guard& g = *lvl4->getGuards()[i];
            g.setFacingAngle(0.0f);
            player->setPosition(g.getCenter().x - 40.0f, g.getCenter().y);
            assert(g.isVulnerableTo(*player, 65.0f, 70.0f));
            g.eliminate();
            player->addElimination();
        }
        assert(lvl4->areAllGuardsEliminated() == true);
        assert(player->getEliminations() == 14); // 9 + 5 = 14 total kills

        // Advance progression to Level 5
        gm.advanceToNextLevel();
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(gm.getCurrentLevelNumber() == 5);

        // --- Level 5: Shadow Protocol ---
        std::cout << "  --> Stage 6: Infiltration of Sector 5 (Shadow Protocol)" << std::endl;
        Level* lvl5 = gm.getCurrentLevel();
        assert(lvl5->getName() == "Shadow Protocol");
        assert(lvl5->getTotalGuardsCount() == 6);
        assert(lvl5->getRemainingGuardsCount() == 6);
        assert(lvl5->getObstacles().size() == 12);
        assert(lvl5->getTotalCoinsCount() == 9);

        assert(player->getCoins() == 25);
        assert(player->getEliminations() == 14);

        // Collect all 9 coins in Level 5
        for (auto& coin : lvl5->getCoins()) {
            player->setPosition(coin.getPosition());
            coin.collect();
            player->addCoins(coin.getValue());
        }
        assert(lvl5->getCollectedCoinsCount() == 9);
        assert(player->getCoins() == 34); // 25 + 9 = 34 coins total

        // Eliminate all 6 elite guards in Level 5
        for (size_t i = 0; i < lvl5->getGuards().size(); ++i) {
            Guard& g = *lvl5->getGuards()[i];
            g.setFacingAngle(0.0f);
            player->setPosition(g.getCenter().x - 40.0f, g.getCenter().y);
            assert(g.isVulnerableTo(*player, 65.0f, 70.0f));
            g.eliminate();
            player->addElimination();
        }
        assert(lvl5->areAllGuardsEliminated() == true);
        assert(player->getEliminations() == 20); // 14 + 6 = 20 total kills

        // Complete final level 5 -> triggers VICTORY state
        std::cout << "  --> Stage 7: Campaign Victory Clearance Verification" << std::endl;
        gm.advanceToNextLevel();
        assert(gm.getCurrentState() == GameState::VICTORY);

        // Replay campaign -> resets to Level 1 and resets campaign score
        gm.loadLevel(1);
        assert(gm.getCurrentLevelNumber() == 1);
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(gm.getPlayer()->getCoins() == 0);
        assert(gm.getPlayer()->getEliminations() == 0);

        std::cout << "  [PASS] Full 5-Sector Campaign End-to-End Playthrough & Victory Sequence." << std::endl;
    }

    // =========================================================================
    // 2. CHECKPOINT RESTORATION & EXPLOIT PREVENTION
    // =========================================================================
    {
        std::cout << "  --> Testing Checkpoint Restoration & Anti-Farming Logic..." << std::endl;
        GameManager gm(1024, 768, "Checkpoint Test");
        gm.loadLevel(1);
        Player* player = gm.getPlayer();
        player->addCoins(4);
        player->addElimination();
        player->addElimination();

        // Advance to Level 2 (Checkpoint saved: 4 coins, 2 kills)
        gm.advanceToNextLevel();
        assert(gm.getCurrentLevelNumber() == 2);
        assert(player->getCoins() == 4);
        assert(player->getEliminations() == 2);

        // In Level 2, collect 3 coins and kill 1 guard
        Level* lvl2 = gm.getCurrentLevel();
        lvl2->getCoins()[0].collect();
        player->addCoins(1);
        lvl2->getCoins()[1].collect();
        player->addCoins(1);
        lvl2->getGuards()[0]->eliminate();
        player->addElimination();
        assert(player->getCoins() == 6);
        assert(player->getEliminations() == 3);

        // Player dies in Level 2!
        player->takeDamage(100);
        assert(!player->isAlive());

        // Player clicks Retry (restartCurrentLevel)
        gm.restartCurrentLevel();
        assert(gm.getCurrentState() == GameState::PLAYING);
        assert(player->isAlive());
        assert(player->getHealth() == 100);
        // Verified: Coins and eliminations restored to Level 2 checkpoint (4 coins, 2 kills)
        // No coin-duplication or farming exploit possible!
        assert(player->getCoins() == 4);
        assert(player->getEliminations() == 2);
        assert(lvl2->getRemainingGuardsCount() == 3);
        assert(lvl2->getCollectedCoinsCount() == 0);

        std::cout << "  [PASS] Checkpoint Restoration & Anti-Farming Safeguard." << std::endl;
    }

    // =========================================================================
    // 3. SIMULATION PAUSE INTEGRITY
    // =========================================================================
    {
        std::cout << "  --> Testing Pause Freeze & Simulation Isolation..." << std::endl;
        GameManager gm(1024, 768, "Pause Test");
        gm.loadLevel(1);
        Player* player = gm.getPlayer();
        Vector2 startPos = player->getPosition();
        Guard* g1 = gm.getCurrentLevel()->getGuards()[0].get();
        Vector2 guardStartPos = g1->getPosition();

        gm.pauseGame();
        assert(gm.getCurrentState() == GameState::PAUSED);

        // Update 100 frames while paused
        for (int i = 0; i < 100; ++i) {
            gm.update(0.016f);
        }

        // Entity positions and states must remain completely unchanged!
        assert(player->getPosition().x == startPos.x);
        assert(player->getPosition().y == startPos.y);
        assert(g1->getPosition().x == guardStartPos.x);
        assert(g1->getPosition().y == guardStartPos.y);

        gm.resumeGame();
        assert(gm.getCurrentState() == GameState::PLAYING);

        std::cout << "  [PASS] Pause Freeze & Simulation Isolation." << std::endl;
    }

    // =========================================================================
    // 4. COMBAT, DETECTION & EDGE-CASE SAFETY
    // =========================================================================
    {
        std::cout << "  --> Testing Combat Invariants & Elimination Invariance..." << std::endl;
        Level level(1);
        Guard& guard = *level.getGuards()[0];
        Player player(level.getPlayerSpawn());

        // Test A: Frontal approach cannot assassinate
        guard.setFacingAngle(0.0f); // Facing East
        player.setPosition(guard.getCenter().x + 30.0f, guard.getCenter().y); // In front of guard
        assert(!guard.isVulnerableTo(player, 65.0f, 70.0f));

        // Test B: Rear approach can assassinate
        player.setPosition(guard.getCenter().x - 30.0f, guard.getCenter().y); // Behind guard
        assert(guard.isVulnerableTo(player, 65.0f, 70.0f));

        // Test C: Elimination transitions to DEAD state
        guard.eliminate();
        assert(guard.getState() == GuardState::DEAD);
        assert(!guard.isAlive());
        assert(!guard.isActive());

        // Test D: Dead guards cannot detect, see, or attack player
        assert(guard.canSeePlayer(player, level.getObstacles()) == false);
        assert(guard.isPointInVisionSector(player.getCenter()) == false);
        assert(guard.isVulnerableTo(player, 65.0f, 70.0f) == false);

        // Test E: Dead guards cannot be assassinated again
        guard.updateAI(0.1f, player, level.getObstacles(), 1024.0f, 608.0f);
        assert(guard.getState() == GuardState::DEAD);

        // Test F: Vision occlusion by obstacles
        Guard liveGuard(300.0f, 300.0f, 16.0f, 50, 80.0f);
        liveGuard.setFacingAngle(0.0f);
        liveGuard.setVisionRadius(250.0f);
        Player distantPlayer(500.0f, 300.0f);

        Obstacle blockingWall(380.0f, 250.0f, 30.0f, 100.0f, true);
        std::vector<Obstacle> obstacles{ blockingWall };
        assert(liveGuard.canSeePlayer(distantPlayer, obstacles) == false);

        // If obstacle does not block vision, detection succeeds
        obstacles[0].setBlocksVision(false);
        assert(liveGuard.canSeePlayer(distantPlayer, obstacles) == true);

        // Test G: Map Boundary Clamping
        Vector2 extremePos{ -100.0f, 10.0f }; // Above top bar and outside left
        Vector2 clamped = CollisionManager::clampToMapBounds(extremePos, 16.0f, 0.0f, 50.0f, 1024.0f, 608.0f);
        assert(clamped.x == 16.0f);
        assert(clamped.y == 66.0f); // 50 + 16 = 66

        std::cout << "  [PASS] Combat Invariants, Vision Occlusion & Boundary Safety." << std::endl;
    }

    // =========================================================================
    // 5. CHARACTER ART & ANIMATION VERIFICATION
    // =========================================================================
    {
        std::cout << "  --> Testing Character Art & Animation Subsystem..." << std::endl;
        Player p(100.0f, 100.0f);
        assert(!p.isMoving());
        assert(p.getAnimDistance() == 0.0f);

        // Simulate movement
        p.setVelocity(Vector2{ 100.0f, 0.0f });
        assert(p.isMoving());
        p.update(0.1f);
        assert(p.getAnimDistance() > 0.0f);

        // Test safe headless drawing (CharacterRenderer fallback)
        p.draw();

        Guard g(200.0f, 200.0f);
        g.addPatrolPoint(Vector2{ 300.0f, 200.0f });
        g.addPatrolPoint(Vector2{ 400.0f, 200.0f });
        assert(g.getAnimDistance() == 0.0f);

        // Update AI and simulate movement towards waypoint
        std::vector<Obstacle> noObs;
        g.updateAI(0.1f, p, noObs, 1024.0f, 608.0f);
        assert(g.getAnimDistance() > 0.0f);

        // Test safe headless drawing
        g.draw();

        std::cout << "  [PASS] Character Art & Animation Subsystem." << std::endl;
    }

    std::cout << "\n>>> ALL PHASE 7 INTEGRATION TESTS PASSED WITH ZERO REGRESSIONS! <<<\n" << std::endl;
    return 0;
}
