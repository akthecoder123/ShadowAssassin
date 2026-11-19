#include <iostream>
#include <cassert>
#include <memory>
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "Guard.h"
#include "Obstacle.h"
#include "Collectible.h"

int main() {
    std::cout << "[TEST] Running Phase 2 Unit Tests..." << std::endl;

    // 1. Player Tests (Entity -> Player)
    Player player(100.0f, 100.0f, 16.0f, 100, 150.0f);
    assert(player.getHealth() == 100);
    assert(player.isAlive() == true);
    assert(player.getCoins() == 0);
    assert(player.getCenter().x == 100.0f);
    assert(player.getCenter().y == 100.0f);

    player.takeDamage(30);
    assert(player.getHealth() == 70);
    player.heal(15);
    assert(player.getHealth() == 85);
    player.addCoins(5);
    assert(player.getCoins() == 5);
    std::cout << "  - Player health & economy: PASSED" << std::endl;

    // 2. Guard Tests (Entity -> Enemy -> Guard)
    Guard guard(200.0f, 100.0f, 16.0f, 50, 80.0f);
    assert(guard.getState() == GuardState::PATROLLING);
    assert(guard.getHealth() == 50);
    assert(guard.isAlive() == true);

    guard.setState(GuardState::ALERTED);
    assert(guard.getState() == GuardState::ALERTED);

    guard.setState(GuardState::CHASING);
    assert(guard.getState() == GuardState::CHASING);

    guard.addPatrolPoint(200.0f, 100.0f);
    guard.addPatrolPoint(200.0f, 300.0f);
    assert(guard.getPatrolPoints().size() == 2);

    // Vision cone test
    guard.setFacingAngle(180.0f); // Facing left toward (100, 100)
    guard.setVisionRadius(150.0f);
    guard.setVisionAngle(60.0f);
    assert(guard.isPointInVisionSector(Vector2{ 100.0f, 100.0f }) == true);
    assert(guard.isPointInVisionSector(Vector2{ 200.0f, 400.0f }) == false); // Out of range and angle

    guard.eliminate();
    assert(guard.getState() == GuardState::DEAD);
    assert(guard.isAlive() == false);
    assert(guard.isPointInVisionSector(Vector2{ 100.0f, 100.0f }) == false);
    std::cout << "  - Guard FSM & Vision Cone: PASSED" << std::endl;

    // 3. Runtime Polymorphism Test
    std::vector<std::unique_ptr<Enemy>> enemyList;
    enemyList.push_back(std::make_unique<Guard>(300.0f, 300.0f));
    assert(enemyList[0]->getHealth() == 50);
    enemyList[0]->onAlerted(Vector2{ 100.0f, 100.0f });
    Guard* polyGuard = dynamic_cast<Guard*>(enemyList[0].get());
    assert(polyGuard != nullptr);
    assert(polyGuard->getState() == GuardState::ALERTED);
    std::cout << "  - Runtime Polymorphism (Enemy* -> Guard): PASSED" << std::endl;

    // 4. Obstacle Tests
    Obstacle wall(Rectangle{ 250.0f, 50.0f, 20.0f, 200.0f });
    assert(wall.contains(Vector2{ 260.0f, 100.0f }) == true);
    assert(wall.contains(Vector2{ 100.0f, 100.0f }) == false);
    assert(wall.collidesWithCircle(Vector2{ 245.0f, 100.0f }, 10.0f) == true);
    assert(wall.collidesWithCircle(Vector2{ 200.0f, 100.0f }, 10.0f) == false);
    std::cout << "  - Obstacle Collision interfaces: PASSED" << std::endl;

    // 5. Collectible Tests
    Collectible coin(150.0f, 100.0f, 10.0f, 2);
    assert(coin.isCollected() == false);
    assert(coin.checkCollision(Vector2{ 150.0f, 100.0f }, 15.0f) == true);
    assert(coin.checkCollision(Vector2{ 300.0f, 300.0f }, 15.0f) == false);
    coin.collect();
    assert(coin.isCollected() == true);
    assert(coin.checkCollision(Vector2{ 150.0f, 100.0f }, 15.0f) == false);
    coin.reset();
    assert(coin.isCollected() == false);
    std::cout << "  - Collectible Lifecycle: PASSED" << std::endl;

    std::cout << "ALL PHASE 2 UNIT TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
