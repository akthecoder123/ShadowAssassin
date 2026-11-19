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

int main() {
    std::cout << "[TEST] Running Phase 4 Unit Tests: Guard AI System & Vision Raycasting..." << std::endl;

    // -------------------------------------------------------------
    // Test 1: Patrol Waypoint Progression
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 100.0f, 16.0f, 50, 100.0f);
        guard.addPatrolPoint(100.0f, 100.0f);
        guard.addPatrolPoint(200.0f, 100.0f);
        guard.addPatrolPoint(200.0f, 200.0f);

        assert(guard.getPatrolPoints().size() == 3);
        assert(guard.getCurrentWaypointIndex() == 0);

        std::vector<Obstacle> obstacles;
        Player dummyPlayer(500.0f, 500.0f); // Out of sight

        // Guard at waypoint 0 (100, 100) -> distance to pt 0 is 0 <= 6.0px
        // updatePatrol advances to waypoint 1 and sets pause timer
        guard.updateAI(0.016f, dummyPlayer, obstacles, 1024.0f, 768.0f);
        assert(guard.getCurrentWaypointIndex() == 1);

        std::cout << "  [1/14] Patrol Waypoint Progression: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 2: Guard Facing Direction Towards Movement
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 100.0f, 16.0f, 50, 100.0f);
        guard.addPatrolPoint(200.0f, 100.0f); // Directly to the right
        guard.setCurrentWaypointIndex(0);

        std::vector<Obstacle> obstacles;
        Player dummyPlayer(800.0f, 800.0f);

        // Move 1 frame towards (200, 100)
        guard.updateAI(0.05f, dummyPlayer, obstacles, 1024.0f, 768.0f);
        // Facing angle should be 0 degrees (+X direction)
        assert(std::abs(guard.getFacingAngle() - 0.0f) < 0.1f);

        // Clear and add waypoint downwards
        guard.clearPatrolPoints();
        guard.addPatrolPoint(guard.getCenter().x, guard.getCenter().y + 100.0f);
        guard.setCurrentWaypointIndex(0);
        guard.updateAI(0.05f, dummyPlayer, obstacles, 1024.0f, 768.0f);
        // Facing angle should be 90 degrees (+Y direction)
        assert(std::abs(guard.getFacingAngle() - 90.0f) < 0.1f);

        std::cout << "  [2/14] Guard Facing Direction Orientation: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 3: Detection Within Vision Radius & Sector Angle
    // -------------------------------------------------------------
    {
        Guard guard(200.0f, 200.0f, 16.0f, 50, 80.0f);
        guard.setFacingAngle(0.0f); // Facing right (+X)
        guard.setVisionRadius(200.0f);
        guard.setVisionAngle(60.0f); // +/- 30 degrees

        // Target at (300, 200) -> 100px away, angle 0 deg -> Inside cone
        assert(guard.isPointInVisionSector(Vector2{ 300.0f, 200.0f }) == true);

        // Player directly in front
        Player playerInCone(300.0f, 200.0f);
        std::vector<Obstacle> emptyObstacles;
        assert(guard.canSeePlayer(playerInCone, emptyObstacles) == true);

        std::cout << "  [3/14] Detection Within Vision Radius & Angle: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 4: Detection Outside Vision Radius
    // -------------------------------------------------------------
    {
        Guard guard(200.0f, 200.0f, 16.0f, 50, 80.0f);
        guard.setFacingAngle(0.0f);
        guard.setVisionRadius(150.0f);
        guard.setVisionAngle(60.0f);

        // Player at (400, 200) -> 200px away > 150px vision radius
        Player playerFar(400.0f, 200.0f);
        std::vector<Obstacle> obstacles;
        assert(guard.isPointInVisionSector(playerFar.getCenter()) == false);
        assert(guard.canSeePlayer(playerFar, obstacles) == false);

        std::cout << "  [4/14] Detection Outside Vision Radius Rejection: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 5: Detection Outside Vision Angle
    // -------------------------------------------------------------
    {
        Guard guard(200.0f, 200.0f, 16.0f, 50, 80.0f);
        guard.setFacingAngle(0.0f); // Facing right (+X)
        guard.setVisionRadius(200.0f);
        guard.setVisionAngle(60.0f); // Half angle = 30 deg

        // Player behind guard at (100, 200) -> 180 deg
        Player playerBehind(100.0f, 200.0f);
        std::vector<Obstacle> obstacles;
        assert(guard.isPointInVisionSector(playerBehind.getCenter()) == false);
        assert(guard.canSeePlayer(playerBehind, obstacles) == false);

        // Player to the side at 45 deg: guard facing 0 deg, player at (250, 250) (45 deg > 30 deg half-angle)
        Player playerFlank(250.0f, 250.0f);
        assert(guard.isPointInVisionSector(playerFlank.getCenter()) == false);
        assert(guard.canSeePlayer(playerFlank, obstacles) == false);

        std::cout << "  [5/14] Detection Outside Vision Angle Rejection: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 6: Vision Blocked by Obstacle (Line of Sight Raycast)
    // -------------------------------------------------------------
    {
        Guard guard(200.0f, 200.0f, 16.0f, 50, 80.0f);
        guard.setFacingAngle(0.0f);
        guard.setVisionRadius(300.0f);
        guard.setVisionAngle(90.0f);

        Player playerAhead(400.0f, 200.0f);

        // Obstacle directly in line of sight: x=280..300, y=170..230
        Obstacle wall(280.0f, 170.0f, 20.0f, 60.0f, true); // blocksVision = true
        std::vector<Obstacle> obstacles;
        obstacles.push_back(wall);

        // Line from (200, 200) to (400, 200) intersects wall at x=280
        assert(CollisionManager::checkLineOfSight(guard.getCenter(), playerAhead.getCenter(), obstacles) == false);
        assert(guard.canSeePlayer(playerAhead, obstacles) == false);

        // If obstacle doesn't block vision (e.g. low decorative prop)
        obstacles[0].setBlocksVision(false);
        assert(CollisionManager::checkLineOfSight(guard.getCenter(), playerAhead.getCenter(), obstacles) == true);
        assert(guard.canSeePlayer(playerAhead, obstacles) == true);

        std::cout << "  [6/14] Line of Sight Raycast & Obstacle Occlusion: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 7: State Transition: PATROLLING to ALERTED
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 200.0f, 16.0f, 50, 80.0f);
        guard.setFacingAngle(0.0f);
        guard.setVisionRadius(250.0f);
        assert(guard.getState() == GuardState::PATROLLING);

        Player player(200.0f, 200.0f); // In front and visible
        std::vector<Obstacle> obstacles;

        guard.updateAI(0.016f, player, obstacles, 1024.0f, 768.0f);
        assert(guard.getState() == GuardState::ALERTED);

        std::cout << "  [7/14] State Transition PATROLLING -> ALERTED: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 8: State Transition: ALERTED to CHASING After Timer
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 200.0f, 16.0f, 50, 80.0f);
        guard.setState(GuardState::ALERTED);

        Player player(200.0f, 200.0f);
        std::vector<Obstacle> obstacles;

        // Alert duration is 0.6s. Advance by 0.3s -> should still be ALERTED
        guard.updateAI(0.3f, player, obstacles, 1024.0f, 768.0f);
        assert(guard.getState() == GuardState::ALERTED);

        // Advance past 0.6s total (0.3s + 0.35s = 0.65s) -> should transition to CHASING
        guard.updateAI(0.35f, player, obstacles, 1024.0f, 768.0f);
        assert(guard.getState() == GuardState::CHASING);

        std::cout << "  [8/14] State Transition ALERTED -> CHASING on Timer: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 9: Chasing Player Towards Player Position
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 200.0f, 16.0f, 50, 100.0f);
        guard.setState(GuardState::CHASING);

        Player player(300.0f, 200.0f);
        std::vector<Obstacle> obstacles;

        float initialDist = guard.distanceTo(player.getCenter());
        guard.updateAI(0.1f, player, obstacles, 1024.0f, 768.0f);
        float newDist = guard.distanceTo(player.getCenter());

        // Guard should have closed distance towards player
        assert(newDist < initialDist);
        assert(guard.getState() == GuardState::CHASING);

        std::cout << "  [9/14] Chasing Mechanics & Pursuit Movement: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 10: Search at Last Known Position When Player Sight is Lost
    // -------------------------------------------------------------
    {
        Guard guard(200.0f, 200.0f, 16.0f, 50, 100.0f);
        guard.setState(GuardState::CHASING);

        // Place player behind a wall
        Obstacle wall(250.0f, 150.0f, 20.0f, 100.0f, true);
        std::vector<Obstacle> obstacles;
        obstacles.push_back(wall);

        Player hiddenPlayer(400.0f, 200.0f);
        // Guard cannot see hidden player
        assert(guard.canSeePlayer(hiddenPlayer, obstacles) == false);

        // Guard last known pos set to (210, 200) close to current position
        // Guard reaches last known pos (dist <= 16px)
        guard.onAlerted(Vector2{ 205.0f, 200.0f });
        guard.setState(GuardState::CHASING);

        float initialAngle = guard.getFacingAngle();
        // Update: guard is at last known position and cannot see player -> searches (rotates)
        guard.updateAI(0.2f, hiddenPlayer, obstacles, 1024.0f, 768.0f);

        // Guard is searching (facing angle rotated looking around)
        assert(guard.getState() == GuardState::CHASING);
        assert(guard.getFacingAngle() != initialAngle);

        std::cout << "  [10/14] Search Mode at Last Known Position: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 11: Return to PATROLLING After Search Timeout
    // -------------------------------------------------------------
    {
        Guard guard(200.0f, 200.0f, 16.0f, 50, 100.0f);
        guard.onAlerted(Vector2{ 200.0f, 200.0f }); // Guard is already at last known pos
        guard.setState(GuardState::CHASING);

        Obstacle wall(250.0f, 150.0f, 20.0f, 100.0f, true);
        std::vector<Obstacle> obstacles;
        obstacles.push_back(wall);

        Player hiddenPlayer(500.0f, 200.0f);

        // Search duration is 2.2s. Advance past 2.2s in increments
        for (int i = 0; i < 25; ++i) {
            guard.updateAI(0.1f, hiddenPlayer, obstacles, 1024.0f, 768.0f);
        }

        // After searching for > 2.2s without sight of player, guard returns to PATROLLING
        assert(guard.getState() == GuardState::PATROLLING);

        std::cout << "  [11/14] Return to PATROLLING After Search Timeout: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 12: Attack Cooldown Prevents Damage Every Frame
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 100.0f, 16.0f, 50, 100.0f);
        guard.setAttackDistance(35.0f);
        guard.setAttackDamage(15);
        guard.setState(GuardState::ATTACKING);

        Player player(120.0f, 100.0f, 16.0f, 100); // 20px away, within 35px attack range
        std::vector<Obstacle> obstacles;

        // Frame 1: Attack strikes (deals 15 dmg, health becomes 85)
        guard.updateAI(0.016f, player, obstacles, 1024.0f, 768.0f);
        assert(player.getHealth() == 85);
        assert(guard.getAttackCooldown() > 0.0f);

        // Frame 2 (0.016s later): Still in cooldown -> no damage dealt!
        guard.updateAI(0.016f, player, obstacles, 1024.0f, 768.0f);
        assert(player.getHealth() == 85);

        std::cout << "  [12/14] Attack Cooldown Throttling: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 13: Player Health Reduction and Attack Engagement
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 100.0f, 16.0f, 50, 100.0f);
        guard.setAttackDistance(40.0f);
        guard.setAttackDamage(25);
        guard.setState(GuardState::CHASING);

        Player player(120.0f, 100.0f, 16.0f, 100);
        std::vector<Obstacle> obstacles;

        // Within attack distance -> switches to ATTACKING
        guard.updateAI(0.016f, player, obstacles, 1024.0f, 768.0f);
        assert(guard.getState() == GuardState::ATTACKING);

        // Attack strikes
        guard.updateAI(0.016f, player, obstacles, 1024.0f, 768.0f);
        assert(player.getHealth() == 75);

        std::cout << "  [13/14] Combat Damage Application: PASSED" << std::endl;
    }

    // -------------------------------------------------------------
    // Test 14: Dead Guards Cannot Detect or Attack
    // -------------------------------------------------------------
    {
        Guard guard(100.0f, 100.0f, 16.0f, 50, 100.0f);
        guard.eliminate();

        assert(guard.getState() == GuardState::DEAD);
        assert(guard.isAlive() == false);
        assert(guard.isActive() == false);

        Player player(105.0f, 100.0f);
        std::vector<Obstacle> obstacles;

        // Dead guard cannot see or detect player
        assert(guard.canSeePlayer(player, obstacles) == false);
        assert(guard.isPointInVisionSector(player.getCenter()) == false);

        // Calling updateAI on dead guard does nothing
        guard.updateAI(0.1f, player, obstacles, 1024.0f, 768.0f);
        assert(guard.getState() == GuardState::DEAD);
        assert(player.getHealth() == 100); // Unharmed

        std::cout << "  [14/14] Dead Guard Inactivity & Neutralization: PASSED" << std::endl;
    }

    std::cout << "\n>>> ALL 14 PHASE 4 UNIT TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
    return 0;
}
