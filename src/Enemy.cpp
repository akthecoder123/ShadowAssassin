#include "Enemy.h"
#include <algorithm>

Enemy::Enemy(Vector2 position, float radius, int health, float speed, Color color)
    : Entity(position.x - radius, position.y - radius, radius * 2.0f, radius * 2.0f, color),
      m_health(health),
      m_maxHealth(health),
      m_speed(speed),
      m_facingAngle(0.0f) {
}

Enemy::Enemy(float x, float y, float radius, int health, float speed, Color color)
    : Entity(x - radius, y - radius, radius * 2.0f, radius * 2.0f, color),
      m_health(health),
      m_maxHealth(health),
      m_speed(speed),
      m_facingAngle(0.0f) {
}

void Enemy::takeDamage(int amount) {
    if (amount <= 0 || !isAlive()) return;
    m_health = std::max(0, m_health - amount);
    if (m_health == 0) {
        m_active = false;
    }
}

void Enemy::onAlerted(Vector2 targetPos) {
    (void)targetPos; // Default base implementation - overridden by Guard
}

void Enemy::attack() {
    // Default base implementation - overridden by Guard
}
