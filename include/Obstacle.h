#pragma once

#include "raylib.h"

/**
 * @brief Represents static barriers, walls, and crates in a level.
 * 
 * Demonstrates:
 * - Encapsulation: Barrier boundaries and optical occlusion properties are private.
 * - Solid geometric collision and line-of-sight query interfaces.
 */
class Obstacle {
public:
    Obstacle(Rectangle bounds, bool blocksVision = true, Color color = Color{ 45, 52, 65, 255 });
    Obstacle(float x, float y, float width, float height, bool blocksVision = true, Color color = Color{ 45, 52, 65, 255 });
    ~Obstacle() = default;

    // Spatial & Bounds Accessors
    Rectangle getBounds() const { return m_bounds; }
    void setBounds(Rectangle bounds) { m_bounds = bounds; }
    Vector2 getPosition() const { return Vector2{ m_bounds.x, m_bounds.y }; }
    Vector2 getSize() const { return Vector2{ m_bounds.width, m_bounds.height }; }

    // Collision Detection Interfaces
    bool contains(Vector2 point) const;
    bool collidesWithCircle(Vector2 circleCenter, float radius) const;
    bool collidesWithRect(Rectangle other) const;

    // Vision Properties
    bool blocksVision() const { return m_blocksVision; }
    void setBlocksVision(bool blocks) { m_blocksVision = blocks; }

    // Rendering
    void draw() const;

private:
    Rectangle m_bounds;
    bool m_blocksVision;
    Color m_color;
};
