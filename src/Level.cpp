#include "Level.h"
#include "Player.h"
#include <algorithm>

Level::Level(int levelNumber)
    : m_levelNumber(1),
      m_name(""),
      m_description(""),
      m_playerSpawn(Vector2{ 140.0f, 320.0f }),
      m_mapWidth(1024.0f),
      m_mapHeight(608.0f) {
    load(levelNumber);
}

void Level::load(int levelNumber) {
    m_levelNumber = std::clamp(levelNumber, 1, 5);
    reset();
}

void Level::reset() {
    m_obstacles.clear();
    m_guards.clear();
    m_coins.clear();

    switch (m_levelNumber) {
        case 1:
            initLevel1();
            break;
        case 2:
            initLevel2();
            break;
        case 3:
            initLevel3();
            break;
        case 4:
            initLevel4();
            break;
        case 5:
            initLevel5();
            break;
        default:
            initLevel1();
            break;
    }
}

void Level::initLevel1() {
    m_name = "Training Ground";
    m_description = "Infiltrate the training facility and neutralize 2 sentry guards.";
    m_playerSpawn = Vector2{ 140.0f, 320.0f };
    m_mapWidth = 1024.0f;
    m_mapHeight = 608.0f;

    // 1. Static Obstacles (Walls & Covers)
    m_obstacles.emplace_back(260.0f, 160.0f, 28.0f, 180.0f, true); // Cover partition near spawn
    m_obstacles.emplace_back(380.0f, 240.0f, 28.0f, 280.0f, true); // Central corridor partition
    m_obstacles.emplace_back(560.0f, 140.0f, 180.0f, 28.0f, true); // North horizontal divider
    m_obstacles.emplace_back(680.0f, 360.0f, 50.0f, 50.0f, true);  // Eastern crate block
    m_obstacles.emplace_back(180.0f, 440.0f, 120.0f, 28.0f, true); // Lower cover barrier

    // 2. Guards (Exactly 2 guards)
    // Guard 1: Vertical corridor sentry
    auto guard1 = std::make_unique<Guard>(500.0f, 200.0f, 16.0f, 50, 70.0f);
    guard1->addPatrolPoint(500.0f, 180.0f);
    guard1->addPatrolPoint(500.0f, 460.0f);
    m_guards.push_back(std::move(guard1));

    // Guard 2: Rectangular perimeter patrol
    auto guard2 = std::make_unique<Guard>(780.0f, 200.0f, 16.0f, 50, 70.0f);
    guard2->addPatrolPoint(780.0f, 200.0f);
    guard2->addPatrolPoint(900.0f, 200.0f);
    guard2->addPatrolPoint(900.0f, 440.0f);
    guard2->addPatrolPoint(780.0f, 440.0f);
    m_guards.push_back(std::move(guard2));

    // 3. Collectibles (4 coins)
    m_coins.emplace_back(200.0f, 220.0f);
    m_coins.emplace_back(440.0f, 200.0f);
    m_coins.emplace_back(640.0f, 460.0f);
    m_coins.emplace_back(840.0f, 330.0f);
}

void Level::initLevel2() {
    m_name = "Restricted Facility";
    m_description = "Infiltrate the high-security facility. Evade 3 patrolling security units.";
    m_playerSpawn = Vector2{ 100.0f, 150.0f };
    m_mapWidth = 1024.0f;
    m_mapHeight = 608.0f;

    // 1. Static Obstacles (8 walls and covers)
    m_obstacles.emplace_back(220.0f, 85.0f, 28.0f, 160.0f, true);  // Northwest room partition
    m_obstacles.emplace_back(100.0f, 245.0f, 148.0f, 28.0f, true); // Northwest horizontal wall
    m_obstacles.emplace_back(420.0f, 190.0f, 28.0f, 140.0f, true); // Central dividing wall north
    m_obstacles.emplace_back(300.0f, 370.0f, 150.0f, 28.0f, true); // Central horizontal cover bar
    m_obstacles.emplace_back(640.0f, 120.0f, 28.0f, 220.0f, true); // East vertical security wall
    m_obstacles.emplace_back(700.0f, 190.0f, 60.0f, 60.0f, true);  // South storage crate
    m_obstacles.emplace_back(600.0f, 430.0f, 28.0f, 130.0f, true); // South storage partition
    m_obstacles.emplace_back(360.0f, 490.0f, 160.0f, 28.0f, true); // South boundary cover

    // 2. Guards (Exactly 3 guards)
    // Guard 1: Northern hall sweep
    auto guard1 = std::make_unique<Guard>(360.0f, 140.0f, 16.0f, 50, 75.0f);
    guard1->addPatrolPoint(300.0f, 140.0f);
    guard1->addPatrolPoint(560.0f, 140.0f);
    m_guards.push_back(std::move(guard1));

    // Guard 2: Central corridor patrol
    auto guard2 = std::make_unique<Guard>(500.0f, 340.0f, 16.0f, 50, 80.0f);
    guard2->addPatrolPoint(500.0f, 240.0f);
    guard2->addPatrolPoint(500.0f, 480.0f);
    m_guards.push_back(std::move(guard2));

    // Guard 3: Southeast storage patrol
    auto guard3 = std::make_unique<Guard>(740.0f, 440.0f, 16.0f, 50, 75.0f);
    guard3->addPatrolPoint(740.0f, 440.0f);
    guard3->addPatrolPoint(880.0f, 440.0f);
    guard3->addPatrolPoint(880.0f, 280.0f);
    m_guards.push_back(std::move(guard3));

    // 3. Collectibles (6 coins in high-risk zones)
    m_coins.emplace_back(100.0f, 320.0f);
    m_coins.emplace_back(380.0f, 110.0f);
    m_coins.emplace_back(460.0f, 340.0f);
    m_coins.emplace_back(560.0f, 220.0f);
    m_coins.emplace_back(800.0f, 200.0f);
    m_coins.emplace_back(880.0f, 500.0f);
}

void Level::initLevel3() {
    m_name = "Final Mission";
    m_description = "Eliminate all 4 elite guards in the command citadel to secure victory.";
    m_playerSpawn = Vector2{ 90.0f, 510.0f };
    m_mapWidth = 1024.0f;
    m_mapHeight = 608.0f;

    // 1. Static Obstacles (9 tactical walls and server mainframes)
    m_obstacles.emplace_back(170.0f, 410.0f, 28.0f, 160.0f, true); // Southwest security wall
    m_obstacles.emplace_back(140.0f, 190.0f, 50.0f, 120.0f, true); // West server stack
    m_obstacles.emplace_back(340.0f, 110.0f, 28.0f, 250.0f, true); // West corridor divider
    m_obstacles.emplace_back(550.0f, 260.0f, 80.0f, 80.0f, true);  // Central mainframe server block
    m_obstacles.emplace_back(440.0f, 420.0f, 180.0f, 28.0f, true); // Central south wall
    m_obstacles.emplace_back(720.0f, 100.0f, 28.0f, 160.0f, true); // East vault entrance wall
    m_obstacles.emplace_back(720.0f, 330.0f, 28.0f, 200.0f, true); // East corridor wall
    m_obstacles.emplace_back(830.0f, 390.0f, 60.0f, 60.0f, true);  // Southeast server vault crate
    m_obstacles.emplace_back(840.0f, 90.0f, 80.0f, 24.0f, true);   // Northeast partition

    // 2. Guards (Exactly 4 guards)
    // Guard 1: South corridor patrol
    auto guard1 = std::make_unique<Guard>(320.0f, 510.0f, 16.0f, 50, 80.0f);
    guard1->addPatrolPoint(250.0f, 510.0f);
    guard1->addPatrolPoint(470.0f, 510.0f);
    m_guards.push_back(std::move(guard1));

    // Guard 2: West wing sweep
    auto guard2 = std::make_unique<Guard>(240.0f, 240.0f, 16.0f, 50, 80.0f);
    guard2->addPatrolPoint(240.0f, 150.0f);
    guard2->addPatrolPoint(240.0f, 360.0f);
    m_guards.push_back(std::move(guard2));

    // Guard 3: Central mainframe box sentinel
    auto guard3 = std::make_unique<Guard>(500.0f, 210.0f, 16.0f, 50, 85.0f);
    guard3->addPatrolPoint(500.0f, 210.0f);
    guard3->addPatrolPoint(680.0f, 210.0f);
    guard3->addPatrolPoint(680.0f, 380.0f);
    guard3->addPatrolPoint(500.0f, 380.0f);
    m_guards.push_back(std::move(guard3));

    // Guard 4: Northeast vault warden
    auto guard4 = std::make_unique<Guard>(820.0f, 180.0f, 16.0f, 50, 85.0f);
    guard4->addPatrolPoint(760.0f, 180.0f);
    guard4->addPatrolPoint(900.0f, 180.0f);
    m_guards.push_back(std::move(guard4));

    // 3. Collectibles (7 coins placed strategically)
    m_coins.emplace_back(90.0f, 380.0f);
    m_coins.emplace_back(140.0f, 130.0f);
    m_coins.emplace_back(340.0f, 440.0f);
    m_coins.emplace_back(450.0f, 260.0f);
    m_coins.emplace_back(600.0f, 160.0f);
    m_coins.emplace_back(880.0f, 130.0f);
    m_coins.emplace_back(890.0f, 490.0f);
}

void Level::initLevel4() {
    m_name = "Underground Bunker";
    m_description = "Infiltrate the subterranean bunker. Neutralize 5 sentries guarding the communication corridors.";
    m_playerSpawn = Vector2{ 90.0f, 130.0f };
    m_mapWidth = 1024.0f;
    m_mapHeight = 608.0f;

    // 1. Static Obstacles (11 walls and barricades forming narrow passages and rooms)
    m_obstacles.emplace_back(160.0f, 70.0f, 28.0f, 140.0f, true);  // Northwest room partition
    m_obstacles.emplace_back(80.0f, 260.0f, 160.0f, 28.0f, true);  // Northwest corridor floor divider
    m_obstacles.emplace_back(200.0f, 330.0f, 28.0f, 150.0f, true); // Generator room dividing wall
    m_obstacles.emplace_back(360.0f, 390.0f, 60.0f, 60.0f, true);  // Generator powerhouse machinery block
    m_obstacles.emplace_back(460.0f, 100.0f, 28.0f, 200.0f, true); // North-central corridor wall
    m_obstacles.emplace_back(460.0f, 370.0f, 28.0f, 180.0f, true); // South-central security divider
    m_obstacles.emplace_back(620.0f, 220.0f, 160.0f, 28.0f, true); // East communication room partition
    m_obstacles.emplace_back(620.0f, 390.0f, 28.0f, 140.0f, true); // East communication south wall
    m_obstacles.emplace_back(810.0f, 100.0f, 28.0f, 170.0f, true); // Northeast bunker vault wall
    m_obstacles.emplace_back(810.0f, 360.0f, 50.0f, 50.0f, true);  // Vault gate control console
    m_obstacles.emplace_back(320.0f, 200.0f, 80.0f, 28.0f, true);  // Central junction barrier

    // 2. Guards (Exactly 5 guards)
    // Guard 1: Patrols the northern storage corridor
    auto guard1 = std::make_unique<Guard>(220.0f, 90.0f, 16.0f, 50, 80.0f);
    guard1->addPatrolPoint(220.0f, 90.0f);
    guard1->addPatrolPoint(420.0f, 90.0f);
    m_guards.push_back(std::move(guard1));

    // Guard 2: Sweeps the generator room
    auto guard2 = std::make_unique<Guard>(120.0f, 450.0f, 16.0f, 50, 78.0f);
    guard2->addPatrolPoint(120.0f, 450.0f);
    guard2->addPatrolPoint(320.0f, 450.0f);
    guard2->addPatrolPoint(320.0f, 360.0f);
    guard2->addPatrolPoint(120.0f, 360.0f);
    m_guards.push_back(std::move(guard2));

    // Guard 3: Guards central security checkpoint
    auto guard3 = std::make_unique<Guard>(550.0f, 140.0f, 16.0f, 50, 85.0f);
    guard3->addPatrolPoint(550.0f, 140.0f);
    guard3->addPatrolPoint(550.0f, 480.0f);
    m_guards.push_back(std::move(guard3));

    // Guard 4: Patrols the eastern communication wing
    auto guard4 = std::make_unique<Guard>(650.0f, 150.0f, 16.0f, 50, 82.0f);
    guard4->addPatrolPoint(650.0f, 150.0f);
    guard4->addPatrolPoint(760.0f, 150.0f);
    guard4->addPatrolPoint(760.0f, 320.0f);
    guard4->addPatrolPoint(650.0f, 320.0f);
    m_guards.push_back(std::move(guard4));

    // Guard 5: Small-perimeter sentry guarding vault access
    auto guard5 = std::make_unique<Guard>(760.0f, 520.0f, 16.0f, 50, 85.0f);
    guard5->addPatrolPoint(760.0f, 520.0f);
    guard5->addPatrolPoint(940.0f, 520.0f);
    m_guards.push_back(std::move(guard5));

    // 3. Collectibles (Exactly 8 coins along high-risk routes)
    m_coins.emplace_back(120.0f, 80.0f);
    m_coins.emplace_back(260.0f, 130.0f);
    m_coins.emplace_back(150.0f, 400.0f);
    m_coins.emplace_back(380.0f, 490.0f);
    m_coins.emplace_back(500.0f, 280.0f);
    m_coins.emplace_back(700.0f, 180.0f);
    m_coins.emplace_back(700.0f, 480.0f);
    m_coins.emplace_back(920.0f, 450.0f);
}

void Level::initLevel5() {
    m_name = "Shadow Protocol";
    m_description = "Campaign Climax: Infiltrate the master security core. Terminate 6 elite sentries.";
    m_playerSpawn = Vector2{ 90.0f, 540.0f };
    m_mapWidth = 1024.0f;
    m_mapHeight = 608.0f;

    // 1. Static Obstacles (12 walls, consoles, reinforced barriers: Infiltration, Lab, Server Matrix, Command)
    m_obstacles.emplace_back(170.0f, 430.0f, 28.0f, 150.0f, true); // Southwest infiltration partition
    m_obstacles.emplace_back(90.0f, 310.0f, 150.0f, 28.0f, true);  // Infiltration north wall
    m_obstacles.emplace_back(280.0f, 90.0f, 28.0f, 220.0f, true);  // Northwest lab eastern wall
    m_obstacles.emplace_back(340.0f, 340.0f, 28.0f, 180.0f, true); // West central corridor wall
    m_obstacles.emplace_back(470.0f, 140.0f, 70.0f, 50.0f, true);  // North server mainframe stack
    m_obstacles.emplace_back(480.0f, 280.0f, 60.0f, 60.0f, true);  // Central quantum core pillar
    m_obstacles.emplace_back(420.0f, 440.0f, 180.0f, 28.0f, true); // South server boundary
    m_obstacles.emplace_back(660.0f, 90.0f, 28.0f, 180.0f, true);  // East command security partition
    m_obstacles.emplace_back(660.0f, 330.0f, 28.0f, 180.0f, true); // Southeast vault partition
    m_obstacles.emplace_back(780.0f, 220.0f, 120.0f, 28.0f, true); // Command center console barrier
    m_obstacles.emplace_back(820.0f, 420.0f, 60.0f, 60.0f, true);  // Vault security terminal block
    m_obstacles.emplace_back(200.0f, 170.0f, 50.0f, 50.0f, true);  // Lab storage server

    // 2. Guards (Exactly 6 elite guards with speeds 80-90 px/s)
    // Guard 1: Infiltration zone security sweep
    auto guard1 = std::make_unique<Guard>(100.0f, 380.0f, 16.0f, 50, 82.0f);
    guard1->addPatrolPoint(100.0f, 380.0f);
    guard1->addPatrolPoint(220.0f, 380.0f);
    m_guards.push_back(std::move(guard1));

    // Guard 2: Northwest cyber warfare lab
    auto guard2 = std::make_unique<Guard>(120.0f, 240.0f, 16.0f, 50, 80.0f);
    guard2->addPatrolPoint(120.0f, 240.0f);
    guard2->addPatrolPoint(240.0f, 240.0f);
    guard2->addPatrolPoint(240.0f, 80.0f);
    guard2->addPatrolPoint(120.0f, 80.0f);
    m_guards.push_back(std::move(guard2));

    // Guard 3: Central server matrix core
    auto guard3 = std::make_unique<Guard>(400.0f, 230.0f, 16.0f, 50, 88.0f);
    guard3->addPatrolPoint(400.0f, 230.0f);
    guard3->addPatrolPoint(600.0f, 230.0f);
    guard3->addPatrolPoint(600.0f, 390.0f);
    guard3->addPatrolPoint(400.0f, 390.0f);
    m_guards.push_back(std::move(guard3));

    // Guard 4: South choke-point sentinel
    auto guard4 = std::make_unique<Guard>(250.0f, 520.0f, 16.0f, 50, 90.0f);
    guard4->addPatrolPoint(250.0f, 520.0f);
    guard4->addPatrolPoint(580.0f, 520.0f);
    m_guards.push_back(std::move(guard4));

    // Guard 5: Northeast command center officer
    auto guard5 = std::make_unique<Guard>(720.0f, 150.0f, 16.0f, 50, 85.0f);
    guard5->addPatrolPoint(720.0f, 150.0f);
    guard5->addPatrolPoint(880.0f, 150.0f);
    guard5->addPatrolPoint(880.0f, 300.0f);
    guard5->addPatrolPoint(720.0f, 300.0f);
    m_guards.push_back(std::move(guard5));

    // Guard 6: East vault gatekeeper
    auto guard6 = std::make_unique<Guard>(740.0f, 520.0f, 16.0f, 50, 90.0f);
    guard6->addPatrolPoint(740.0f, 520.0f);
    guard6->addPatrolPoint(920.0f, 520.0f);
    guard6->addPatrolPoint(920.0f, 360.0f);
    guard6->addPatrolPoint(740.0f, 360.0f);
    m_guards.push_back(std::move(guard6));

    // 3. Collectibles (Exactly 9 coins placed strategically)
    m_coins.emplace_back(90.0f, 450.0f);
    m_coins.emplace_back(180.0f, 90.0f);
    m_coins.emplace_back(230.0f, 280.0f);
    m_coins.emplace_back(380.0f, 170.0f);
    m_coins.emplace_back(560.0f, 200.0f);
    m_coins.emplace_back(380.0f, 490.0f);
    m_coins.emplace_back(740.0f, 100.0f);
    m_coins.emplace_back(890.0f, 260.0f);
    m_coins.emplace_back(890.0f, 520.0f);
}

int Level::getRemainingGuardsCount() const {
    int count = 0;
    for (const auto& guard : m_guards) {
        if (guard && guard->isAlive()) {
            count++;
        }
    }
    return count;
}

bool Level::areAllGuardsEliminated() const {
    return getRemainingGuardsCount() == 0;
}

int Level::getCollectedCoinsCount() const {
    int count = 0;
    for (const auto& coin : m_coins) {
        if (coin.isCollected()) {
            count++;
        }
    }
    return count;
}

bool Level::areAllCoinsCollected() const {
    return getCollectedCoinsCount() == getTotalCoinsCount();
}

void Level::update(float deltaTime, Player& player) {
    for (auto& guard : m_guards) {
        guard->updateAI(deltaTime, player, m_obstacles, m_mapWidth, m_mapHeight);
    }
    for (auto& coin : m_coins) {
        coin.update(deltaTime);
    }
}

void Level::draw() const {
    // 1. Draw Static Obstacles
    for (const auto& obstacle : m_obstacles) {
        obstacle.draw();
    }

    // 2. Draw Collectible Coins
    for (const auto& coin : m_coins) {
        coin.draw();
    }

    // 3. Draw Guards (Vision Cones, Alert Badges, Strike Visuals)
    for (const auto& guard : m_guards) {
        guard->draw();
    }
}
