#pragma once

#include <string>
#include <vector>
#include <memory>
#include "raylib.h"
#include "Obstacle.h"
#include "Guard.h"
#include "Collectible.h"

// Forward declaration
class Player;

/**
 * @brief Represents a discrete handcrafted gameplay mission/level in Shadow Assassin.
 * 
 * Demonstrates:
 * - Encapsulation: Level geometry, enemy rosters, collectibles, and spawn coordinates are private.
 * - Composition: Strongly owns collections of Obstacle, Guard, and Collectible objects.
 * - Single Responsibility Principle (SRP): Encapsulates level map layouts, entity initialization,
 *   spatial queries, and resetting state without managing high-level screen states.
 */
class Level {
public:
    explicit Level(int levelNumber = 1);
    ~Level() = default;

    // Disallow copying to enforce strict unique ownership of dynamic entities
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;

    // Allow moving
    Level(Level&&) noexcept = default;
    Level& operator=(Level&&) noexcept = default;

    // Level initialization and state reset
    void load(int levelNumber);
    void reset();

    // Level Information Queries
    int getLevelNumber() const { return m_levelNumber; }
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    Vector2 getPlayerSpawn() const { return m_playerSpawn; }
    float getMapWidth() const { return m_mapWidth; }
    float getMapHeight() const { return m_mapHeight; }

    // Object Accessors (Encapsulated collection references)
    std::vector<Obstacle>& getObstacles() { return m_obstacles; }
    const std::vector<Obstacle>& getObstacles() const { return m_obstacles; }

    std::vector<std::unique_ptr<Guard>>& getGuards() { return m_guards; }
    const std::vector<std::unique_ptr<Guard>>& getGuards() const { return m_guards; }

    std::vector<Collectible>& getCoins() { return m_coins; }
    const std::vector<Collectible>& getCoins() const { return m_coins; }

    // Status & Progress Queries
    int getRemainingGuardsCount() const;
    int getTotalGuardsCount() const { return static_cast<int>(m_guards.size()); }
    bool areAllGuardsEliminated() const;

    int getCollectedCoinsCount() const;
    int getTotalCoinsCount() const { return static_cast<int>(m_coins.size()); }
    bool areAllCoinsCollected() const;

    // Lifecycle updates & rendering
    void update(float deltaTime, Player& player);
    void draw() const;

private:
    void initLevel1();
    void initLevel2();
    void initLevel3();
    void initLevel4();
    void initLevel5();

    int m_levelNumber;
    std::string m_name;
    std::string m_description;
    Vector2 m_playerSpawn;
    float m_mapWidth;
    float m_mapHeight;

    std::vector<Obstacle> m_obstacles;
    std::vector<std::unique_ptr<Guard>> m_guards;
    std::vector<Collectible> m_coins;
};
