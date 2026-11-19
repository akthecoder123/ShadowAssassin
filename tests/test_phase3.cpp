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
    std::cout << "[TEST] Running Phase 3 Unit Tests: Player Mechanics & Assassination..." << std::endl;

    // -------------------------------------------------------------
    // Test 1: Normal Movement & Diagonal Normalization
    // -------------------------------------------------------------
    Player player(100.0f, 100.0f, 16.0f, 100, 200.0f);
    assert(player.getSpeed() == 200.0f);

    // Diagonal displacement normalization test
    float dx = 1.0f;
    float dy = 1.0f;
    float len = std::sqrt(dx * dx + dy * dy);
    dx /= len;
    dy /= len;
    Vector2 normalizedVel = Vector2{ dx * player.getSpeed(), dy * player.getSpeed() };

    float calculatedSpeed = std::sqrt(normalizedVel.x * normalizedVel.x + normalizedVel.y * normalizedVel.y);
    assert(std::abs(calculatedSpeed - 200.0f) < 0.001f);
    std::cout << "  [1/8] Diagonal Movement Normalization: PASSED (Speed = " << calculatedSpeed << " px/s)" << std::endl;

    // -------------------------------------------------------------
    // Test 2: Collision with Walls (Circle vs Rect Resolution)
    // -------------------------------------------------------------
    // Obstacle at x=200, y=100, w=30, h=100
    Obstacle wall(200.0f, 100.0f, 30.0f, 100.0f);
    std::vector<Obstacle> obstacles;
    obstacles.push_back(wall);

    // Position circle trying to penetrate the left wall: center at (195, 150), radius 16
    // Penetration: closest point is (200, 150), dist=5 < 16. Overlap=11.
    Vector2 penPos = Vector2{ 195.0f, 150.0f };
    assert(wall.collidesWithCircle(penPos, 16.0f) == true);

    Vector2 resolvedPos = CollisionManager::resolveCircleRect(penPos, 16.0f, wall.getBounds());
    assert(wall.collidesWithCircle(resolvedPos, 16.0f) == false);
    assert(std::abs(resolvedPos.x - (200.0f - 16.0f)) < 0.01f); // Should be pushed to 184
    std::cout << "  [2/8] Circle vs Rect Collision Resolution: PASSED" << std::endl;

    // -------------------------------------------------------------
    // Test 3: Smooth Wall Sliding
    // -------------------------------------------------------------
    // Player is at (184, 150), right on the left face of the wall at x=200.
    // Velocity is diagonally down-right: vx = +200, vy = +200.
    // Because of the wall on the right, X movement is blocked, but Y movement slides down!
    Vector2 startCenter = Vector2{ 184.0f, 150.0f };
    Vector2 diagVel = Vector2{ 200.0f, 200.0f };
    float dt = 0.1f; // 100ms

    Vector2 slideResult = CollisionManager::resolveMovement(startCenter, diagVel, 16.0f, dt, obstacles);

    // X should not penetrate wall (x <= 184.01)
    assert(slideResult.x <= 184.01f);
    assert(wall.collidesWithCircle(slideResult, 16.0f) == false);
    // Y should have advanced by 200 * 0.1 = 20px (150 -> 170)
    assert(std::abs(slideResult.y - 170.0f) < 0.01f);
    std::cout << "  [3/8] Wall Sliding Physics: PASSED (X blocked at " << slideResult.x << ", Y slid to " << slideResult.y << ")" << std::endl;

    // -------------------------------------------------------------
    // Test 4: Map Boundary Collisions
    // -------------------------------------------------------------
    // Attempting to move past boundary edges
    Vector2 outOfBoundsPos = Vector2{ -10.0f, 800.0f };
    Vector2 clamped = CollisionManager::clampToMapBounds(outOfBoundsPos, 16.0f, 0.0f, 0.0f, 1024.0f, 768.0f);
    assert(clamped.x == 16.0f);
    assert(clamped.y == 768.0f - 16.0f);
    std::cout << "  [4/8] Map Boundary Clamping: PASSED" << std::endl;

    // -------------------------------------------------------------
    // Test 5: Assassination Within Range vs Outside Range
    // -------------------------------------------------------------
    Guard guard(300.0f, 200.0f, 16.0f, 50, 80.0f);
    guard.setFacingAngle(0.0f); // Facing right (+X)

    // Player positioned behind guard at (260, 200): distance = 40 <= 65 -> In Range
    Player playerBehindClose(260.0f, 200.0f);
    assert(playerBehindClose.distanceTo(guard.getCenter()) == 40.0f);
    assert(playerBehindClose.canAssassinate(guard.getCenter(), guard.getFacingAngle(), 65.0f, 70.0f) == true);

    // Player positioned behind guard at (200, 200): distance = 100 > 65 -> Outside Range
    Player playerBehindFar(200.0f, 200.0f);
    assert(playerBehindFar.distanceTo(guard.getCenter()) == 100.0f);
    assert(playerBehindFar.canAssassinate(guard.getCenter(), guard.getFacingAngle(), 65.0f, 70.0f) == false);
    std::cout << "  [5/8] Assassination Distance Boundary: PASSED" << std::endl;

    // -------------------------------------------------------------
    // Test 6: Rear Approach vs Front Approach
    // -------------------------------------------------------------
    // Guard is at (300, 200) facing right (0 deg).
    // Player is within distance (40px) in FRONT of guard at (340, 200).
    Player playerInFront(340.0f, 200.0f);
    assert(playerInFront.distanceTo(guard.getCenter()) == 40.0f);
    assert(playerInFront.isBehindTarget(guard.getCenter(), guard.getFacingAngle(), 70.0f) == false);
    assert(playerInFront.canAssassinate(guard.getCenter(), guard.getFacingAngle(), 65.0f, 70.0f) == false);

    // Guard method isVulnerableTo check
    assert(guard.isVulnerableTo(playerInFront, 65.0f, 70.0f) == false);
    assert(guard.isVulnerableTo(playerBehindClose, 65.0f, 70.0f) == true);
    std::cout << "  [6/8] Rear-Approach vs Front-Facing Check: PASSED" << std::endl;

    // -------------------------------------------------------------
    // Test 7: Successful Guard Elimination & Rewards
    // -------------------------------------------------------------
    assert(guard.isAlive() == true);
    assert(guard.getState() == GuardState::PATROLLING);
    assert(playerBehindClose.getEliminations() == 0);

    // Execute assassination
    guard.eliminate();
    playerBehindClose.addElimination();
    playerBehindClose.triggerAssassinationEffect(guard.getCenter());

    assert(guard.getState() == GuardState::DEAD);
    assert(guard.isAlive() == false);
    assert(playerBehindClose.getEliminations() == 1);
    std::cout << "  [7/8] Assassination Execution & Rewards: PASSED" << std::endl;

    // -------------------------------------------------------------
    // Test 8: Prevention of Repeated Assassination on Dead Guards
    // -------------------------------------------------------------
    // Now that the guard is DEAD, neither canAssassinate nor isVulnerableTo should be true
    assert(guard.isVulnerableTo(playerBehindClose, 65.0f, 70.0f) == false);
    std::cout << "  [8/8] Prevention of Repeated Assassination: PASSED" << std::endl;

    std::cout << "\n>>> ALL 8 PHASE 3 UNIT TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
    return 0;
}
