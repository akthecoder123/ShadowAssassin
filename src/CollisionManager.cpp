#include "CollisionManager.h"
#include <algorithm>
#include <cmath>

bool CollisionManager::checkCircleRect(Vector2 circleCenter, float radius, Rectangle rect) {
    float closestX = std::clamp(circleCenter.x, rect.x, rect.x + rect.width);
    float closestY = std::clamp(circleCenter.y, rect.y, rect.y + rect.height);

    float distX = circleCenter.x - closestX;
    float distY = circleCenter.y - closestY;

    // Strict overlap test
    return (distX * distX + distY * distY) < (radius * radius);
}

Vector2 CollisionManager::resolveCircleRect(Vector2 circleCenter, float radius, Rectangle rect) {
    float closestX = std::clamp(circleCenter.x, rect.x, rect.x + rect.width);
    float closestY = std::clamp(circleCenter.y, rect.y, rect.y + rect.height);

    float distX = circleCenter.x - closestX;
    float distY = circleCenter.y - closestY;
    float distSq = distX * distX + distY * distY;

    // No collision: circle is outside the rectangle
    if (distSq >= radius * radius) {
        return circleCenter;
    }

    float dist = std::sqrt(distSq);
    // Collision skin epsilon to clear boundary precision
    const float kSkin = 0.01f;

    // If center is outside or on the edge of the rectangle
    if (dist > 0.0001f) {
        float penetration = radius - dist + kSkin;
        Vector2 normal = Vector2{ distX / dist, distY / dist };
        return Vector2{
            circleCenter.x + normal.x * penetration,
            circleCenter.y + normal.y * penetration
        };
    } else {
        // Circle center is inside the rectangle: push out along minimum penetration axis
        float dLeft = circleCenter.x - rect.x;
        float dRight = (rect.x + rect.width) - circleCenter.x;
        float dTop = circleCenter.y - rect.y;
        float dBottom = (rect.y + rect.height) - circleCenter.y;

        float minD = std::min({ dLeft, dRight, dTop, dBottom });
        Vector2 resolved = circleCenter;

        if (minD == dLeft) {
            resolved.x = rect.x - radius - kSkin;
        } else if (minD == dRight) {
            resolved.x = rect.x + rect.width + radius + kSkin;
        } else if (minD == dTop) {
            resolved.y = rect.y - radius - kSkin;
        } else {
            resolved.y = rect.y + rect.height + radius + kSkin;
        }

        return resolved;
    }
}

Vector2 CollisionManager::resolveMovement(
    Vector2 currentCenter,
    Vector2 velocity,
    float radius,
    float deltaTime,
    const std::vector<Obstacle>& obstacles
) {
    Vector2 pos = currentCenter;

    // 1. Resolve X-axis displacement (allows wall sliding horizontally)
    pos.x += velocity.x * deltaTime;
    for (const auto& obs : obstacles) {
        if (obs.collidesWithCircle(pos, radius)) {
            pos = resolveCircleRect(pos, radius, obs.getBounds());
        }
    }

    // 2. Resolve Y-axis displacement (allows wall sliding vertically)
    pos.y += velocity.y * deltaTime;
    for (const auto& obs : obstacles) {
        if (obs.collidesWithCircle(pos, radius)) {
            pos = resolveCircleRect(pos, radius, obs.getBounds());
        }
    }

    // 3. Relaxation pass for complex corners or multi-obstacle intersections
    for (const auto& obs : obstacles) {
        if (obs.collidesWithCircle(pos, radius)) {
            pos = resolveCircleRect(pos, radius, obs.getBounds());
        }
    }

    return pos;
}

Vector2 CollisionManager::clampToMapBounds(
    Vector2 center,
    float radius,
    float minX,
    float minY,
    float maxX,
    float maxY
) {
    return Vector2{
        std::clamp(center.x, minX + radius, maxX - radius),
        std::clamp(center.y, minY + radius, maxY - radius)
    };
}

bool CollisionManager::collidesWithAnyObstacle(
    Vector2 center,
    float radius,
    const std::vector<Obstacle>& obstacles
) {
    for (const auto& obs : obstacles) {
        if (obs.collidesWithCircle(center, radius)) {
            return true;
        }
    }
    return false;
}

bool CollisionManager::checkLineIntersection(
    Vector2 a1, Vector2 a2,
    Vector2 b1, Vector2 b2,
    Vector2* hitPoint
) {
    float d1x = a2.x - a1.x;
    float d1y = a2.y - a1.y;
    float d2x = b2.x - b1.x;
    float d2y = b2.y - b1.y;

    float det = d1x * d2y - d1y * d2x;
    if (std::abs(det) < 0.00001f) {
        return false; // Parallel or collinear lines
    }

    float t1 = ((b1.x - a1.x) * d2y - (b1.y - a1.y) * d2x) / det;
    float t2 = ((b1.x - a1.x) * d1y - (b1.y - a1.y) * d1x) / det;

    if (t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f) {
        if (hitPoint) {
            hitPoint->x = a1.x + t1 * d1x;
            hitPoint->y = a1.y + t1 * d1y;
        }
        return true;
    }

    return false;
}

bool CollisionManager::checkLineRectIntersection(
    Vector2 start, Vector2 end,
    Rectangle rect
) {
    // If either endpoint is inside the rectangle, line of sight is obstructed
    if (CheckCollisionPointRec(start, rect) || CheckCollisionPointRec(end, rect)) {
        return true;
    }

    Vector2 topLeft{ rect.x, rect.y };
    Vector2 topRight{ rect.x + rect.width, rect.y };
    Vector2 bottomLeft{ rect.x, rect.y + rect.height };
    Vector2 bottomRight{ rect.x + rect.width, rect.y + rect.height };

    // Test intersection against all 4 edges of the AABB
    if (checkLineIntersection(start, end, topLeft, topRight)) return true;
    if (checkLineIntersection(start, end, topRight, bottomRight)) return true;
    if (checkLineIntersection(start, end, bottomRight, bottomLeft)) return true;
    if (checkLineIntersection(start, end, bottomLeft, topLeft)) return true;

    return false;
}

bool CollisionManager::checkLineOfSight(
    Vector2 start, Vector2 end,
    const std::vector<Obstacle>& obstacles
) {
    for (const auto& obs : obstacles) {
        if (obs.blocksVision() && checkLineRectIntersection(start, end, obs.getBounds())) {
            return false; // Vision is blocked
        }
    }
    return true; // Unobstructed line of sight
}
