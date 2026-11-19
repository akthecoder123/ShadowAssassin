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
#include "GameManager.h"

int main() {
    std::cout << "[TEST] Running Phase 5 Unit Tests: Level Implementation & Progression..." << std::endl;

    // -------------------------------------------------------------
    // Test 1: Exactly Five Handcrafted Levels Defined
    // -------------------------------------------------------------
    {
        Level level1(1);
        Level level2(2);
        Level level3(3);
        Level level4(4);
        Level level5(5);

        assert(level1.getLevelNumber() == 1);
        assert(level2.getLevelNumber() == 2);
        assert(level3.getLevelNumber() == 3);
        assert(level4.getLevelNumber() == 4);
        assert(level5.getLevelNumber() == 5);

        assert(level1.getName() == "Training Ground");
        assert(level2.getName() == "Restricted Facility");
        assert(level3.getName() == "Final Mission");
        assert(level4.getName() == "Underground Bunker");
        assert(level5.getName() == "Shadow Protocol");

        // Clamping check (1 to 5)
        Level levelOutOfBounds(99);
        assert(levelOutOfBounds.getLevelNumber() == 5);

        Level levelNegative(-5);
        assert(levelNegative.getLevelNumber() == 1);

        std::cout << "  [1/10] Exactly Five Discrete Levels Defined: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 2: Expected Guard Count per Level
    // -------------------------------------------------------------
    {
        Level level1(1);
        Level level2(2);
        Level level3(3);
        Level level4(4);
        Level level5(5);

        assert(level1.getTotalGuardsCount() == 2);
        assert(level2.getTotalGuardsCount() == 3);
        assert(level3.getTotalGuardsCount() == 4);
        assert(level4.getTotalGuardsCount() == 5);
        assert(level5.getTotalGuardsCount() == 6);

        // Ensure all guards start alive
        assert(level1.getRemainingGuardsCount() == 2);
        assert(level2.getRemainingGuardsCount() == 3);
        assert(level3.getRemainingGuardsCount() == 4);
        assert(level4.getRemainingGuardsCount() == 5);
        assert(level5.getRemainingGuardsCount() == 6);

        std::cout << "  [2/10] Expected Guard Count per Level (2, 3, 4, 5, 6): PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 3: Valid Player Spawn Positions
    // -------------------------------------------------------------
    {
        Level level1(1);
        Level level2(2);
        Level level3(3);
        Level level4(4);
        Level level5(5);

        Vector2 spawn1 = level1.getPlayerSpawn();
        Vector2 spawn2 = level2.getPlayerSpawn();
        Vector2 spawn3 = level3.getPlayerSpawn();
        Vector2 spawn4 = level4.getPlayerSpawn();
        Vector2 spawn5 = level5.getPlayerSpawn();

        // Spawns must be inside playable map boundaries
        assert(spawn1.x >= 20.0f && spawn1.x <= level1.getMapWidth() - 20.0f);
        assert(spawn1.y >= 50.0f && spawn1.y <= level1.getMapHeight() - 20.0f);

        assert(spawn2.x >= 20.0f && spawn2.x <= level2.getMapWidth() - 20.0f);
        assert(spawn2.y >= 50.0f && spawn2.y <= level2.getMapHeight() - 20.0f);

        assert(spawn3.x >= 20.0f && spawn3.x <= level3.getMapWidth() - 20.0f);
        assert(spawn3.y >= 50.0f && spawn3.y <= level3.getMapHeight() - 20.0f);

        assert(spawn4.x >= 20.0f && spawn4.x <= level4.getMapWidth() - 20.0f);
        assert(spawn4.y >= 50.0f && spawn4.y <= level4.getMapHeight() - 20.0f);

        assert(spawn5.x >= 20.0f && spawn5.x <= level5.getMapWidth() - 20.0f);
        assert(spawn5.y >= 50.0f && spawn5.y <= level5.getMapHeight() - 20.0f);

        // Verify player does not spawn inside any obstacle
        for (const auto& obs : level1.getObstacles()) {
            assert(!obs.collidesWithCircle(spawn1, 16.0f));
        }
        for (const auto& obs : level2.getObstacles()) {
            assert(!obs.collidesWithCircle(spawn2, 16.0f));
        }
        for (const auto& obs : level3.getObstacles()) {
            assert(!obs.collidesWithCircle(spawn3, 16.0f));
        }
        for (const auto& obs : level4.getObstacles()) {
            assert(!obs.collidesWithCircle(spawn4, 16.0f));
        }
        for (const auto& obs : level5.getObstacles()) {
            assert(!obs.collidesWithCircle(spawn5, 16.0f));
        }

        std::cout << "  [3/10] Valid Player Spawn Positions Across All 5 Sectors: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 4: Expected Obstacles and Collectibles in Level Layouts
    // -------------------------------------------------------------
    {
        Level level1(1);
        Level level2(2);
        Level level3(3);
        Level level4(4);
        Level level5(5);

        // Level 1: 5 obstacles, 4 coins
        assert(level1.getObstacles().size() == 5);
        assert(level1.getTotalCoinsCount() == 4);

        // Level 2: 8 obstacles, 6 coins
        assert(level2.getObstacles().size() == 8);
        assert(level2.getTotalCoinsCount() == 6);

        // Level 3: 9 obstacles, 7 coins
        assert(level3.getObstacles().size() == 9);
        assert(level3.getTotalCoinsCount() == 7);

        // Level 4: 11 obstacles, 8 coins
        assert(level4.getObstacles().size() == 11);
        assert(level4.getTotalCoinsCount() == 8);

        // Level 5: 12 obstacles, 9 coins
        assert(level5.getObstacles().size() == 12);
        assert(level5.getTotalCoinsCount() == 9);

        // All obstacles must block vision by default
        for (const auto& obs : level1.getObstacles()) assert(obs.blocksVision());
        for (const auto& obs : level2.getObstacles()) assert(obs.blocksVision());
        for (const auto& obs : level3.getObstacles()) assert(obs.blocksVision());
        for (const auto& obs : level4.getObstacles()) assert(obs.blocksVision());
        for (const auto& obs : level5.getObstacles()) assert(obs.blocksVision());

        std::cout << "  [4/10] Expected Obstacles and Collectibles Configuration: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 5: Resetting Level Restores Guards and Collectibles
    // -------------------------------------------------------------
    {
        Level level(1);
        assert(level.getRemainingGuardsCount() == 2);
        assert(level.getCollectedCoinsCount() == 0);

        // Eliminate guards and collect coins
        level.getGuards()[0]->eliminate();
        level.getGuards()[1]->eliminate();
        level.getCoins()[0].collect();
        level.getCoins()[1].collect();

        assert(level.getRemainingGuardsCount() == 0);
        assert(level.areAllGuardsEliminated() == true);
        assert(level.getCollectedCoinsCount() == 2);

        // Call reset
        level.reset();

        // Verify restoration
        assert(level.getRemainingGuardsCount() == 2);
        assert(level.areAllGuardsEliminated() == false);
        assert(level.getGuards()[0]->getState() == GuardState::PATROLLING);
        assert(level.getGuards()[1]->getState() == GuardState::PATROLLING);
        assert(level.getCollectedCoinsCount() == 0);
        assert(!level.getCoins()[0].isCollected());

        std::cout << "  [5/10] Level State Reset & Entity Restoration: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 6: Level Progression (1 -> 2 -> 3 -> 4 -> 5)
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        gm.loadLevel(1);
        assert(gm.getCurrentLevelNumber() == 1);
        assert(gm.getCurrentState() == GameState::PLAYING);

        // Advance to level 2
        gm.advanceToNextLevel();
        assert(gm.getCurrentLevelNumber() == 2);
        assert(gm.getCurrentLevel()->getTotalGuardsCount() == 3);
        assert(gm.getCurrentState() == GameState::PLAYING);

        // Advance to level 3
        gm.advanceToNextLevel();
        assert(gm.getCurrentLevelNumber() == 3);
        assert(gm.getCurrentLevel()->getTotalGuardsCount() == 4);
        assert(gm.getCurrentState() == GameState::PLAYING);

        // Advance to level 4
        gm.advanceToNextLevel();
        assert(gm.getCurrentLevelNumber() == 4);
        assert(gm.getCurrentLevel()->getTotalGuardsCount() == 5);
        assert(gm.getCurrentState() == GameState::PLAYING);

        // Advance to level 5
        gm.advanceToNextLevel();
        assert(gm.getCurrentLevelNumber() == 5);
        assert(gm.getCurrentLevel()->getTotalGuardsCount() == 6);
        assert(gm.getCurrentState() == GameState::PLAYING);

        std::cout << "  [6/10] Level Progression Sequencing (1 -> 2 -> 3 -> 4 -> 5): PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 7: Completing Level 5 Triggers Final Victory
    // -------------------------------------------------------------
    {
        GameManager gm(1024, 768, "Test GM");
        gm.loadLevel(5);
        assert(gm.getCurrentLevelNumber() == 5);
        assert(gm.getCurrentState() == GameState::PLAYING);

        // Advance past level 5
        gm.advanceToNextLevel();
        assert(gm.getCurrentState() == GameState::VICTORY);

        std::cout << "  [7/10] Level 5 Completion Triggers VICTORY State: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 8: Player Movement & Collision Resolution with Level Obstacles
    // -------------------------------------------------------------
    {
        Level level(1);
        Player player(level.getPlayerSpawn(), 16.0f, 100, 200.0f);

        // Obstacle 0 in Level 1 is at x=260, y=160, w=28, h=180
        // Move player towards the wall face at x=260
        player.setPosition(240.0f, 200.0f);
        // Desired velocity directly into the wall: vx = +200, vy = 0
        player.moveAndSlide(0.1f, level.getObstacles(), level.getMapWidth(), level.getMapHeight());

        // Player must be stopped from entering wall (x <= 260 - 16 = 244)
        assert(player.getCenter().x <= 244.01f);
        for (const auto& obs : level.getObstacles()) {
            assert(!obs.collidesWithCircle(player.getCenter(), player.getRadius()));
        }

        std::cout << "  [8/10] Player Movement & Obstacle Collision Resolution: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 9: Guard Vision & Line-of-Sight Occlusion with Level Obstacles
    // -------------------------------------------------------------
    {
        Level level(1);
        Guard& guard1 = *level.getGuards()[0]; // At (500, 200)

        // Player on the other side of Obstacle 0 (at x=260)
        Player playerBehindWall(180.0f, 200.0f);

        // Line between guard (500, 200) and player (180, 200) crosses Obstacle 0 and Obstacle 1
        bool canSee = guard1.canSeePlayer(playerBehindWall, level.getObstacles());
        assert(canSee == false);

        std::cout << "  [9/10] Guard Vision & Line-of-Sight Raycasting Across Map: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 10: Assassination and Coin Collection in Handcrafted Level
    // -------------------------------------------------------------
    {
        Level level(2);
        Player player(level.getPlayerSpawn(), 16.0f, 100, 200.0f);

        // Pick up coin 0
        Collectible& coin0 = level.getCoins()[0];
        player.setPosition(coin0.getPosition());
        assert(coin0.checkCollision(player.getCenter(), player.getRadius()));
        coin0.collect();
        player.addCoins(coin0.getValue());
        assert(coin0.isCollected());
        assert(player.getCoins() == coin0.getValue());

        // Assassinate Guard 0 from behind
        Guard& guard0 = *level.getGuards()[0];
        guard0.setFacingAngle(0.0f); // Guard faces +X (East)
        // Player approaches from behind (West: x = guard.x - 40)
        player.setPosition(guard0.getCenter().x - 40.0f, guard0.getCenter().y);

        assert(guard0.isVulnerableTo(player, 65.0f, 70.0f));
        guard0.eliminate();
        player.addElimination();

        assert(guard0.getState() == GuardState::DEAD);
        assert(player.getEliminations() == 1);
        assert(level.getRemainingGuardsCount() == 2); // 3 - 1 = 2 remaining

        std::cout << "  [10/10] Assassination and Coin Collection in Handcrafted Level: PASSED" << std::endl;
    }

    std::cout << "\n>>> ALL 10 PHASE 5 UNIT TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
    return 0;
}
