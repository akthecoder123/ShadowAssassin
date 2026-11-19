#include "Entity.h"
#include "raymath.h"

Entity::Entity(Vector2 position, Vector2 size, Color color)
    : m_position(position),
      m_size(size),
      m_active(true),
      m_color(color) {
}

Entity::Entity(float x, float y, float width, float height, Color color)
    : m_position(Vector2{ x, y }),
      m_size(Vector2{ width, height }),
      m_active(true),
      m_color(color) {
}

Vector2 Entity::getCenter() const {
    return Vector2{
        m_position.x + m_size.x * 0.5f,
        m_position.y + m_size.y * 0.5f
    };
}

Rectangle Entity::getBounds() const {
    return Rectangle{
        m_position.x,
        m_position.y,
        m_size.x,
        m_size.y
    };
}

float Entity::distanceTo(const Entity& other) const {
    return Vector2Distance(getCenter(), other.getCenter());
}

float Entity::distanceTo(Vector2 point) const {
    return Vector2Distance(getCenter(), point);
}
