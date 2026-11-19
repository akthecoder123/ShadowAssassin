# Object-Oriented Programming (OOP) Architecture & Concepts
## Project: Shadow Assassin (2D Stealth Game)

This document details the software architecture and formal Object-Oriented Programming (OOP) principles implemented in **Shadow Assassin**, an academic C++17 project demonstrating clean, robust design.

---

## 1. Class Hierarchy and Diagram

```
                 +-------------------+
                 |      Entity       |  (Abstract Base Class)
                 +-------------------+
                 | - m_position: V2  |
                 | - m_size: V2      |
                 | - m_active: bool  |
                 +-------------------+
                 | + update(float) =0|  (Pure virtual)
                 | + draw() =0       |  (Pure virtual)
                 +---------+---------+
                           |
            +--------------+--------------+
            |                             |
  +---------+---------+         +---------+---------+
  |      Player       |         |       Enemy       | (Virtual Base Enemy)
  +-------------------+         +-------------------+
  | - m_health: int   |         | - m_health: int   |
  | - m_speed: float  |         | - m_speed: float  |
  +-------------------+         +-------------------+
  | + attackGuard()   |         | + update(float)   |
  | + move()          |         +---------+---------+
  +-------------------+                   |
                                +---------+---------+
                                |       Guard       | (FSM-driven AI)
                                +-------------------+
                                | - m_patrolPoints  |
                                | - m_visionCone    |
                                | - m_aiState: FSM  |
                                +-------------------+
                                | + updateAI()      |
                                | + checkVision()   |
                                +-------------------+

Standalone Composed Classes:
  +-------------------+         +-------------------+         +-------------------+
  |     Obstacle      |         |    Collectible    |         | CollisionManager  |
  +-------------------+         +-------------------+         +-------------------+
  | - m_bounds: Rec   |         | - m_position: V2  |         | + checkCircleRect |
  | - m_blocksVision  |         | - m_collected     |         | + checkLineOfSight|
  +-------------------+         +-------------------+         +-------------------+

Composite Owner:
  +-----------------------------------------------+
  |                     Level                     |
  +-----------------------------------------------+
  | - m_obstacles: vector<Obstacle>               |
  | - m_enemies: vector<unique_ptr<Enemy>>        |
  | - m_coins: vector<Collectible>                |
  | - m_playerStart: Vector2                      |
  +-----------------------------------------------+
  | + update(float), draw(), isComplete()         |
  +-----------------------------------------------+
```

---

## 2. Core OOP Principles Demonstrated

### 2.1 Encapsulation
- **Data Hiding**: All member variables across `Entity`, `Player`, `Guard`, `Level`, and `GameManager` are marked `private` or `protected`.
- **Public API Contract**: Internal representations (e.g., coordinates, health, state machine transitions) cannot be directly manipulated from outside. They are accessed and modified exclusively through well-defined public getters, setters, and operational methods (`takeDamage()`, `eliminate()`, `collect()`).
- **Invariant Protection**: Member methods validate state changes (e.g., health cannot drop below zero, state machine cannot transition along illegal edges).

### 2.2 Inheritance
- **Entity Hierarchy**: `Entity` serves as the foundational base class for all dynamic interactive objects in the game world.
- **Specialized Behavior**: `Player` inherits core spatial identity from `Entity` and adds player input, assassination reach, and health mechanics.
- **Enemy Specialization**: `Enemy` inherits from `Entity`, providing common hostile attributes. `Guard` inherits from `Enemy`, introducing finite state machine behavior, vision cones, line-of-sight awareness, and patrol routes.

### 2.3 Polymorphism (Run-Time Virtual Dispatch)
- **Virtual Interface**: `Entity` declares `virtual void update(float deltaTime) = 0;` and `virtual void draw() = 0;`.
- **Dynamic Polymorphism**: The `Level` class maintains a collection of enemy pointers `std::vector<std::unique_ptr<Enemy>>`. During the game loop, the level updates and renders all enemies uniformly:
  ```cpp
  for (const auto& enemy : m_enemies) {
      enemy->update(deltaTime);
      enemy->draw();
  }
  ```
  The specific behavior (e.g., `Guard` patrol AI, chase logic, vision cone rendering) is dispatched dynamically at runtime via C++ virtual method tables (vtables).

### 2.4 Abstraction
- **Separation of Concerns**: High-level game modules interact through clean abstractions without knowledge of internal calculations. For example, `GameManager` triggers `level.update()` and `level.draw()` without needing to know raylib rendering internals or mathematical raycasting formulas for vision cones.
- **AI State Machine Abstraction**: Guard AI is modeled as an enumeration of distinct states (`PATROLLING`, `ALERTED`, `CHASING`, `ATTACKING`, `DEAD`), isolating AI logic from rendering and collision details.

### 2.5 Composition
- **Has-A Relationships**:
  - `Level` *has-a* set of `Obstacle`s (walls, crates).
  - `Level` *has-a* set of `Enemy` pointers.
  - `Level` *has-a* set of `Collectible` coins.
  - `GameManager` *has-a* `Level` and *has-a* `Player`.
- Composition is prioritized over multi-level inheritance trees to promote modularity, clear ownership semantics, and maintainability.

---

## 3. Modern C++ Best Practices (C++17)

- **Smart Pointers & RAII**: Dynamic polymorphic entities (`Enemy`) are managed using `std::unique_ptr` to ensure clear single-ownership semantics and automatic deallocation without manual memory leaks.
- **Rule of Zero / Five**: Resource-owning classes properly define or delete copy/move constructors to prevent shallow copy bugs and dangling pointers.
- **Type Safety**: Strongly-typed `enum class` is used for `GameState` and `GuardState` to eliminate implicit integer conversion bugs.
- **Const Correctness**: Query methods are strictly `const`-qualified (`isRunning() const`, `getHealth() const`).


---

## 4. Phase 3 Architecture: Collision Management & Combat Interactions

### 4.1 CollisionManager and Separation of Concerns
The `CollisionManager` class serves as a pure spatial physics subsystem:
- **Single Responsibility Principle (SRP)**: All geometric intersection tests (Circle vs AABB Rectangle), penetration resolution, and axis-separated wall sliding calculations are strictly centralized in `CollisionManager`.
- **Decoupling**: Neither `Player` nor `Obstacle` performs complex penetration math. `Player::moveAndSlide()` delegates collision resolution to `CollisionManager::resolveMovement()`.
- **Axis-Separated Wall Sliding**: By resolving horizontal (X) and vertical (Y) displacements sequentially against obstacle bounding boxes, the player smoothly slides along walls when moving diagonally into obstacles, eliminating corner sticking.

### 4.2 Interaction Workflow: Player, Guard, and CollisionManager
```
[User Input: WASD / Arrows]
           │
           ▼
     Player::handleInput()
   (Diagonal Normalization)
           │
           ▼
     Player::moveAndSlide()
           │
           ├────────► CollisionManager::resolveMovement()
           │                │ (Checks and resolves against Obstacles)
           │                ▼
           ├────────► CollisionManager::clampToMapBounds()
           │
           ▼
  [Updated Player Position]
           │
           ├────────► Guard::isVulnerableTo(Player)
           │                │ (Checks distance <= 65px AND rear angle <= 70°)
           │                ▼
           │          Guard::setVulnerable(true) ──► Renders Targeting Reticle
           │
           ▼
[Key Press: SPACE] (if vulnerable)
           │
           ├────────► Guard::eliminate()  (State -> DEAD, Stops AI)
           ├────────► Player::addElimination()
           └────────► Player::triggerAssassinationEffect() (Slash animation)
```

### 4.3 Encapsulation & Mathematical Contracts in Assassination
- **Rear-Approach Calculation**: The `Player::isBehindTarget()` method computes the dot product between the normalized vector from the guard to the player $\mathbf{u}_{toPlayer}$ and the negative of the guard's facing vector $-\mathbf{F}_{guard}$:
  $$\cos\alpha = - (\mathbf{u}_{toPlayer} \cdot \mathbf{F}_{guard})$$
  The approach is valid if and only if $\cos\alpha \ge \cos(70^\circ) \approx 0.342$.
- **State Invariance**: Dead guards cannot be assassinated again; `Guard::isVulnerableTo()` immediately verifies `isAlive() && m_state != GuardState::DEAD` before conducting spatial tests.


---

## 5. Phase 4 Architecture: Guard AI Finite State Machine & Sensory Raycasting

### 5.1 Finite State Machine (FSM) Pattern in Guard
The Guard AI uses a cleanly encapsulated Finite State Machine (FSM) to model enemy behavior:

```
              ┌───────────────┐
              │   PATROLLING  │◄─────────────────────────────┐
              └───────┬───────┘                              │
                      │ canSeePlayer()                       │ Search Timeout (> 2.2s)
                      ▼                                      │
              ┌───────────────┐                              │
              │    ALERTED    │                              │
              └───────┬───────┘                              │
                      │ Timer >= 0.6s                        │
                      ▼                                      │
              ┌───────────────┐                              │
       ┌─────►│    CHASING    ├──────────────────────────────┘
       │      └───────┬───────┘
       │              │ distToPlayer <= attackDistance (34px)
Player │              ▼
Escapes│      ┌───────────────┐
Range  └──────┤   ATTACKING   │
              └───────┬───────┘
                      │
                      │ Player assassinated / Guard takes lethal damage
                      ▼
              ┌───────────────┐
              │     DEAD      │ (Terminal State: All AI halted)
              └───────────────┘
```

- **PATROLLING**: The guard moves along a sequential list of waypoints (`m_patrolPoints`). When within 6.0px of the target waypoint, the guard advances to the next waypoint (`(index + 1) % size`) and enters a brief 0.4s pause to simulate realistic guard pacing. Facing angle smoothly aligns with movement direction.
- **ALERTED**: Upon detecting the player, the guard immediately turns to face the player, logs the target's position (`m_lastKnownPlayerPos`), and displays a red `!` visual badge. A configurable reaction delay (0.6s) gives the player a brief stealth window before pursuit commences.
- **CHASING**: The guard accelerates to 1.55x speed and pursues the player using wall sliding physics. If line of sight is broken (player ducks behind cover), the guard navigates directly to `m_lastKnownPlayerPos`. Upon reaching the last known coordinate, the guard enters search mode, rotating to sweep the area. If no player is found within 2.2s, the guard resumes `PATROLLING`.
- **ATTACKING**: Once within melee striking distance ($\le 34\text{ px}$), the guard attacks. Attacks are regulated by `m_attackCooldown` (0.85s interval) and apply damage via `player.takeDamage(m_attackDamage)`. A visual slash line provides instantaneous combat feedback.
- **DEAD**: Irreversible terminal state. Guard drops to the floor with neutralized coloring, disables all vision cones, cancels all attacks, and ceases all physics processing.

### 5.2 Sensory Perception: Vector Vision Sector Math
A target position $\mathbf{P}$ is inside a guard's vision cone if and only if two geometric conditions are satisfied:
1. **Distance Condition**:
   $$\|\mathbf{P} - \mathbf{G}\| \le R_{vision}$$
2. **Angular Sector Condition**:
   Let $\theta_{target} = \text{atan2}(P_y - G_y, P_x - G_x) \times \frac{180}{\pi}$.
   The signed angular difference relative to the guard's facing angle $\theta_{guard}$ is:
   $$\Delta\theta = \text{wrap}_{[-180, 180]}(\theta_{target} - \theta_{guard})$$
   The target is inside the field of view if:
   $$|\Delta\theta| \le \frac{\theta_{vision}}{2}$$

### 5.3 Determinant-Based Line-of-Sight (LoS) Raycasting
Even if a player is within the angular vision sector, solid obstacles block detection. This is handled by `CollisionManager::checkLineOfSight()`.

A 2D ray from guard center $\mathbf{A}$ to player center $\mathbf{B}$ is tested against all 4 edges of each obstacle bounding box:
$$\mathbf{p}(t) = \mathbf{p}_1 + t(\mathbf{p}_2 - \mathbf{p}_1), \quad \mathbf{q}(u) = \mathbf{q}_1 + u(\mathbf{q}_2 - \mathbf{q}_1)$$

The segments intersect if and only if:
$$\text{det} = (\mathbf{p}_2 - \mathbf{p}_1) \times (\mathbf{q}_2 - \mathbf{q}_1) \ne 0$$
and both parameters $t, u \in [0, 1]$.

If any intersection is found with an obstacle that has `blocksVision() == true`, the line of sight is obstructed and detection fails.


---

## 6. Phase 5 Architecture: Level Composite Pattern & Game Lifecycle Coordination

### 6.1 Composite Pattern & Clear Ownership Semantics
The `Level` class demonstrates the **Composite Pattern** and strict RAII memory management:
- **Strong Ownership**: A `Level` owns collections of `Obstacle`, `Collectible`, and polymorphic `Guard` entities via `std::vector<std::unique_ptr<Guard>>`. When a `Level` is destroyed or reset, all child entities are automatically and cleanly deallocated.
- **Copy Prevention**: `Level` explicitly deletes its copy constructor and copy assignment operator (`delete`) to prevent accidental shallow slicing or duplication of `std::unique_ptr` resources, while allowing move semantics (`Level(Level&&) noexcept = default`).
- **Encapsulated Queries**: `GameManager` interacts with the level via clean query interfaces (`areAllGuardsEliminated()`, `getRemainingGuardsCount()`, `getObstacles()`, `getGuards()`, `getCoins()`) without mutating the underlying data structures directly.

```
       +-------------------------------------------------+
       |                  GameManager                    |
       +-------------------------------------------------+
       | - m_currentState: GameState                     |
       | - m_player: unique_ptr<Player>                  |
       | - m_currentLevel: unique_ptr<Level>             |
       +-------------------------------------------------+
                                │ owns (1:1)
                                ▼
       +-------------------------------------------------+
       |                     Level                       |
       +-------------------------------------------------+
       | - m_levelNumber: int (1, 2, 3)                  |
       | - m_playerSpawn: Vector2                        |
       | - m_obstacles: vector<Obstacle>                 |
       | - m_guards: vector<unique_ptr<Guard>>           |
       | - m_coins: vector<Collectible>                  |
       +-------------------------------------------------+
         │ (1:N)                 │ (1:N)                 │ (1:N)
         ▼                       ▼                       ▼
    +----------+         +---------------+         +-------------+
    | Obstacle |         |     Guard     |         | Collectible |
    +----------+         +---------------+         +-------------+
```

### 6.2 Separation of Concerns: Level vs GameManager
- **Level Responsibility**:
  1. Instantiating handcrafted, deterministic maps for Level 1 (*Training Ground*), Level 2 (*Restricted Facility*), and Level 3 (*Final Mission*).
  2. Encapsulating spatial queries, player spawn coordinates, and entity collections.
  3. Dispatching batched `update(deltaTime, player)` and `draw()` across obstacles, coins, and guards.
  4. Providing idempotent `reset()` capability restoring all original state.
- **GameManager Responsibility**:
  1. Window lifecycle, frame timing, and user input routing.
  2. Managing the high-level Finite State Machine across `PLAYING`, `LEVEL_COMPLETE`, `GAME_OVER`, and `VICTORY`.
  3. Coordinating cross-entity interactions (e.g., player assassinating guards, player collecting coins).
  4. Handling campaign progression and displaying modal cards for mission victory and failure.

### 6.3 Deterministic Handcrafted Level Specifications
Rather than relying on procedural generation which can create unreachable coins or trapped guards, each level is hand-tailored:
- **Level 1 (Training Ground)**: Focuses on core mechanics. 2 guards with predictable linear/box patrols; wide corridors allowing easy stealth approaches from behind.
- **Level 2 (Restricted Facility)**: Introduces 3 guards with intersecting patrol zones. Tighter corridors, blind corners, and 6 coins placed in high-risk zones.
- **Level 3 (Final Mission)**: Maximum tactical challenge with 4 elite guards, a central computer mainframe blocking line of sight, and 7 strategic coins.


---

## 7. Phase 6 Architecture: User Interface (UI) Decoupling & View Separation

### 7.1 Single Responsibility Principle (SRP) in UIManager
In Phase 6, all graphical presentation for menus, heads-up displays (HUD), modal overlays, and visual feedback is encapsulated inside the `UIManager` class.
- **View vs Controller**: `UIManager` represents the *View* layer. It does not mutate player inventory, entity positions, or level structures.
- **Controller**: `GameManager` represents the *Controller*. It manages lifecycle loops, processes physical events, and determines *when* state transitions should occur.
- **Model**: `Player`, `Guard`, `Obstacle`, and `Level` represent the *Model*, strictly encapsulating spatial and gameplay simulation rules.

### 7.2 Decoupled Action Routing via `UIAction`
To eliminate tight coupling between UI widgets and game state machines, all interactive screen rendering functions return a strongly-typed `UIAction` enum:

```
[User Clicks Button / Presses Key]
                 │
                 ▼
       UIManager::drawMainMenu()
                 │ Returns UIAction::START_GAME
                 ▼
       GameManager::draw()
                 │ Evaluates action
                 ▼
       GameManager::loadLevel(1)
                 │ Sets m_currentState = GameState::PLAYING
                 ▼
      [Transitions to Gameplay]
```

This guarantees:
1. `UIManager` does not require references to `GameManager` (zero circular dependencies).
2. UI rendering can be unit tested independently of window lifecycles.
3. Both mouse clicks and keyboard shortcuts map onto the identical execution paths.

### 7.3 Reusable Widget Components & Stateless Hit-Testing
The `UIManager::drawButton()` method is a stateless, pure function:
- Computes hover state via `CheckCollisionPointRec(mousePos, bounds)`.
- Applies dynamic visual feedback (brightened hover fill, glowing border).
- Automatically centers text using `MeasureText()`.
- Returns `isHovered && mouseClicked` to signal actuation.

### 7.4 Non-Intrusive Tactical HUD Design
The HUD uses a dual-dock architecture:
- **Top Mission Bar (48px)**: Level index, mission name, and engine FPS.
- **Playable Field (655px)**: Unobstructed view of operative, walls, vision cones, and guards.
- **Bottom Status Dock (65px)**: Segmented color-shifting health bar, gold intel coin counter, remaining guards counter, elimination tally, and control reminders.


---

## 8. Phase 7 Architecture: Integration Verification, Checkpoint Persistence & Memory Safety

### 8.1 Checkpoint Rollback Pattern (Anti-Farming Protection)
To maintain consistent game economy invariants while allowing convenient sector retries, `GameManager` implements a lightweight Memento-style checkpoint pattern:
- When advancing into a new sector ($N > 1$), `m_checkpointCoins` and `m_checkpointEliminations` record the player's starting metrics.
- If the player is neutralized and chooses "RETRY SECTOR", `restartCurrentLevel()` restores the player's balance to the exact checkpoint values:
  ```cpp
  m_player->setCoins(m_checkpointCoins);
  m_player->setEliminations(m_checkpointEliminations);
  m_player->resetHealth();
  ```
- This prevents coin duplication and score inflation exploits from repeated retries.

### 8.2 Defensive Boundary Invariants
In Phase 7, boundary clipping invariants were formalized:
- Minimum Y is clamped to `50.0f` across both player movement and enemy AI routines, preventing any entity from entering the top mission banner.
- Obstacle line-of-sight raycasting enforces `blocksVision() == true` geometry, guaranteeing that dead guards cannot detect, pursue, or attack players.

### 8.3 Memory Safety & Sanitizer Validation
All dynamic resources in Shadow Assassin strictly follow **Resource Acquisition Is Initialization (RAII)**:
- Zero raw `new` / `delete` calls in runtime application code.
- Dynamic entity pointers are stored exclusively within `std::unique_ptr<Guard>` and `std::unique_ptr<Player>`.
- The comprehensive integration test was compiled and executed under `-fsanitize=address,undefined`. The AddressSanitizer and UndefinedBehaviorSanitizer verified:
  1. **Zero Memory Leaks**: All level objects, obstacle lists, and UI allocations are destroyed cleanly on shutdown or level change.
  2. **Zero Use-After-Free**: Level switches replace `std::unique_ptr<Level>` without leaving dangling references in `GameManager` or `UIManager`.
  3. **Zero Buffer Overflows**: All raymath vectors and obstacle bounding checks are strictly bounded.
