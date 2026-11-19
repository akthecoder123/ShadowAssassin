#pragma once

#include "Entity.h"

/**
 * @brief Base class for hostile entities in the game.
 * 
 * Demonstrates:
 * - Inheritance: Inherits from Entity.
 * - Polymorphism: Declares virtual hooks (takeDamage, onAlerted, attack) that derived enemy types override.
 * - Encapsulation: Health, speed, and directional orientation are protected/private.
 */
class Enemy : public Entity {
public:
    Enemy(Vector2 position, float radius = 16.0f, int health = 50, float speed = 100.0f, Color color = Color{ 220, 50, 50, 255 });
    Enemy(float x, float y, float radius = 16.0f, int health = 50, float speed = 100.0f, Color color = Color{ 220, 50, 50, 255 });
    ~Enemy() override = default;

    // Entity lifecycle overrides
    void update(float deltaTime) override = 0; // Remains abstract for specialized enemies
    void draw() const override = 0;

    // Health and Combat Interfaces
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    bool isAlive() const { return m_health > 0; }
    
    virtual void takeDamage(int amount);
    virtual void onAlerted(Vector2 targetPos);
    virtual void attack();

    // Movement & Orientation
    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }
    float getFacingAngle() const { return m_facingAngle; }
    void setFacingAngle(float degrees) { m_facingAngle = degrees; }
    float getRadius() const { return m_size.x * 0.5f; }

protected:
    int m_health;
    int m_maxHealth;
    float m_speed;
    float m_facingAngle; // In degrees
};
