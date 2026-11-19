#include "Collectible.h"
#include "raymath.h"
#include <cmath>

Collectible::Collectible(Vector2 position, float radius, int value)
    : m_position(position),
      m_radius(radius),
      m_value(value),
      m_isCollected(false),
      m_animTimer(0.0f) {
}

Collectible::Collectible(float x, float y, float radius, int value)
    : m_position(Vector2{ x, y }),
      m_radius(radius),
      m_value(value),
      m_isCollected(false),
      m_animTimer(0.0f) {
}

void Collectible::collect() {
    m_isCollected = true;
}

void Collectible::reset() {
    m_isCollected = false;
    m_animTimer = 0.0f;
}

bool Collectible::checkCollision(Vector2 entityCenter, float entityRadius) const {
    if (m_isCollected) return false;
    return Vector2Distance(m_position, entityCenter) <= (m_radius + entityRadius);
}

void Collectible::update(float deltaTime) {
    if (!m_isCollected) {
        m_animTimer += deltaTime * 3.5f;
    }
}

void Collectible::draw() const {
    if (m_isCollected) return;

    // Gentle pulsing shimmer
    float pulse = std::sin(m_animTimer) * 1.5f;
    float currentRadius = m_radius + pulse;

    // Drop shadow
    DrawCircle(
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y + 2),
        currentRadius,
        Color{ 0, 0, 0, 70 }
    );

    // Outer subtle gold glow
    DrawCircle(
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y),
        currentRadius + 3.0f,
        Color{ 255, 215, 0, 40 }
    );

    // Primary gold body
    DrawCircleV(m_position, currentRadius, Color{ 255, 195, 20, 255 });
    DrawCircleLines(
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y),
        currentRadius,
        Color{ 180, 130, 10, 255 }
    );

    // Inner bright reflection ring
    DrawCircle(
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y),
        currentRadius * 0.55f,
        Color{ 255, 235, 120, 255 }
    );

    // Central star/intel diamond emblem
    DrawRectangle(
        static_cast<int>(m_position.x - 1.5f),
        static_cast<int>(m_position.y - 1.5f),
        3, 3,
        Color{ 190, 120, 5, 255 }
    );
}
