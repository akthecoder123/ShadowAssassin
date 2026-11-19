#include "Player.h"
#include "CollisionManager.h"
#include "CharacterRenderer.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

Player::Player(Vector2 position, float radius, int maxHealth, float speed)
    : Entity(position.x - radius, position.y - radius, radius * 2.0f, radius * 2.0f, Color{ 50, 130, 240, 255 }),
      m_health(maxHealth),
      m_maxHealth(maxHealth),
      m_speed(speed),
      m_velocity(Vector2{ 0.0f, 0.0f }),
      m_facingAngle(0.0f),
      m_coinCount(0),
      m_eliminationCount(0),
      m_assassinationTimer(0.0f),
      m_lastAssassinationPos(position),
      m_hitFlashTimer(0.0f),
      m_animDistance(0.0f),
      m_animTimer(0.0f) {
}

Player::Player(float x, float y, float radius, int maxHealth, float speed)
    : Entity(x - radius, y - radius, radius * 2.0f, radius * 2.0f, Color{ 50, 130, 240, 255 }),
      m_health(maxHealth),
      m_maxHealth(maxHealth),
      m_speed(speed),
      m_velocity(Vector2{ 0.0f, 0.0f }),
      m_facingAngle(0.0f),
      m_coinCount(0),
      m_eliminationCount(0),
      m_assassinationTimer(0.0f),
      m_lastAssassinationPos(Vector2{ x, y }),
      m_hitFlashTimer(0.0f),
      m_animDistance(0.0f),
      m_animTimer(0.0f) {
}

void Player::handleInput() {
    if (!isAlive()) {
        m_velocity = Vector2{ 0.0f, 0.0f };
        return;
    }

    float dx = 0.0f;
    float dy = 0.0f;

    // Support both WASD and Arrow Keys for fluid 8-directional control
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    dy -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  dy += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  dx -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) dx += 1.0f;

    // Diagonal Movement Normalization (prevents 1.414x faster diagonal speed)
    float lengthSq = dx * dx + dy * dy;
    if (lengthSq > 0.0f) {
        float length = std::sqrt(lengthSq);
        dx /= length;
        dy /= length;
        m_velocity = Vector2{ dx * m_speed, dy * m_speed };

        // Update facing orientation to direction of movement
        m_facingAngle = std::atan2(dy, dx) * RAD2DEG;
    } else {
        m_velocity = Vector2{ 0.0f, 0.0f };
    }
}

void Player::moveAndSlide(
    float deltaTime,
    const std::vector<Obstacle>& obstacles,
    float mapWidth,
    float mapHeight
) {
    if (!isAlive()) return;

    Vector2 currentCenter = getCenter();
    Vector2 newCenter = CollisionManager::resolveMovement(
        currentCenter,
        m_velocity,
        getRadius(),
        deltaTime,
        obstacles
    );

    newCenter = CollisionManager::clampToMapBounds(
        newCenter,
        getRadius(),
        0.0f,
        50.0f,
        mapWidth,
        mapHeight
    );

    // Synchronize Entity top-left position from resolved center
    float radius = getRadius();
    m_position = Vector2{ newCenter.x - radius, newCenter.y - radius };
}

void Player::takeDamage(int amount) {
    if (amount <= 0 || !isAlive()) return;
    m_health = std::max(0, m_health - amount);
    m_hitFlashTimer = 0.22f; // Trigger visual hit feedback flash
    if (m_health == 0) {
        m_active = false;
    }
}

void Player::heal(int amount) {
    if (amount <= 0 || !isAlive()) return;
    m_health = std::min(m_maxHealth, m_health + amount);
}

void Player::resetHealth() {
    m_health = m_maxHealth;
    m_active = true;
    m_hitFlashTimer = 0.0f;
}

void Player::addCoins(int amount) {
    if (amount > 0) {
        m_coinCount += amount;
    }
}

void Player::addElimination() {
    m_eliminationCount++;
}

bool Player::isBehindTarget(
    Vector2 targetCenter,
    float targetFacingAngle,
    float rearToleranceDegrees
) const {
    Vector2 playerCenter = getCenter();
    float dx = playerCenter.x - targetCenter.x;
    float dy = playerCenter.y - targetCenter.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.001f) {
        return true; // Directly on target
    }

    // Direction vector of target facing
    float rad = targetFacingAngle * DEG2RAD;
    float forwardX = std::cos(rad);
    float forwardY = std::sin(rad);

    // Vector from target to player normalized
    float toPlayerX = dx / dist;
    float toPlayerY = dy / dist;

    // Dot product with rear direction (-forward):
    // cosAlpha = toPlayer . (-forward)
    float cosAlpha = -(toPlayerX * forwardX + toPlayerY * forwardY);
    float cosTolerance = std::cos(rearToleranceDegrees * DEG2RAD);

    return cosAlpha >= cosTolerance;
}

bool Player::canAssassinate(
    Vector2 targetCenter,
    float targetFacingAngle,
    float maxDistance,
    float rearToleranceDegrees
) const {
    if (!isAlive()) return false;
    if (distanceTo(targetCenter) > maxDistance) return false;
    return isBehindTarget(targetCenter, targetFacingAngle, rearToleranceDegrees);
}

void Player::triggerAssassinationEffect(Vector2 assassinationPoint) {
    m_assassinationTimer = 0.35f;
    m_lastAssassinationPos = assassinationPoint;
}

void Player::update(float deltaTime) {
    // Accumulate animation distance when moving
    float speedSq = m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y;
    if (speedSq > 1.0f) {
        m_animDistance += std::sqrt(speedSq) * deltaTime;
    }
    m_animTimer += deltaTime;

    // Decay assassination slash animation timer
    if (m_assassinationTimer > 0.0f) {
        m_assassinationTimer -= deltaTime;
        if (m_assassinationTimer < 0.0f) {
            m_assassinationTimer = 0.0f;
        }
    }

    // Decay damage flash timer
    if (m_hitFlashTimer > 0.0f) {
        m_hitFlashTimer -= deltaTime;
        if (m_hitFlashTimer < 0.0f) {
            m_hitFlashTimer = 0.0f;
        }
    }
}

void Player::draw() const {
    Vector2 center = getCenter();
    float radius = getRadius();
    bool moving = (m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y) > 1.0f;

    // Render polished top-down character sprite via CharacterRenderer
    CharacterRenderer::drawPlayer(
        center,
        radius,
        m_facingAngle,
        moving,
        isAlive() && m_active,
        m_animDistance,
        m_animTimer,
        isHitFlashing(),
        m_assassinationTimer,
        m_lastAssassinationPos
    );

    // Overhead Mini Health Bar if damaged and alive
    if (isAlive() && m_active && m_health < m_maxHealth) {
        float barWidth = radius * 2.4f;
        float barHeight = 4.0f;
        float barX = center.x - barWidth * 0.5f;
        float barY = center.y - radius - 10.0f;
        float healthPct = static_cast<float>(m_health) / static_cast<float>(m_maxHealth);

        DrawRectangle(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth), static_cast<int>(barHeight), Color{ 50, 50, 50, 200 });
        DrawRectangle(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth * healthPct), static_cast<int>(barHeight), Color{ 40, 200, 70, 255 });
        DrawRectangleLines(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth), static_cast<int>(barHeight), BLACK);
    }
}
