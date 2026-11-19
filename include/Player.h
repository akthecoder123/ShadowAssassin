#pragma once

#include "Entity.h"
#include "Obstacle.h"
#include <vector>

/**
 * @brief Represents the player operative controlled by the user.
 * 
 * Demonstrates:
 * - Inheritance: Extends abstract base class Entity.
 * - Encapsulation: Health, speed, facing angle, velocity, and economy are private.
 * - Polymorphism: Overrides virtual update() and draw().
 * - Single Responsibility: Manages operative state, input parsing, and combat interactions.
 */
class Player : public Entity {
public:
    Player(Vector2 position, float radius = 16.0f, int maxHealth = 100, float speed = 190.0f);
    Player(float x, float y, float radius = 16.0f, int maxHealth = 100, float speed = 190.0f);
    ~Player() override = default;

    // Entity polymorphic overrides
    void update(float deltaTime) override;
    void draw() const override;

    // Movement & Input
    void handleInput();
    void setVelocity(Vector2 velocity) { m_velocity = velocity; }
    Vector2 getVelocity() const { return m_velocity; }
    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }
    float getFacingAngle() const { return m_facingAngle; }
    void setFacingAngle(float degrees) { m_facingAngle = degrees; }
    float getRadius() const { return m_size.x * 0.5f; }

    // Collision-aware movement resolution
    void moveAndSlide(float deltaTime, const std::vector<Obstacle>& obstacles, float mapWidth, float mapHeight);

    // Health & Combat Management
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    bool isAlive() const { return m_health > 0; }
    void takeDamage(int amount);
    void heal(int amount);
    void resetHealth();
    bool isHitFlashing() const { return m_hitFlashTimer > 0.0f; }

    // Economy & Score
    int getCoins() const { return m_coinCount; }
    void addCoins(int amount = 1);
    void setCoins(int amount) { m_coinCount = std::max(0, amount); }
    int getEliminations() const { return m_eliminationCount; }
    void addElimination();
    void setEliminations(int count) { m_eliminationCount = std::max(0, count); }

    // Visual & Animation Telemetry
    float getAnimDistance() const { return m_animDistance; }
    float getAnimTimer() const { return m_animTimer; }
    bool isMoving() const { return (m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y) > 1.0f; }

    // Assassination Mechanics
    bool isBehindTarget(Vector2 targetCenter, float targetFacingAngle, float rearToleranceDegrees = 70.0f) const;
    bool canAssassinate(Vector2 targetCenter, float targetFacingAngle, float maxDistance = 60.0f, float rearToleranceDegrees = 70.0f) const;
    void triggerAssassinationEffect(Vector2 assassinationPoint);

private:
    int m_health;
    int m_maxHealth;
    float m_speed;
    Vector2 m_velocity;
    float m_facingAngle;            // Orientation in degrees
    int m_coinCount;
    int m_eliminationCount;
    float m_assassinationTimer;     // Active timer for slash animation
    Vector2 m_lastAssassinationPos; // Location where the slash occurred
    float m_hitFlashTimer;          // Visual hit reaction feedback timer
    float m_animDistance;           // Accumulated distance for walk cycle
    float m_animTimer;              // Stance & idle breathing timer
};
