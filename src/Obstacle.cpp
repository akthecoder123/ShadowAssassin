#include "Obstacle.h"

Obstacle::Obstacle(Rectangle bounds, bool blocksVision, Color color)
    : m_bounds(bounds),
      m_blocksVision(blocksVision),
      m_color(color) {
}

Obstacle::Obstacle(float x, float y, float width, float height, bool blocksVision, Color color)
    : m_bounds(Rectangle{ x, y, width, height }),
      m_blocksVision(blocksVision),
      m_color(color) {
}

bool Obstacle::contains(Vector2 point) const {
    return CheckCollisionPointRec(point, m_bounds);
}

bool Obstacle::collidesWithCircle(Vector2 circleCenter, float radius) const {
    return CheckCollisionCircleRec(circleCenter, radius, m_bounds);
}

bool Obstacle::collidesWithRect(Rectangle other) const {
    return CheckCollisionRecs(m_bounds, other);
}

void Obstacle::draw() const {
    // Drop shadow
    DrawRectangle(
        static_cast<int>(m_bounds.x + 3),
        static_cast<int>(m_bounds.y + 3),
        static_cast<int>(m_bounds.width),
        static_cast<int>(m_bounds.height),
        Color{ 10, 12, 16, 120 }
    );

    // Solid obstacle surface
    DrawRectangleRec(m_bounds, m_color);

    // Tactical metallic border & bevel
    DrawRectangleLinesEx(m_bounds, 1.5f, Color{ 70, 80, 100, 255 });

    // Inner subtle pattern/accent
    if (m_bounds.width > 24 && m_bounds.height > 24) {
        DrawLine(
            static_cast<int>(m_bounds.x + 4),
            static_cast<int>(m_bounds.y + 4),
            static_cast<int>(m_bounds.x + m_bounds.width - 4),
            static_cast<int>(m_bounds.y + 4),
            Color{ 90, 105, 130, 150 }
        );
    }
}
