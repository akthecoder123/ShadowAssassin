#pragma once

#include "raylib.h"

/**
 * @brief Abstract base class representing any interactive entity in the game world.
 * 
 * Demonstrates:
 * - Abstraction: Exposes pure virtual interfaces (update, draw) while hiding concrete behavior.
 * - Encapsulation: Coordinates, size, and activity status are protected with public accessors.
 * - Polymorphism: Destructor and core life-cycle functions are declared virtual.
 */
class Entity {
public:
    Entity(Vector2 position, Vector2 size, Color color = WHITE);
    Entity(float x, float y, float width, float height, Color color = WHITE);
    virtual ~Entity() = default;

    // Prevent object slicing
    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;

    // Pure virtual lifecycle methods (Abstraction & Runtime Polymorphism)
    virtual void update(float deltaTime) = 0;
    virtual void draw() const = 0;

    // Spatial Accessors and Mutators (Encapsulation)
    Vector2 getPosition() const { return m_position; }
    void setPosition(Vector2 position) { m_position = position; }
    void setPosition(float x, float y) { m_position = Vector2{ x, y }; }

    Vector2 getSize() const { return m_size; }
    void setSize(Vector2 size) { m_size = size; }
    void setSize(float width, float height) { m_size = Vector2{ width, height }; }

    Vector2 getCenter() const;
    Rectangle getBounds() const;

    // Spatial utility queries
    float distanceTo(const Entity& other) const;
    float distanceTo(Vector2 point) const;

    // Entity Active State
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    Color getColor() const { return m_color; }
    void setColor(Color color) { m_color = color; }

protected:
    Vector2 m_position;  // Top-left coordinate or center depending on shape
    Vector2 m_size;      // Width and height
    bool m_active;       // Flag indicating if entity participates in gameplay
    Color m_color;       // Base visual tint
};
