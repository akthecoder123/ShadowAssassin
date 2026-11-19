#include "Guard.h"
#include "Player.h"
#include "CollisionManager.h"
#include "CharacterRenderer.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

Guard::Guard(Vector2 position, float radius, int health, float speed)
    : Enemy(position, radius, health, speed, Color{ 220, 50, 50, 255 }),
      m_state(GuardState::PATROLLING),
      m_stateTimer(0.0f),
      m_alertDuration(0.6f),
      m_searchDuration(2.2f),
      m_searchTimer(0.0f),
      m_currentWaypointIndex(0),
      m_patrolSpeed(speed),
      m_chaseSpeed(speed * 1.55f),
      m_waypointWaitTimer(0.0f),
      m_visionRadius(200.0f),
      m_visionAngle(60.0f),
      m_lastKnownPlayerPos(position),
      m_attackCooldown(0.0f),
      m_attackInterval(0.85f),
      m_attackDamage(15),
      m_attackDistance(34.0f),
      m_attackSlashTimer(0.0f),
      m_isVulnerable(false),
      m_animDistance(0.0f),
      m_animTimer(0.0f) {
}

Guard::Guard(float x, float y, float radius, int health, float speed)
    : Enemy(x, y, radius, health, speed, Color{ 220, 50, 50, 255 }),
      m_state(GuardState::PATROLLING),
      m_stateTimer(0.0f),
      m_alertDuration(0.6f),
      m_searchDuration(2.2f),
      m_searchTimer(0.0f),
      m_currentWaypointIndex(0),
      m_patrolSpeed(speed),
      m_chaseSpeed(speed * 1.55f),
      m_waypointWaitTimer(0.0f),
      m_visionRadius(200.0f),
      m_visionAngle(60.0f),
      m_lastKnownPlayerPos(Vector2{ x, y }),
      m_attackCooldown(0.0f),
      m_attackInterval(0.85f),
      m_attackDamage(15),
      m_attackDistance(34.0f),
      m_attackSlashTimer(0.0f),
      m_isVulnerable(false),
      m_animDistance(0.0f),
      m_animTimer(0.0f) {
}

void Guard::setState(GuardState state) {
    m_state = state;
    m_stateTimer = 0.0f;
    if (m_state == GuardState::DEAD) {
        m_health = 0;
        m_active = false;
        m_isVulnerable = false;
    }
}

const char* Guard::getStateName() const {
    switch (m_state) {
        case GuardState::PATROLLING: return "PATROLLING";
        case GuardState::ALERTED:    return "ALERTED";
        case GuardState::CHASING:    return "CHASING";
        case GuardState::ATTACKING:  return "ATTACKING";
        case GuardState::DEAD:       return "DEAD";
        default:                     return "UNKNOWN";
    }
}

void Guard::eliminate() {
    setState(GuardState::DEAD);
}

void Guard::takeDamage(int amount) {
    Enemy::takeDamage(amount);
    if (!isAlive()) {
        setState(GuardState::DEAD);
    }
}

void Guard::onAlerted(Vector2 targetPos) {
    if (m_state != GuardState::DEAD) {
        m_lastKnownPlayerPos = targetPos;
        setState(GuardState::ALERTED);
    }
}

void Guard::attack() {
    if (m_state != GuardState::DEAD) {
        setState(GuardState::ATTACKING);
    }
}

void Guard::addPatrolPoint(Vector2 point) {
    m_patrolPoints.push_back(point);
}

void Guard::addPatrolPoint(float x, float y) {
    m_patrolPoints.push_back(Vector2{ x, y });
}

void Guard::clearPatrolPoints() {
    m_patrolPoints.clear();
    m_currentWaypointIndex = 0;
}

bool Guard::isPointInVisionSector(Vector2 targetPos) const {
    if (m_state == GuardState::DEAD) return false;

    Vector2 center = getCenter();
    float dist = Vector2Distance(center, targetPos);
    if (dist > m_visionRadius) return false;

    // Angle to target in degrees (-180 to 180)
    float dx = targetPos.x - center.x;
    float dy = targetPos.y - center.y;
    float angleToTarget = std::atan2(dy, dx) * RAD2DEG;

    // Normalize angle difference to [-180, 180]
    float diff = angleToTarget - m_facingAngle;
    while (diff > 180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;

    return std::abs(diff) <= (m_visionAngle * 0.5f);
}

bool Guard::canSeePlayer(const Player& player, const std::vector<Obstacle>& obstacles) const {
    if (m_state == GuardState::DEAD || !player.isAlive()) {
        return false;
    }

    Vector2 center = getCenter();
    Vector2 playerCenter = player.getCenter();

    // 1. Distance check
    if (Vector2Distance(center, playerCenter) > m_visionRadius) {
        return false;
    }

    // 2. Vision angle sector check
    if (!isPointInVisionSector(playerCenter)) {
        return false;
    }

    // 3. Line of sight raycast across obstacles
    if (!CollisionManager::checkLineOfSight(center, playerCenter, obstacles)) {
        return false;
    }

    return true;
}

bool Guard::isVulnerableTo(const Player& player, float maxDistance, float rearToleranceDegrees) const {
    if (!isAlive() || m_state == GuardState::DEAD) return false;
    return player.canAssassinate(getCenter(), m_facingAngle, maxDistance, rearToleranceDegrees);
}

void Guard::update(float deltaTime) {
    m_stateTimer += deltaTime;
    if (m_state == GuardState::DEAD) {
        m_isVulnerable = false;
        return;
    }
}

void Guard::updateAI(
    float deltaTime,
    Player& player,
    const std::vector<Obstacle>& obstacles,
    float mapWidth,
    float mapHeight
) {
    if (m_state == GuardState::DEAD) {
        m_isVulnerable = false;
        return;
    }

    m_stateTimer += deltaTime;
    m_animTimer += deltaTime;
    if (m_attackCooldown > 0.0f) {
        m_attackCooldown = std::max(0.0f, m_attackCooldown - deltaTime);
    }
    if (m_attackSlashTimer > 0.0f) {
        m_attackSlashTimer = std::max(0.0f, m_attackSlashTimer - deltaTime);
    }

    // Execute active FSM state handler
    switch (m_state) {
        case GuardState::PATROLLING:
            updatePatrol(deltaTime, obstacles, mapWidth, mapHeight);
            if (canSeePlayer(player, obstacles)) {
                onAlerted(player.getCenter());
            }
            break;

        case GuardState::ALERTED:
            updateAlerted(deltaTime, player);
            break;

        case GuardState::CHASING:
            updateChase(deltaTime, player, obstacles, mapWidth, mapHeight);
            break;

        case GuardState::ATTACKING:
            updateAttack(deltaTime, player);
            break;

        case GuardState::DEAD:
            break;
    }
}

void Guard::updatePatrol(
    float deltaTime,
    const std::vector<Obstacle>& obstacles,
    float mapWidth,
    float mapHeight
) {
    if (m_patrolPoints.empty()) return;

    if (m_waypointWaitTimer > 0.0f) {
        m_waypointWaitTimer -= deltaTime;
        return;
    }

    Vector2 center = getCenter();
    Vector2 target = m_patrolPoints[m_currentWaypointIndex];

    float dx = target.x - center.x;
    float dy = target.y - center.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= 6.0f) {
        // Waypoint reached: advance to next waypoint
        m_currentWaypointIndex = (m_currentWaypointIndex + 1) % m_patrolPoints.size();
        m_waypointWaitTimer = 0.4f; // Brief tactical pause
        return;
    }

    // Smooth movement towards waypoint
    dx /= dist;
    dy /= dist;
    m_facingAngle = std::atan2(dy, dx) * RAD2DEG;

    Vector2 velocity = Vector2{ dx * m_patrolSpeed, dy * m_patrolSpeed };
    Vector2 newCenter = CollisionManager::resolveMovement(center, velocity, getRadius(), deltaTime, obstacles);
    newCenter = CollisionManager::clampToMapBounds(newCenter, getRadius(), 0.0f, 50.0f, mapWidth, mapHeight);

    m_animDistance += m_patrolSpeed * deltaTime;
    float radius = getRadius();
    m_position = Vector2{ newCenter.x - radius, newCenter.y - radius };
}

void Guard::updateAlerted(float deltaTime, Player& player) {
    (void)deltaTime;

    // Face the player while alerted
    Vector2 center = getCenter();
    Vector2 playerCenter = player.getCenter();
    float dx = playerCenter.x - center.x;
    float dy = playerCenter.y - center.y;
    m_facingAngle = std::atan2(dy, dx) * RAD2DEG;
    m_lastKnownPlayerPos = playerCenter;

    // Transition to CHASING after alert duration
    if (m_stateTimer >= m_alertDuration) {
        setState(GuardState::CHASING);
        m_searchTimer = 0.0f;
    }
}

void Guard::updateChase(
    float deltaTime,
    Player& player,
    const std::vector<Obstacle>& obstacles,
    float mapWidth,
    float mapHeight
) {
    Vector2 center = getCenter();
    bool playerVisible = canSeePlayer(player, obstacles);

    if (playerVisible) {
        m_lastKnownPlayerPos = player.getCenter();
        m_searchTimer = 0.0f;
    }

    // Check if within attack range
    float distToPlayer = distanceTo(player.getCenter());
    if (distToPlayer <= m_attackDistance && player.isAlive()) {
        setState(GuardState::ATTACKING);
        return;
    }

    // Target position: live player or last known position
    Vector2 target = playerVisible ? player.getCenter() : m_lastKnownPlayerPos;

    float dx = target.x - center.x;
    float dy = target.y - center.y;
    float distToTarget = std::sqrt(dx * dx + dy * dy);

    // If player is out of sight and guard reached last known location -> Search
    if (!playerVisible && distToTarget <= 16.0f) {
        m_searchTimer += deltaTime;
        // Search behavior: slowly rotate looking for clues
        m_facingAngle += 90.0f * deltaTime;

        if (m_searchTimer >= m_searchDuration) {
            // Lost track completely: return to patrolling
            setState(GuardState::PATROLLING);
            return;
        }
    } else if (distToTarget > 6.0f) {
        dx /= distToTarget;
        dy /= distToTarget;
        m_facingAngle = std::atan2(dy, dx) * RAD2DEG;

        Vector2 velocity = Vector2{ dx * m_chaseSpeed, dy * m_chaseSpeed };
        Vector2 newCenter = CollisionManager::resolveMovement(center, velocity, getRadius(), deltaTime, obstacles);
        newCenter = CollisionManager::clampToMapBounds(newCenter, getRadius(), 0.0f, 50.0f, mapWidth, mapHeight);

        m_animDistance += m_chaseSpeed * deltaTime;
        float radius = getRadius();
        m_position = Vector2{ newCenter.x - radius, newCenter.y - radius };
    }
}

void Guard::updateAttack(float deltaTime, Player& player) {
    (void)deltaTime;

    if (!player.isAlive()) {
        setState(GuardState::PATROLLING);
        return;
    }

    Vector2 center = getCenter();
    Vector2 playerCenter = player.getCenter();

    // Face target
    float dx = playerCenter.x - center.x;
    float dy = playerCenter.y - center.y;
    m_facingAngle = std::atan2(dy, dx) * RAD2DEG;

    // Check if player fled out of attack range
    float dist = distanceTo(playerCenter);
    if (dist > m_attackDistance + 12.0f) {
        setState(GuardState::CHASING);
        return;
    }

    // Apply attack damage respecting cooldown
    if (m_attackCooldown <= 0.0f) {
        player.takeDamage(m_attackDamage);
        m_attackCooldown = m_attackInterval;
        m_attackSlashTimer = 0.22f; // Visual strike feedback
    }
}

void Guard::draw() const {
    CharacterRenderer::drawGuard(
        getCenter(),
        getRadius(),
        m_facingAngle,
        m_state,
        getStateName(),
        m_animDistance,
        m_animTimer,
        m_isVulnerable,
        m_attackSlashTimer,
        m_visionRadius,
        m_visionAngle
    );
}
