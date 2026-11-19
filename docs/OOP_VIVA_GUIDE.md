# OOP Viva Voce & Interview Defense Guide
## Project: Shadow Assassin (C++17 2D Stealth Game)

---

# PART A: CORE OOP FUNDAMENTALS & C++ LANGUAGE CONCEPTS

---

### 1. Class vs. Object
- **Class**: A user-defined blueprint or type specification that defines data members (state) and member functions (behavior). It occupies no runtime storage for instances until instantiated (except static members and vtables).
- **Object**: A concrete runtime instance of a class occupying a specific block of memory on the stack or heap.
- **Example in Code**:
  ```cpp
  class Obstacle { ... };      // Class declaration (blueprint)
  Obstacle wall(10, 20, 50, 100); // Object instantiation (occupies memory on stack)
  ```

---

### 2. Encapsulation & Data Hiding
- **Encapsulation**: The bundling of data attributes and the methods that operate upon them into a single cohesive unit (a class), restricting direct outside access to internal representations.
- **Data Hiding**: Enforcing access control via access specifiers (`private`, `protected`) so that internal state cannot be directly corrupted by external code, preserving class invariants.
- **Example in Code**:
  ```cpp
  class Player : public Entity {
  private:
      int m_health; // Cannot be modified directly by external code
  public:
      void takeDamage(int amount) {
          m_health = std::max(0, m_health - amount); // Enforces invariant (health >= 0)
          if (m_health == 0) m_active = false;
      }
      int getHealth() const { return m_health; } // Read-only query
  };
  ```

---

### 3. Abstraction
- **Definition**: Abstraction separates *interface* from *implementation*. It presents essential features to the outside world while hiding underlying algorithmic and hardware complexities.
- **Mechanism in C++**: Pure virtual functions (`= 0`) in abstract base classes define behavioral contracts without dictating how derived classes fulfill them.
- **Example in Code**:
  ```cpp
  class Entity {
  public:
      virtual void update(float deltaTime) = 0; // Interface contract
      virtual void draw() const = 0;
  };
  // Caller simply executes:
  entityPtr->update(dt); // Caller does not need to know concrete physics/AI logic
  ```

---

### 4. Inheritance
- **Definition**: A mechanism where a derived (child) class inherits attributes and member functions from a base (parent) class, promoting code reuse and establishing taxonomic "is-a" relationships.
- **Inheritance Types**:
  1. **Single Inheritance**: Derived class inherits from one base class (e.g., `Player` : `Entity`).
  2. **Multilevel Inheritance**: A class is derived from another derived class (e.g., `Guard` : `Enemy` : `Entity`).
  3. **Hierarchical Inheritance**: Multiple derived classes inherit from a single base class (e.g., `Player` : `Entity` and `Enemy` : `Entity`).
  4. **Multiple Inheritance**: A class inherits from two or more base classes simultaneously (e.g., `class Hybrid : public ClassA, public ClassB`).
- **The Diamond Problem**: When two classes inherit from a common base class, and a fourth class inherits from both, duplicate copies of the common base exist, causing ambiguity. Resolved in C++ using **virtual inheritance** (`class B : virtual public A`).

---

### 5. Polymorphism (Static vs. Dynamic)
- **Compile-Time (Static) Polymorphism**: Resolved at compile time.
  - *Function Overloading*: Same function name with different parameter types/signatures.
  - *Operator Overloading*: Providing custom semantics for language operators on user-defined types.
  - *Templates*: Parametric polymorphism allowing generic functions and classes.
- **Run-Time (Dynamic) Polymorphism**: Resolved at runtime via virtual functions and dynamic dispatch.
  - Requires base class pointer/reference pointing to derived instance.
  - Methods marked `virtual` in base class and `override` in derived class.
- **Example in Code**:
  ```cpp
  Entity* entity = new Guard(100.0f, 100.0f);
  entity->update(0.016f); // Calls Guard::update() at runtime via vtable
  ```

---

### 6. Virtual Tables (`vtable`) and Virtual Table Pointers (`vptr`)
- **How Dynamic Dispatch Works**:
  1. For any class declaring or inheriting at least one virtual function, the compiler constructs a static array of function pointers known as the **vtable** (`virtual table`).
  2. Every instance of that class is invisibly augmented with a hidden pointer, the **`vptr`**, typically placed at byte offset 0 of the object.
  3. When the constructor executes, it initializes the `vptr` to point to the class's vtable.
  4. When invoking `basePtr->virtualMethod()`, the compiler emits assembly that dereferences `basePtr->vptr`, indexes into the table slot for `virtualMethod`, and executes an indirect call instruction (`call [rax + offset]`).
- **Memory Overhead**: 1 pointer (8 bytes on 64-bit systems) per object instance for the `vptr`, plus one table per polymorphic class.

---

### 7. Pure Virtual Functions and Abstract Classes
- **Pure Virtual Function**: A virtual function declared with `= 0` syntax, having no required implementation in the base class:
  ```cpp
  virtual void draw() const = 0;
  ```
- **Abstract Class**: Any class containing at least one pure virtual function. It **cannot be instantiated** directly (`Entity e;` produces a compile-time error).
- **Purpose**: Defines an abstract interface that all derived classes must implement to become concrete instantiable types.

---

### 8. Virtual Destructors
- **Why Base Classes Require Virtual Destructors**:
  When deleting a derived object through a base class pointer:
  ```cpp
  Entity* e = new Player();
  delete e;
  ```
  If `Entity`'s destructor is non-virtual, the compiler performs static binding and calls *only* `~Entity()`. The derived class destructor `~Player()` never executes. This leads to **resource leaks** and **undefined behavior** under ISO C++.
- **Rule**: Any class with at least one virtual function must declare a `virtual ~Base() = default;`.

---

### 9. Access Specifiers in C++
- **Member Access Specifiers**:
  - `public`: Accessible from any code in the program.
  - `protected`: Accessible only within the declaring class and its derived classes.
  - `private`: Accessible only within the declaring class itself (and designated `friend` classes/functions).
- **Inheritance Access Specifiers**:
  - `class Derived : public Base`: Public members stay public; protected stay protected.
  - `class Derived : protected Base`: Public and protected members become protected.
  - `class Derived : private Base`: Public and protected members become private.

---

### 10. Constructors, Destructors, Copy/Move Semantics & RAII
- **RAII (Resource Acquisition Is Initialization)**: Binding the lifecycle of a resource (heap memory, file handles, window contexts) to the lifetime of a stack-allocated object. When the enclosing object goes out of scope, its destructor deterministically releases the resource.
- **Rule of Zero / Three / Five**:
  - *Rule of Zero*: Rely on standard RAII wrappers (`std::unique_ptr`, `std::vector`, `std::string`) so you do not need custom destructors, copy, or move operations.
  - *Rule of Five*: If you manually define a Destructor, Copy Constructor, Copy Assignment, Move Constructor, or Move Assignment, you should explicitly declare or delete all five.
- **In Shadow Assassin**: Classes like `Level` and `GameManager` enforce unique resource ownership by deleting copy operations and declaring move operations:
  ```cpp
  Level(const Level&) = delete;
  Level& operator=(const Level&) = delete;
  Level(Level&&) noexcept = default;
  Level& operator=(Level&&) noexcept = default;
  ```

---

### 11. Composition vs. Inheritance ("has-a" vs. "is-a")
- **Inheritance ("is-a")**: High coupling. Derived classes inherit base implementation, creating fragile base class problems if overused. Best for genuine behavioral taxonomy (e.g., `Guard` is-an `Enemy`).
- **Composition ("has-a")**: Low coupling. Enclosing class contains instances or smart pointers to other classes. Allows assembling complex systems from simple, interchangeable components.
- **Principle**: *"Favor composition over inheritance."* In Shadow Assassin, `Level` is *not* a child of `Obstacle` or `Guard`; it *has* a collection of `Obstacle`s and `Guard`s.

---

### 12. SOLID Principles in C++
- **S - Single Responsibility Principle (SRP)**: A class should have one, and only one, reason to change. (e.g., `CollisionManager` handles physics/raycasts only; `UIManager` handles UI rendering only).
- **O - Open/Closed Principle (OCP)**: Software entities should be open for extension, but closed for modification. (e.g., New enemy types can extend `Enemy` without changing `GameManager` or `CollisionManager`).
- **L - Liskov Substitution Principle (LSP)**: Objects of a derived class must be substitutable for objects of the base class without altering program correctness. (e.g., `Guard` can be treated as `Enemy` or `Entity` everywhere).
- **I - Interface Segregation Principle (ISP)**: Clients should not be forced to depend on interfaces they do not use.
- **D - Dependency Inversion Principle (DIP)**: High-level modules should not depend on low-level modules; both should depend on abstractions.

---

# PART B: PROJECT-SPECIFIC OOP IMPLEMENTATION BREAKDOWN

---

## 1. Class: `Entity`
- **Location**: `include/Entity.h`, `src/Entity.cpp`
- **Role**: Abstract root base class for all interactive game entities.
- **Key Members**:
  - `Vector2 m_position`: Spatial coordinate (top-left or center).
  - `Vector2 m_size`: Bounding box dimensions (width, height).
  - `bool m_active`: Participation flag in gameplay/rendering loop.
  - `Color m_color`: Base visual tint.
- **Key Methods**:
  - `virtual void update(float deltaTime) = 0` (Pure virtual)
  - `virtual void draw() const = 0` (Pure virtual)
  - `virtual ~Entity() = default` (Virtual destructor)
  - `Rectangle getBounds() const`, `float distanceTo(...) const`
- **OOP Principles Demonstrated**:
  - **Abstraction**: Pure virtual methods define an entity contract.
  - **Encapsulation**: Protected state with public validated getters/setters.
  - **Polymorphism**: Declares virtual destructor and polymorphic methods.

---

## 2. Class: `Player`
- **Location**: `include/Player.h`, `src/Player.cpp`
- **Role**: Operative character controlled by user inputs.
- **Inheritance**: `class Player : public Entity`
- **Key Members**:
  - `int m_health`, `m_maxHealth`: Vital statistics.
  - `float m_speed`: Movement velocity scalar ($190.0\text{ px/s}$).
  - `Vector2 m_velocity`: Current normalized directional velocity.
  - `float m_facingAngle`: Rotation angle in degrees.
  - `int m_coinCount`, `m_eliminationCount`: Campaign metrics.
  - `float m_assassinationTimer`, `m_hitFlashTimer`: Visual feedback timers.
- **Key Methods**:
  - `void update(float deltaTime) override`, `void draw() const override`
  - `void handleInput()`: Translates WASD/Arrow keys into normalized velocity.
  - `void moveAndSlide(...)`: Resolves collisions against obstacles via `CollisionManager`.
  - `bool canAssassinate(...) const`, `bool isBehindTarget(...) const`: Validates rear approach.
  - `void takeDamage(int amount)`, `void heal(int amount)`, `void resetHealth()`
- **OOP Principles Demonstrated**:
  - **Inheritance & Polymorphism**: Overrides `update()` and `draw()`.
  - **Encapsulation**: Private combat and movement state with protected invariants.
  - **Separation of Concerns**: Delegates collision solving to `CollisionManager`.

---

## 3. Class: `Enemy`
- **Location**: `include/Enemy.h`, `src/Enemy.cpp`
- **Role**: Abstract intermediate base class for all hostile entities.
- **Inheritance**: `class Enemy : public Entity`
- **Key Members**:
  - `int m_health`, `m_maxHealth`: Enemy health pool.
  - `float m_speed`: Patrol/movement velocity.
  - `float m_facingAngle`: Orientation in degrees.
- **Key Methods**:
  - `void update(float) override = 0`: Remains pure virtual.
  - `void draw() const override = 0`: Remains pure virtual.
  - `virtual void takeDamage(int amount)`: Deducts health and updates alive status.
  - `virtual void onAlerted(Vector2 targetPos)`: Polymorphic hook for alert reaction.
  - `virtual void attack()`: Polymorphic combat execution hook.
- **OOP Principles Demonstrated**:
  - **Hierarchical Inheritance**: Intermediate node between `Entity` and `Guard`.
  - **Open/Closed Principle**: Future enemy types (e.g., snipers, drones) can extend `Enemy` without rewriting combat logic.

---

## 4. Class: `Guard`
- **Location**: `include/Guard.h`, `src/Guard.cpp`
- **Role**: Autonomous patrol guard implementing an AI Finite State Machine.
- **Inheritance**: `class Guard : public Enemy` (Multilevel: `Guard` $\to$ `Enemy` $\to$ `Entity`)
- **Key Members**:
  - `GuardState m_state`: Current FSM state (`PATROLLING`, `ALERTED`, `CHASING`, `ATTACKING`, `DEAD`).
  - `std::vector<Vector2> m_patrolPoints`: Waypoint route.
  - `size_t m_currentWaypointIndex`: Active destination waypoint.
  - `float m_visionRadius` ($190\dots 220\text{ px}$), `m_visionAngle` ($55^\circ\dots 65^\circ$).
  - `float m_attackCooldown` ($0.85\text{ s}$), `int m_attackDamage` ($15\text{ HP}$).
  - `Vector2 m_lastKnownPlayerPos`: Coordinate investigated during search.
- **Key Methods**:
  - `void updateAI(float dt, Player& player, const std::vector<Obstacle>& obstacles, ...)`
  - `bool canSeePlayer(const Player& player, const std::vector<Obstacle>& obstacles) const`
  - `bool isVulnerableTo(const Player& player, float maxDist, float rearTolerance) const`
  - `void updatePatrol(...)`, `void updateAlerted(...)`, `void updateChase(...)`, `void updateAttack(...)`
- **OOP Principles Demonstrated**:
  - **State Pattern / FSM**: State transitions governed by internal member methods.
  - **Multilevel Inheritance**: Extends `Enemy` and implements pure virtuals of `Entity`.
  - **Information Hiding**: Sub-state update routines are strictly private.

---

## 5. Class: `Obstacle`
- **Location**: `include/Obstacle.h`, `src/Obstacle.cpp`
- **Role**: Solid environmental barriers, crates, and walls.
- **Key Members**:
  - `Rectangle m_bounds`: Axis-aligned bounding box coordinates.
  - `bool m_blocksVision`: Flag indicating optical occlusion (default `true`).
  - `Color m_color`: Tactical barrier color.
- **Key Methods**:
  - `bool contains(Vector2 point) const`
  - `bool collidesWithCircle(Vector2 center, float radius) const`
  - `bool collidesWithRect(Rectangle other) const`
  - `void draw() const`: Renders beveled tactical barrier with drop shadows.
- **OOP Principles Demonstrated**:
  - **Encapsulation**: State is private; queries provide read-only geometric answers.
  - **Modularity**: Self-contained geometric primitive usable by any system.

---

## 6. Class: `Collectible`
- **Location**: `include/Collectible.h`, `src/Collectible.cpp`
- **Role**: Optional bonus intelligence coins scattered across sectors.
- **Key Members**:
  - `Vector2 m_position`: World coordinate.
  - `float m_radius`: Collection radius ($9.0\text{ px}$).
  - `int m_value`: Score worth (default $1$).
  - `bool m_isCollected`: Collection status flag.
  - `float m_animTimer`: Timer for sinusoidal gold shimmer effect.
- **Key Methods**:
  - `bool checkCollision(Vector2 entityCenter, float entityRadius) const`
  - `void collect()`, `void reset()`
  - `void update(float deltaTime)`, `void draw() const`
- **OOP Principles Demonstrated**:
  - **Encapsulation**: State mutations restricted to `collect()` and `reset()`.
  - **Self-Contained Rendering**: Handles own animation timers and visual pulses.

---

## 7. Class: `CollisionManager`
- **Location**: `include/CollisionManager.h`, `src/CollisionManager.cpp`
- **Role**: Pure static utility class for computational geometry, collision solving, and raycasting.
- **Key Methods**:
  - `static bool checkCircleRect(Vector2 center, float radius, Rectangle rect)`
  - `static Vector2 resolveCircleRect(Vector2 center, float radius, Rectangle rect)`
  - `static Vector2 resolveMovement(...)`: Enables axis-separated wall sliding.
  - `static Vector2 clampToMapBounds(...)`: Clamps entities to playable bounds ($Y \ge 50$).
  - `static bool checkLineOfSight(Vector2 start, Vector2 end, const std::vector<Obstacle>& obstacles)`
  - `static bool checkLineIntersection(Vector2 a1, Vector2 a2, Vector2 b1, Vector2 b2, Vector2* hitPoint)`
- **OOP Principles Demonstrated**:
  - **Single Responsibility Principle (SRP)**: Isolates pure physics and mathematical calculations from game entities and managers.
  - **Stateless Utility**: Declares static methods without holding persistent mutable state, eliminating side effects.

---

## 8. Class: `Level`
- **Location**: `include/Level.h`, `src/Level.cpp`
- **Role**: Composite container and lifecycle manager for a discrete gameplay mission.
- **Composition Members**:
  - `std::vector<Obstacle> m_obstacles`
  - `std::vector<std::unique_ptr<Guard>> m_guards`
  - `std::vector<Collectible> m_coins`
  - `Vector2 m_playerSpawn`: Level spawn point.
  - `std::string m_name`, `m_description`: Level metadata.
- **Key Methods**:
  - `void load(int levelNumber)`: Handcrafted layouts for Levels 1, 2, 3, 4, and 5.
  - `void reset()`: Restores guards to life, waypoints, and uncollected coins.
  - `void update(float deltaTime, Player& player)`: Propagates ticks to all owned entities.
  - `void draw() const`: Renders all obstacles, coins, and guard vision cones.
  - `bool areAllGuardsEliminated() const`, `int getRemainingGuardsCount() const`
- **OOP Principles Demonstrated**:
  - **Composition**: Strongly owns entity collections; memory is released automatically upon level destruction.
  - **Smart Pointers & Polymorphism**: Manages guards via `std::unique_ptr<Guard>` to enforce single ownership and prevent memory leaks.
  - **Move Semantics**: Copy operations deleted; move operations enabled.

---

## 9. Class: `UIManager`
- **Location**: `include/UIManager.h`, `src/UIManager.cpp`
- **Role**: Decoupled presentation engine managing HUD, menus, buttons, and visual feedback.
- **Key Members**:
  - `int m_screenWidth`, `m_screenHeight`: Viewport resolution.
  - `std::vector<UINotification> m_notifications`: Active floating feedback pool.
- **Key Methods**:
  - `void drawHUD(...) const`: Renders dual-dock header and footer telemetry.
  - `UIAction drawMainMenu(...) const`, `UIAction drawPauseMenu(...) const`
  - `UIAction drawLevelComplete(...) const`, `UIAction drawGameOver(...) const`, `UIAction drawVictory(...) const`
  - `static bool drawButton(...)`: Reusable UI button widget with hover/click detection.
  - `void addNotification(...)`, `void update(float dt)`, `void drawNotifications() const`
- **OOP Principles Demonstrated**:
  - **Single Responsibility Principle (SRP)**: Strictly isolates visual layout and widget hit-testing from gameplay simulation logic.
  - **Headless Safety**: Safeguards all drawing with `IsWindowReady()` to allow headless automated testing without OpenGL context crashes.

---

## 10. Class: `GameManager`
- **Location**: `include/GameManager.h`, `src/GameManager.cpp`
- **Role**: Master coordinator managing application lifecycle, state flow, and level transitions.
- **Composed Systems**:
  - `std::unique_ptr<Player> m_player`
  - `std::unique_ptr<Level> m_currentLevel`
  - `std::unique_ptr<UIManager> m_uiManager`
  - `GameState m_currentState` (`MENU`, `PLAYING`, `PAUSED`, `LEVEL_COMPLETE`, `GAME_OVER`, `VICTORY`)
  - `int m_checkpointCoins`, `m_checkpointEliminations`: Anti-farming checkpoint tracking.
- **Key Methods**:
  - `bool init()`, `void run()`, `void update(float dt)`, `void draw()`, `void shutdown()`
  - `void loadLevel(int levelNumber)`, `void advanceToNextLevel()`, `void restartCurrentLevel()`
  - `void pauseGame()`, `void resumeGame()`, `void returnToMainMenu()`
- **OOP Principles Demonstrated**:
  - **High-Level Orchestration (Mediator Pattern)**: Coordinates data flow between Player, Level, and UIManager without creating circular dependencies.
  - **RAII Lifecycle**: Manages window initialization and deterministic teardown.

---

# PART C: LIKELY VIVA QUESTIONS & MODEL ANSWERS (40 QUESTIONS)

---

### Category 1: General OOP & C++ Language Concepts

#### Q1: What is the difference between an abstract class and an interface in C++?
**Answer**: C++ has no dedicated `interface` keyword. An abstract class is any class containing at least one pure virtual function (`virtual void f() = 0`). A pure interface is an abstract class with *only* pure virtual functions, no member variables, and a virtual destructor. In Shadow Assassin, `Entity` is an abstract base class because it declares pure virtual methods (`update`, `draw`), but it also encapsulates state common to all entities (`m_position`, `m_size`, `m_active`).

#### Q2: Why is a virtual destructor necessary in `Entity`? What happens if you remove it?
**Answer**: If a base class has a non-virtual destructor, deleting a derived instance through a base pointer (`Entity* ptr = new Player(); delete ptr;`) invokes undefined behavior under the C++ standard. In practice, only `~Entity()` would be called, skipping `~Player()`. Any resources allocated in `Player` would leak. Marking `virtual ~Entity() = default;` ensures the virtual table resolves destruction dynamically, executing `~Player()` first, followed by `~Entity()`.

#### Q3: What is the difference between shallow copy and deep copy? How does this project handle copying?
**Answer**: A shallow copy duplicates member variable values as-is (copying raw pointer addresses rather than the underlying allocated memory), leading to double-free errors when both destructors run. A deep copy allocates new heap storage and duplicates the underlying data. In Shadow Assassin, we eliminate copy risks entirely by following the **Rule of Zero/Five**: polymorphic classes like `Level` and `GameManager` explicitly delete their copy constructors and copy assignment operators (`= delete`), allowing only move semantics or smart pointer management.

#### Q4: What is object slicing in C++, and how did you prevent it?
**Answer**: Object slicing occurs when an instance of a derived class is assigned or passed by value to an object of a base class. The derived-specific members and vtable are "sliced off," leaving only the base portion. In our project, object slicing is prevented by:
1. Always passing polymorphic entities by reference (`const Player& player`) or via smart pointers (`std::unique_ptr<Guard>`).
2. Declaring copy operations explicitly or deleting them in base classes.

#### Q5: Explain the difference between compile-time and runtime polymorphism with examples from your code.
**Answer**: Compile-time polymorphism is resolved during compilation:
- *Overloading*: `Entity::setPosition(Vector2)` vs `Entity::setPosition(float, float)`.
- *Templates*: `std::max(0, m_health - amount)`.
Runtime polymorphism is resolved at execution time via vtables:
- Calling `guard->update(deltaTime)` inside `Level::update()`, where `guard` is stored as `std::unique_ptr<Guard>` and dynamically calls `Guard::update()`.

#### Q6: What is a `vtable`, and where is it stored in memory?
**Answer**: A `vtable` is a static table of function pointers created by the compiler for every class containing virtual functions. It is typically stored in the read-only data segment (`.rodata`) of the executable. Each object instance of that class contains an invisible pointer (`vptr`) pointing to this table in memory.

#### Q7: What are `std::unique_ptr` and `std::make_unique`? Why use them over raw pointers?
**Answer**: `std::unique_ptr` is a modern C++ smart pointer providing exclusive ownership semantics. It cannot be copied, only moved. When the `unique_ptr` goes out of scope, it automatically invokes `delete` on the owned object, preventing memory leaks and dangling pointers. In `Level`, guards are stored as `std::vector<std::unique_ptr<Guard>>`, guaranteeing clean memory deallocation without manual loops.

#### Q8: What does the `override` keyword do in C++17? What happens if you omit it?
**Answer**: `override` is a contextual keyword that instructs the compiler to verify that the member function actually overrides a virtual function in a base class. If there is a typo or signature mismatch (e.g., `void update(double)` instead of `void update(float)`), the compiler throws a compile-time error. Omitting it still compiles, but silent bugs can occur if the signatures do not match exactly.

#### Q9: What is the purpose of `const` member functions (e.g., `void draw() const`)?
**Answer**: A `const` member function guarantees that it will not modify any non-mutable member variables of the calling instance (`this` becomes `const ClassName*`). It allows the method to be safely invoked on `const` references (e.g., in `Level::draw() const` or `CollisionManager::checkLineOfSight(...)`). It enforces compiler-checked immutability.

#### Q10: What is RAII, and where is it applied in Shadow Assassin?
**Answer**: Resource Acquisition Is Initialization (RAII) ensures resources are acquired during construction and released during destruction. Examples in Shadow Assassin:
1. Window management: `GameManager::init()` initializes raylib's window; `GameManager::~GameManager()` invokes `shutdown()`, closing the window cleanly.
2. Dynamic entities: `std::unique_ptr<Guard>` in `Level` automatically reclaims heap memory when the level is unloaded or reloaded.

#### Q11: What is the difference between `protected` and `private` inheritance?
**Answer**: In `protected` inheritance, public and protected members of the base class become `protected` in the derived class. In `private` inheritance, they become `private`. In Shadow Assassin, we exclusively use `public` inheritance (e.g., `class Player : public Entity`) to preserve the true "is-a" relationship, adhering to the Liskov Substitution Principle.

#### Q12: Why did you make `CollisionManager` methods `static`?
**Answer**: `CollisionManager` performs pure mathematical computations (circle-rect intersection, vector line determinant tests, wall sliding). It requires no internal mutable state or instance data. Making methods `static` allows callers to invoke `CollisionManager::checkLineOfSight(...)` directly without allocating or passing a manager instance, achieving zero memory overhead and adhering to SRP.

#### Q13: What is the Diamond Problem, and does your project exhibit it?
**Answer**: The Diamond Problem arises in multiple inheritance when a class derives from two classes that both inherit from a single common base. Shadow Assassin completely avoids multiple inheritance by utilizing a strictly linear hierarchy (`Guard` $\to$ `Enemy` $\to$ `Entity`), eliminating diamond ambiguities entirely.

#### Q14: How does move semantics (`std::move`) work, and where is it used in your project?
**Answer**: Move semantics transfers ownership of heap resources from an rvalue (temporary object) to another object without deep-copying memory. In `Level`, we delete copy operations and enable defaulted move constructors:
```cpp
Level(Level&&) noexcept = default;
Level& operator=(Level&&) noexcept = default;
```
This allows levels to be moved into `GameManager` without copying their vectors or allocated guards.

#### Q15: What is an `enum class`, and why is it preferred over a traditional `enum`?
**Answer**: An `enum class` (scoped enumeration) provides two major advantages over plain `enum`:
1. *Scoping*: Enumerators do not pollute the enclosing namespace (`GuardState::ALERTED` instead of `ALERTED`).
2. *Type Safety*: They do not implicitly convert to integers (`int`), preventing accidental arithmetic comparisons between unrelated enum values.

---

### Category 2: Project Architecture & Design Decisions

#### Q16: Walk me through the class hierarchy of your project.
**Answer**: The foundation is `Entity` (abstract base). `Player` derives directly from `Entity`. `Enemy` derives from `Entity` as an abstract base for hostile entities. `Guard` derives from `Enemy`, representing an intelligent patrol agent. Auxiliary classes include `Obstacle` (static walls) and `Collectible` (coins). `Level` acts as a composite owning these entities. `UIManager` handles all rendering, and `GameManager` coordinates the entire application loop.

#### Q17: Why did you separate `UIManager` from `GameManager`?
**Answer**: To uphold the **Single Responsibility Principle**. `GameManager` manages game states, input polling, and campaign flow. If it also handled button hit-testing, fonts, color palettes, and HUD layouts, it would become an unmaintainable "god class." `UIManager` isolates presentation, so visual styles can be changed without risking gameplay simulation bugs.

#### Q18: How does `GameManager` manage transitions between game states?
**Answer**: `GameManager` maintains an `m_currentState` variable of type `GameState` (`MENU`, `PLAYING`, `PAUSED`, `LEVEL_COMPLETE`, `GAME_OVER`, `VICTORY`). In `update()`, it evaluates user inputs and UI interactions. For example, clicking "Start Game" on the main menu calls `loadLevel(1)` and sets the state to `PLAYING`. When all guards are neutralized, `Level::areAllGuardsEliminated()` triggers a transition to `LEVEL_COMPLETE`.

#### Q19: Why are guards stored as `std::unique_ptr<Guard>` in `Level` rather than plain objects (`Guard`)?
**Answer**:
1. *Polymorphic Extensibility*: Storing objects as pointers allows storing derived types (`Guard`, or future `SniperGuard`) in a polymorphic container (`std::vector<std::unique_ptr<Enemy>>`).
2. *Preventing Object Slicing*: A vector of base objects would slice derived classes.
3. *Move-Safe Container*: Resizing a `std::vector` of pointers moves lightweight pointer addresses rather than copying large entity objects.

#### Q20: How does `Level::reset()` work, and why is it important for OOP encapsulation?
**Answer**: `Level::reset()` restores all level objects to their initial sector state. It revives dead guards, clears active chase states back to `PATROLLING`, resets waypoint indices, uncollects coins, and positions the player back at `m_playerSpawn`. By encapsulating this inside `Level`, external classes do not need to know the inner workings or coordinates of individual guards.

#### Q21: What is the "Checkpoint Anti-Farming" system, and how is it implemented?
**Answer**: In game development, players could exploit level retries to farm infinite coins or score. In `GameManager`, when a level loads, we cache `m_checkpointCoins = m_player->getCoins();` and `m_checkpointEliminations`. If the player dies or restarts the level, `restartCurrentLevel()` restores the player's metrics to the cached checkpoint values using `setCoins()` and `setEliminations()`, ensuring economic integrity.

#### Q22: How is the Open/Closed Principle (OCP) demonstrated in your design?
**Answer**: The `Enemy` base class is open for extension but closed for modification. If we want to add a `CameraDrone` enemy with circular flight paths, we simply inherit from `Enemy`, implement `update()` and `draw()`, and add it to `Level`. Neither `Player`, `CollisionManager`, nor `GameManager` would require code modifications to support the new enemy.

#### Q23: Why did you create an explicit `Obstacle` class instead of just using raylib's `Rectangle`?
**Answer**: An `Obstacle` has conceptual attributes beyond just four coordinates: it holds an `m_blocksVision` flag (enabling future transparent glass walls or low barricades that block movement but not vision) and encapsulates custom tactical rendering with borders and drop shadows. Wrapping it in a class provides strong typing and encapsulation.

#### Q24: How did you ensure automated tests could run in headless environments without crashes?
**Answer**: Raylib's drawing functions assume an initialized OpenGL context. Running tests on a headless server without a display causes segmentation faults if draw calls are executed. We added `if (IsWindowReady())` guards across all drawing routines in `UIManager`. This allows test runners (`test_phase6`, `test_phase7`) to test button hit-testing and notification logic headlessly without crashing.

#### Q25: What is the role of `Collectible` in the game architecture?
**Answer**: `Collectible` represents gold intelligence coins. It demonstrates encapsulation by keeping its collection status and bobbing animation timer private. It exposes simple `checkCollision()` and `collect()` interfaces, and cleanly resets when a sector is restarted via `reset()`.

---

### Category 3: Mechanics, AI, Algorithms & Math

#### Q26: Explain the Guard AI Finite State Machine. What are its states and transition triggers?
**Answer**:
1. `PATROLLING`: Default state. Guard moves smoothly along predefined waypoints.
   - *Transition*: If `canSeePlayer()` returns true $\to$ `ALERTED`.
2. `ALERTED`: Guard pauses for $0.4\text{ s}$ facing the player with an exclamation mark.
   - *Transition*: Timer expires $\to$ `CHASING`.
3. `CHASING`: Guard sprints towards the player coordinate.
   - *Transition*: Within attack distance ($26\text{ px}$) $\to$ `ATTACKING`.
   - *Transition*: Loses sight $\to$ moves to last known coordinate, searches for $2.5\text{ s}$, then returns to `PATROLLING`.
4. `ATTACKING`: Deals $15\text{ HP}$ melee damage on a $0.85\text{ s}$ cooldown.
5. `DEAD`: Neutralized by player assassination. Permanently inactive.

#### Q27: How does Guard vision cone detection work mathematically?
**Answer**: A target point is in the guard's vision cone if it satisfies two conditions:
1. *Distance*: Euclidean distance $\|P - G\| \le \text{visionRadius}$ ($200\text{ px}$).
2. *Angle*: The angle between the guard's forward facing vector and the vector pointing towards the player must be $\le \frac{\text{visionAngle}}{2}$ ($\le 30^\circ$).
The angle is computed using `atan2`:
```cpp
float angleToTarget = std::atan2(dy, dx) * RAD2DEG;
float diff = std::abs(angleToTarget - m_facingAngle);
// Normalized to [0, 180]
```
If within the cone, a line-of-sight raycast is evaluated against obstacles.

#### Q28: How does the 2D Line-of-Sight (LoS) Raycasting algorithm determine if a wall blocks vision?
**Answer**: We test the line segment from the guard's eyes to the player's center against all four line segments of every obstacle's bounding rectangle. Two line segments $AB$ and $CD$ intersect if and only if points $C$ and $D$ lie on opposite sides of $AB$, and $A$ and $B$ lie on opposite sides of $CD$. This is tested using orientation determinants (2D cross products). If any solid obstacle segment intersects the vision ray, `checkLineOfSight` returns `false`.

#### Q29: What is the mathematical condition for an assassination? Why is a frontal attack rejected?
**Answer**: Assassination requires:
1. Distance: $\|P - G\| \le 65.0\text{ px}$.
2. Approach Angle: The angle between the guard's rear vector and the guard-to-player vector must be $\le 70.0^\circ$.
A frontal attack produces an approach angle near $180^\circ$ relative to the guard's back, which exceeds $70^\circ$. Thus `canAssassinate()` returns `false`, preventing the player from assassinating an enemy who is looking directly at them.

#### Q30: How is normalized 8-directional movement implemented, and why is it necessary?
**Answer**: If a player presses `W` and `D`, $v_x = 1$ and $v_y = -1$. The magnitude is $\sqrt{1^2 + (-1)^2} = \sqrt{2} \approx 1.414$. Without normalization, the player moves $41.4\%$ faster diagonally. By dividing the velocity components by the vector length:
$$v_x' = \frac{v_x}{\|v\|} \cdot \text{speed}, \quad v_y' = \frac{v_y}{\|v\|} \cdot \text{speed}$$
movement speed remains exactly $190\text{ px/s}$ in all 8 directions.

#### Q31: How does wall sliding work in `CollisionManager::resolveMovement`?
**Answer**: Rather than testing movement as a single 2D vector $(v_x \Delta t, v_y \Delta t)$, the solver resolves axes independently:
1. It applies $X$ displacement first: $x' = x + v_x \Delta t$. If this creates a collision with an obstacle, $x'$ is clamped to the obstacle edge and $X$ velocity is absorbed.
2. It then applies $Y$ displacement: $y' = y + v_y \Delta t$ and tests collisions independently.
If a player runs diagonally into a vertical wall, the $X$ movement is blocked, but the $Y$ movement completes unobstructed, producing a smooth sliding motion along the wall face.

#### Q32: What is the purpose of the `kSkin = 0.01f` constant in collision resolution?
**Answer**: When a circle's position is resolved against a rectangle, placing it *exactly* on the edge can cause floating-point rounding errors where subsequent frame collision checks register an overlap. Adding a minuscule skin offset ($0.01\text{ px}$) places the entity just outside the boundary, preventing collision oscillation ("jitter").

#### Q33: How does the game ensure entities do not clip through the top HUD bar?
**Answer**: The playable area is restricted to $Y \in [50.0\text{ px}, 608.0\text{ px}]$ to reserve the top $48\text{ px}$ for the mission HUD bar. In `Player::moveAndSlide()` and `Guard::updateAI()`, the position is clamped using `CollisionManager::clampToMapBounds()`, enforcing a strict minimum boundary of $Y \ge 50.0\text{ px}$.

#### Q34: How are floating UI notifications managed in memory?
**Answer**: `UIManager` maintains a `std::vector<UINotification>`. When an event occurs (e.g., `+1 INTEL`), a struct with text, color, position, and lifetime ($1.0\text{ s}$) is appended. In `UIManager::update(deltaTime)`, notifications float upwards, fade, and have their lifetimes decremented. Expired notifications are pruned using the standard C++ erase-remove idiom:
```cpp
m_notifications.erase(
    std::remove_if(m_notifications.begin(), m_notifications.end(),
        [](const UINotification& n) { return n.lifetime <= 0.0f; }),
    m_notifications.end()
);
```

#### Q35: What testing tools did you use to verify memory safety?
**Answer**: We used GCC's built-in **AddressSanitizer (ASan)** and **UndefinedBehaviorSanitizer (UBSan)** by compiling tests with `-fsanitize=address,undefined -g`. ASan monitors every heap allocation and deallocation to detect memory leaks, use-after-free, and out-of-bounds accesses. All 48 tests passed with zero leaks and zero errors.

---

### Category 4: Edge Cases & Real-World Code Debugging

#### Q36: What happened when the player died and retried a level during Phase 7 testing? How was it fixed?
**Answer**: During Phase 7 testing, calling `Player::heal(100)` on level retry failed to revive the player because `heal()` early-exited when `!isAlive()`. As a result, the dead player remained inactive (`m_active = false`). We resolved this by adding an explicit `Player::resetHealth()` method that resets `m_health = m_maxHealth;` and explicitly sets `m_active = true;`.

#### Q37: How do you prevent a player from repeatedly assassinating a dead guard?
**Answer**: In `Guard::isVulnerableTo()`, the first check is `if (m_state == GuardState::DEAD || !m_active) return false;`. Furthermore, in `Player::canAssassinate()`, proximity checks are only performed on living guards. Once assassinated, a guard transitions to `DEAD` immediately, deactivating vulnerability brackets.

#### Q38: Why is raylib integrated via CMake FetchContent with vendored fallback?
**Answer**: To guarantee reproducible, zero-configuration builds across both offline environments (e.g., evaluation machines with vendored source in `external/raylib`) and connected developer machines (using `FetchContent` to download official raylib 5.5 release tags).

#### Q39: What is the time complexity of the collision detection per frame?
**Answer**: In each frame, the player is tested against $M$ obstacles ($O(M)$). Each of the $N$ guards is tested against $M$ obstacles for movement ($O(N \cdot M)$) and casts 1 line-of-sight ray against $M$ obstacles with 4 edges each ($O(N \cdot 4M)$). Total complexity is $O(N \cdot M)$. With $N \le 6$ and $M \le 12$, the maximum calculations per frame are $\approx 6 \times 48 = 288$ vector operations, which executes in less than $0.03\text{ ms}$, effortlessly maintaining 60 FPS.

#### Q40: What was the most important software engineering lesson learned during this project?
**Answer**: The critical value of **loose coupling and single responsibility**. In early prototypes, putting collision checks inside `Player` or rendering code inside `Guard` made automated testing difficult. Separating math into `CollisionManager` and presentation into `UIManager` allowed us to write automated headless unit tests (`test_phase2` through `test_phase7`) that verify gameplay mechanics and memory safety in seconds without launching an interactive window.
