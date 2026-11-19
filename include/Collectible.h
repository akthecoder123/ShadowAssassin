#pragma once

#include "raylib.h"

/**
 * @brief Represents bonus collectible intelligence coins scattered across maps.
 * 
 * Demonstrates:
 * - Encapsulation: Coordinates, value, collection state, and animation timers are private.
 * - Simple geometric collision and rendering interface.
 */
class Collectible {
public:
    Collectible(Vector2 position, float radius = 9.0f, int value = 1);
    Collectible(float x, float y, float radius = 9.0f, int value = 1);
    ~Collectible() = default;

    // Spatial and State Queries
    Vector2 getPosition() const { return m_position; }
    void setPosition(Vector2 pos) { m_position = pos; }
    float getRadius() const { return m_radius; }
    int getValue() const { return m_value; }
    bool isCollected() const { return m_isCollected; }

    // Collection Mutation
    void collect();
    void reset();

    // Collision Check
    bool checkCollision(Vector2 entityCenter, float entityRadius) const;

    // Lifecycle
    void update(float deltaTime);
    void draw() const;

private:
    Vector2 m_position;
    float m_radius;
    int m_value;
    bool m_isCollected;
    float m_animTimer; // Used for gentle gold shimmer / bobbing effect
};
