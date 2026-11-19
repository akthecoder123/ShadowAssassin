#pragma once

#include "raylib.h"
#include "Obstacle.h"
#include <vector>

/**
 * @brief Handles geometric collision detection, penetration resolution, and line-of-sight raycasts.
 * 
 * Demonstrates:
 * - Single Responsibility Principle (SRP): Isolates collision mathematics and physics resolution.
 * - Abstraction: Exposes clean interfaces for movement resolution and line-of-sight queries.
 * - Separation of Concerns: Decoupled from Player, Guard, and GameManager.
 */
class CollisionManager {
public:
    CollisionManager() = default;
    ~CollisionManager() = default;

    // Circle vs Rectangle intersection test
    static bool checkCircleRect(Vector2 circleCenter, float radius, Rectangle rect);

    // Resolves penetration of a circle against a single rectangle, returning corrected center
    static Vector2 resolveCircleRect(Vector2 circleCenter, float radius, Rectangle rect);

    // Resolves movement against a collection of obstacles, enabling smooth wall sliding
    static Vector2 resolveMovement(
        Vector2 currentCenter,
        Vector2 velocity,
        float radius,
        float deltaTime,
        const std::vector<Obstacle>& obstacles
    );

    // Clamps entity to world / map boundary margins
    static Vector2 clampToMapBounds(
        Vector2 center,
        float radius,
        float minX,
        float minY,
        float maxX,
        float maxY
    );

    // Queries if a circle intersects with any obstacle in the collection
    static bool collidesWithAnyObstacle(
        Vector2 center,
        float radius,
        const std::vector<Obstacle>& obstacles
    );

    // Line Segment vs Line Segment intersection test
    static bool checkLineIntersection(
        Vector2 a1, Vector2 a2,
        Vector2 b1, Vector2 b2,
        Vector2* hitPoint = nullptr
    );

    // Line Segment vs Rectangle intersection test
    static bool checkLineRectIntersection(
        Vector2 start, Vector2 end,
        Rectangle rect
    );

    // Line of sight raycast across obstacles (returns true if unobstructed)
    static bool checkLineOfSight(
        Vector2 start, Vector2 end,
        const std::vector<Obstacle>& obstacles
    );
};
