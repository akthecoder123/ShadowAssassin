#pragma once

#include "Enemy.h"
#include "Obstacle.h"
#include <vector>

// Forward declaration
class Player;

/**
 * @brief Discrete AI states for the Guard Finite State Machine.
 */
enum class GuardState {
    PATROLLING,
    ALERTED,
    CHASING,
    ATTACKING,
    DEAD
};

/**
 * @brief Represents a patrol guard with field-of-vision detection and FSM behavior.
 * 
 * Demonstrates:
 * - Multi-level Inheritance: Guard -> Enemy -> Entity.
 * - Encapsulation: AI state, patrol waypoints, vision parameters, and attack timers are private.
 * - Polymorphism: Overrides update(), draw(), onAlerted(), and attack().
 * - Finite State Machine (FSM): Clean transitions across PATROLLING, ALERTED, CHASING, ATTACKING, DEAD.
 */
class Guard : public Enemy {
public:
    Guard(Vector2 position, float radius = 16.0f, int health = 50, float speed = 75.0f);
    Guard(float x, float y, float radius = 16.0f, int health = 50, float speed = 75.0f);
    ~Guard() override = default;

    // Entity polymorphic overrides
    void update(float deltaTime) override;
    void draw() const override;

    // Enemy combat and alert overrides
    void onAlerted(Vector2 targetPos) override;
    void attack() override;
    void takeDamage(int amount) override;

    // Comprehensive AI Update Loop
    void updateAI(
        float deltaTime,
        Player& player,
        const std::vector<Obstacle>& obstacles,
        float mapWidth,
        float mapHeight
    );

    // AI Finite State Machine Transitions & Queries
    GuardState getState() const { return m_state; }
    void setState(GuardState state);
    const char* getStateName() const;
    void eliminate();

    // Patrol Route Management
    void addPatrolPoint(Vector2 point);
    void addPatrolPoint(float x, float y);
    void clearPatrolPoints();
    const std::vector<Vector2>& getPatrolPoints() const { return m_patrolPoints; }
    size_t getCurrentWaypointIndex() const { return m_currentWaypointIndex; }
    void setCurrentWaypointIndex(size_t index) { m_currentWaypointIndex = index; }

    // Vision Cone Parameters
    float getVisionRadius() const { return m_visionRadius; }
    void setVisionRadius(float radius) { m_visionRadius = radius; }
    float getVisionAngle() const { return m_visionAngle; }
    void setVisionAngle(float degrees) { m_visionAngle = degrees; }

    // Vision & Detection Methods
    bool isPointInVisionSector(Vector2 targetPos) const;
    bool canSeePlayer(const Player& player, const std::vector<Obstacle>& obstacles) const;

    // Combat & Attack Parameters
    float getAttackCooldown() const { return m_attackCooldown; }
    void setAttackCooldown(float cd) { m_attackCooldown = cd; }
    int getAttackDamage() const { return m_attackDamage; }
    void setAttackDamage(int dmg) { m_attackDamage = dmg; }
    float getAttackDistance() const { return m_attackDistance; }
    void setAttackDistance(float dist) { m_attackDistance = dist; }
    Vector2 getLastKnownPlayerPos() const { return m_lastKnownPlayerPos; }

    // Assassination Vulnerability
    void setVulnerable(bool vulnerable) { m_isVulnerable = vulnerable; }
    bool isVulnerable() const { return m_isVulnerable; }
    bool isVulnerableTo(const Player& player, float maxDistance = 60.0f, float rearToleranceDegrees = 70.0f) const;

    // Visual & Animation Telemetry
    float getAnimDistance() const { return m_animDistance; }
    float getAnimTimer() const { return m_animTimer; }

private:
    // Modular Sub-State Handlers
    void updatePatrol(float deltaTime, const std::vector<Obstacle>& obstacles, float mapWidth, float mapHeight);
    void updateAlerted(float deltaTime, Player& player);
    void updateChase(float deltaTime, Player& player, const std::vector<Obstacle>& obstacles, float mapWidth, float mapHeight);
    void updateAttack(float deltaTime, Player& player);

    // State & Timers
    GuardState m_state;
    float m_stateTimer;
    float m_alertDuration;
    float m_searchDuration;
    float m_searchTimer;

    // Movement & Waypoints
    std::vector<Vector2> m_patrolPoints;
    size_t m_currentWaypointIndex;
    float m_patrolSpeed;
    float m_chaseSpeed;
    float m_waypointWaitTimer;

    // Vision Parameters
    float m_visionRadius;
    float m_visionAngle;
    Vector2 m_lastKnownPlayerPos;

    // Combat Parameters
    float m_attackCooldown;
    float m_attackInterval;
    int m_attackDamage;
    float m_attackDistance;
    float m_attackSlashTimer;

    // Assassination Flag
    bool m_isVulnerable;

    // Animation Telemetry
    float m_animDistance;
    float m_animTimer;
};
