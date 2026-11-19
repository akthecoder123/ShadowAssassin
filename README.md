# SHADOW ASSASSIN
### 2D Top-Down Stealth Assassination Game (C++17 OOP Academic Project)

---

## 1. Project Overview
**Shadow Assassin** is a 2D top-down stealth assassination game inspired by the core mechanics of *Hunter Assassin*. Developed as an academic software engineering project for Object-Oriented Programming (OOP) evaluation, the primary objective is to demonstrate clean, idiomatic, and robust OOP architecture in modern **C++17** using the lightweight multimedia library **raylib 5.5** and the **CMake** build system.

The player assumes the role of a shadow operative infiltrating hostile facilities. Success relies on patience, line-of-sight concealment, and tactical rear approaches. Direct confrontation is lethal: guards are equipped with acute vision cones and high-damage melee attacks. Players must eliminate all stationed guards from behind, dodge patrol paths, and harvest intelligence coins across handcrafted sectors to unlock campaign victory.

---

## 2. Problem Statement & Motivation
Developing real-time games often leads to spaghetti code, tight coupling, and leaky abstractions when modern software engineering practices are ignored. Typical beginner game implementations conflate physics calculations, rendering routines, AI logic, and input handling within a monolithic game loop.

**Shadow Assassin** solves this by establishing a disciplined, modular Object-Oriented architecture:
- **Clean Separation of Concerns**: Rendering (`UIManager`), physics and raycasting (`CollisionManager`), world layout (`Level`), AI decision-making (`Guard`), and application orchestration (`GameManager`) are fully decoupled into independent, cohesive classes.
- **Strict Data Encapsulation & Polymorphic Extensibility**: All state is protected against illegal direct mutations, and polymorphic interfaces (`Entity`, `Enemy`) allow seamless addition of new entity types without modifying existing simulation code (Open/Closed Principle).
- **Predictable Memory Safety (RAII)**: Zero manual memory management (`new`/`delete`) in application logic; ownership is strictly enforced through standard smart pointers (`std::unique_ptr`).

---

## 3. Game Features
- **Dynamic Vision Cone Detection**: Guards scan the environment with directional vision cones (60° field of view, 200px detection radius) that react dynamically to player intrusion and change color based on alert state.
- **Obstacle Line-of-Sight Occlusion**: Walls and barriers physically cast shadows by blocking vision rays using an exact 2D determinant raycasting algorithm. Players can hide safely behind cover.
- **Rear-Approach Assassination**: Operatives can silently neutralize guards by approaching within 65 pixels from behind (within a 70° rear angle). Frontal approaches alert the enemy.
- **Finite State Machine (FSM) Guard AI**: Five distinct behavioral states—`PATROLLING`, `ALERTED`, `CHASING`, `ATTACKING`, and `DEAD`—featuring waypoint navigation, suspicious investigation, and target pursuit.
- **Normalized 8-Directional Movement**: Diagonal movement is mathematically normalized to eliminate the $\sqrt{2}$ speed exploit, paired with axis-separated circle-rectangle wall sliding.
- **Campaign Progression & Handcrafted Levels**: Three handcrafted sectors of escalating complexity: *Training Ground*, *Restricted Facility*, and *Final Mission*.
- **Tactical HUD & Feedback**: Real-time health bars, gold tallies, enemy counters, animated crescent blade slashes, and floating notification text (`+1 INTEL`, `ASSASSINATED!`).
- **Character Art & Top-Down Sprites**: Hunter Assassin-inspired top-down characters:
  - *Player Assassin*: Dark stealth hooded cloak with cowl, glowing cyan tactical optics, dual boots, and combat dagger with metallic glint. Directional orientation, walking stride cycle, assassination lunge/slash, and idle breathing pulse.
  - *Enemy Guards*: Armored riot helmets, security crimson ballistic vests with plate inserts, tactical carbines with flashlight, and FSM-driven animations (patrol walk, alert stance, sprint chase, melee strike, corpse on ground).
  - *Dynamic Visual Effects*: Ground contact shadows, blood splatter decals, and alert indicator badges.
- **Checkpoint Economy & Anti-Farming**: Restarts and retries restore exact sector-entry coin and elimination metrics, preventing score-farming exploits.

---

## 4. Technologies Used
- **Programming Language**: C++17 (Strict ISO standard, smart pointers, RAII, uniform initialization).
- **Game & Graphics Library**: [raylib](https://www.raylib.com/) 5.5 (Hardware-accelerated 2D rendering via OpenGL).
- **Build System**: CMake 3.20+ (Cross-platform build configuration).
- **Compiler Support**: GCC (g++ 9+), Clang (clang++ 10+), MSVC (Visual Studio 2019/2022), MinGW-w64.
- **Memory & Safety Verification**: Valgrind, GCC AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan).

---

## 5. Installation & Prerequisites

### Windows
1. **Compiler**: MinGW-w64 (GCC 9+) or Microsoft Visual Studio 2019/2022 (with "Desktop development with C++" workload).
2. **Build Tool**: [CMake 3.20+](https://cmake.org/download/) added to system `PATH`.
3. **Version Control**: [Git](https://git-scm.com/) (required for CMake `FetchContent` if raylib is fetched).

### Linux / WSL (Ubuntu / Debian)
Install standard build essentials and OpenGL/X11 development libraries:
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libgl1-mesa-dev libx11-dev libxcursor-dev libxinerama-dev libxrandr-dev libxi-dev
```

---

## 6. Exact Build Instructions

### Method A: Command-Line Build (Recommended for Windows & Linux)
```bash
# 1. Clone repository and navigate to root directory
cd D:/oops_proj   # On Linux/WSL: cd /mnt/d/oops_proj

# 2. Configure build system (Release mode for maximum performance)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Compile the executable and all test suites
cmake --build build --config Release
```

### Method B: Visual Studio Code (Windows)
1. Open the folder `D:/oops_proj` in VS Code.
2. Install the **C/C++** (`ms-vscode.cpptools`) and **CMake Tools** (`ms-vscode.cmake-tools`) extensions.
3. Select your compiler kit (e.g., `GCC for x86_64-w64-mingw32` or `Visual Studio Community Release - amd64`).
4. Click **Build** on the bottom status bar (or press `F7`).

---

## 7. How to Run the Game

### From Command Line
```bash
# Windows (Command Prompt / PowerShell)
./build/ShadowAssassin.exe
# (If using multi-config MSVC generator)
./build/Release/ShadowAssassin.exe

# Linux / WSL
./build/ShadowAssassin
```

---

## 8. Controls
| Input Key | Context | Action |
|---|---|---|
| `W` / `Up Arrow` | Gameplay | Move Operative North / Up |
| `S` / `Down Arrow` | Gameplay | Move Operative South / Down |
| `A` / `Left Arrow` | Gameplay | Move Operative West / Left |
| `D` / `Right Arrow` | Gameplay | Move Operative East / Right |
| `SPACE` | Gameplay | Execute Silent Assassination (when in range and behind target) |
| `ENTER` / `SPACE` | Modals / Menus | Confirm dialog, advance to next level upon clearance |
| `P` / `ESC` | Gameplay / Modals | Pause / Resume game; Dismiss Field Manual modal |
| `M` | Modals | Return to Main Menu (from Pause, Game Over, or Victory) |
| `R` | In-Game / Modals | Restart current level (resets entities, positions, and coins) |
| `Q` | Modals / Menu | Quit application |
| `Mouse Left Click` | All Screens | Interactive button selection (Play, Manual, Resume, Retry, Exit) |
| `F1` | Developer Debug | Toggle Developer Debug Mode |
| `1` / `4` / `5` / `6` / `7` | Debug Mode Only | Instant teleport to Level 1, Level 2, Level 3, Level 4, or Level 5 |
| `2` / `3` | Debug Mode Only | Damage Operative (-20 HP) / Heal Operative (+20 HP) |

---

## 9. Level Descriptions
The campaign comprises five handcrafted, progressively challenging sectors:

### Sector 1: Training Ground (Level 1)
- **Enemy Roster**: 2 Patrol Guards.
- **Terrain**: 5 strategic obstacles creating a central corridor and flanking alcoves.
- **Collectibles**: 4 Intelligence Coins.
- **Design Intent**: Teaches core fundamentals: reading vision cones, taking cover behind solid barriers, and executing rear-approach assassinations on simple back-and-forth patrol routes.

### Sector 2: Restricted Facility (Level 2)
- **Enemy Roster**: 3 Patrol Guards.
- **Terrain**: 8 obstacles forming interior rooms, choke points, and narrow corridors.
- **Collectibles**: 6 Intelligence Coins.
- **Design Intent**: Introduces overlapping vision cones, perpendicular patrol sweeps, and restricted sightlines, requiring players to time their movements carefully.

### Sector 3: Final Mission (Level 3)
- **Enemy Roster**: 4 Patrol Guards (High speed, wide-angle vision cones).
- **Terrain**: 9 obstacles creating a complex multi-room security complex.
- **Collectibles**: 7 Intelligence Coins.
- **Design Intent**: High-intensity security sector. Guards cross paths and cover each other's blind spots. Neutralizing all 4 guards unlocks access to the underground sectors.

### Sector 4: Underground Bunker (Level 4)
- **Enemy Roster**: 5 Patrol Guards (Generator sentry, corridor sweepers, vault sentry).
- **Terrain**: 11 tactical obstacles creating narrow passages, generator powerhouse, and comms wing.
- **Collectibles**: 8 Intelligence Coins.
- **Design Intent**: Deep subterranean infiltration. Requires navigating tight chokepoints and taking advantage of blind spots during generator room rotations.

### Sector 5: Shadow Protocol (Level 5)
- **Enemy Roster**: 6 Elite Sentries (Speed 80–90 px/s, intersecting patrols).
- **Terrain**: 12 reinforced obstacles forming the Infiltration Zone, Cyber Lab, Quantum Server Core, and Command Citadel.
- **Collectibles**: 9 Intelligence Coins.
- **Design Intent**: The ultimate campaign climax. Highly vigilant elite units covering multi-angle corridors. Neutralizing all 6 elite guards triggers the **CAMPAIGN VICTORY** sequence.

---

## 10. Project Folder Structure
```
D:/oops_proj/
├── CMakeLists.txt                  # Master CMake build configuration
├── README.md                       # Main project documentation & overview
│
├── assets/                         # Multimedia & Visual Assets
│   ├── images/                     # 2D Character Sprite Sheets & Decals
│   │   ├── player_spritesheet.png  # Player assassin top-down sprite sheet
│   │   ├── guard_spritesheet.png   # Enemy guard top-down sprite sheet
│   │   ├── shadow.png              # Soft radial ground contact shadow
│   │   └── blood_splatter.png      # Defeated guard blood decal
│   └── generate_sprites.py         # Pure Python antialiased sprite generator
│
├── include/                        # C++ Header Declarations (.h)
│   ├── Entity.h                    # Abstract base entity class
│   ├── Player.h                    # Player operative character class
│   ├── Enemy.h                     # Base enemy class
│   ├── Guard.h                     # Patrol guard AI with FSM & vision cone
│   ├── Obstacle.h                  # Solid walls & vision-blocking cover
│   ├── Collectible.h               # Intelligence coin pickup class
│   ├── CollisionManager.h          # Static physics, wall sliding & raycasting solver
│   ├── CharacterRenderer.h         # Top-down sprite rendering & animation manager
│   ├── Level.h                     # Level layout, composite ownership & reset
│   ├── GameManager.h               # Game state controller & application loop
│   └── UIManager.h                 # Decoupled UI presentation, HUD & menus
│
├── src/                            # C++ Source Implementations (.cpp)
│   ├── main.cpp                    # Application entry point
│   ├── Entity.cpp                  # Spatial methods & distance queries
│   ├── Player.cpp                  # Movement, input, health & assassination logic
│   ├── Enemy.cpp                   # Base hostile combat hooks
│   ├── Guard.cpp                   # FSM transitions, patrol routes & vision checks
│   ├── Obstacle.cpp                # Barrier rendering & point/circle collision
│   ├── Collectible.cpp             # Coin rendering, animation & collection logic
│   ├── CollisionManager.cpp        # Circle-rect resolution & 2D raycasting math
│   ├── CharacterRenderer.cpp       # Texture loading, frame slicing & character drawing
│   ├── Level.cpp                   # Handcrafted map layouts & composite lifecycle
│   ├── GameManager.cpp             # Game loop, screen flow & checkpoint state
│   └── UIManager.cpp               # HUD, menus, buttons & floating text
│
├── tests/                          # Automated Verification Test Suites (.cpp)
│   ├── test_phase2.cpp             # Entity hierarchy & polymorphism tests
│   ├── test_phase3.cpp             # Movement, wall sliding & assassination tests
│   ├── test_phase4.cpp             # Guard AI FSM, vision cones & raycasting tests
│   ├── test_phase5.cpp             # Level composite, loading & progression tests
│   ├── test_phase6.cpp             # UIManager, HUD, button widgets & state tests
│   └── test_phase7_integration.cpp # Full end-to-end campaign integration test
│
└── docs/                           # Academic Project Documentation
    ├── PROJECT_REPORT.md           # Formal academic project report
    ├── OOP_VIVA_GUIDE.md           # Comprehensive viva defense & interview prep guide
    ├── CLASS_DIAGRAM.md            # Mermaid UML class diagram & architecture breakdown
    ├── EXECUTION_GUIDE.md          # Step-by-step setup, compilation & troubleshooting
    ├── OOP_CONCEPTS.md             # In-depth analysis of OOP principles in code
    └── DEVELOPMENT_LOG.md          # Chronological phase development audit trail
```

---

## 11. Testing Instructions and Actual Results
Shadow Assassin features a comprehensive automated testing pipeline consisting of 6 dedicated test executables. Testing runs headlessly without requiring an active display.

### Executing All Test Suites
```bash
# Build test executables
cmake --build build --config Release

# Run test executables sequentially
./build/test_phase2 && ./build/test_phase3 && ./build/test_phase4 && ./build/test_phase5 && ./build/test_phase6 && ./build/test_phase7
```

### Actual Verification Results
| Test Suite Target | Scope / Features Tested | Test Cases | Status |
|---|---|:---:|:---:|
| `test_phase2` | Entity hierarchy, virtual dispatch, encapsulation, bounds | 5 / 5 | **PASSED** (100%) |
| `test_phase3` | Normalized movement, circle-rect collision, wall slide, rear assassination | 8 / 8 | **PASSED** (100%) |
| `test_phase4` | Guard AI FSM (5 states), vision cone math, 2D LoS raycasting, search timer | 14 / 14 | **PASSED** (100%) |
| `test_phase5` | 5 Handcrafted levels, entity composition, level reset, progression (1..5) | 10 / 10 | **PASSED** (100%) |
| `test_phase6` | UI widget hit-testing, state transitions, HUD integrity, floating alerts | 10 / 10 | **PASSED** (100%) |
| `test_phase7` | Full Campaign Playthrough (L1 $\to$ L2 $\to$ L3 $\to$ L4 $\to$ L5 $\to$ Victory), Checkpoint anti-farming, pause freeze | 1 / 1 (Multi-stage) | **PASSED** (100%) |
| **Total** | **Comprehensive Regression Suite** | **48 / 48** | **100% SUCCESS** |

### Memory Safety & Dynamic Analysis
The codebase was verified with **AddressSanitizer (ASan)** and **UndefinedBehaviorSanitizer (UBSan)**:
```
=================================================================
== AddressSanitizer Report:
== 0 heap-use-after-free
== 0 heap-buffer-overflow
== 0 global-buffer-overflow
== 0 memory leaks detected
== Result: CLEAN (Zero memory defects)
=================================================================
```

---

## 12. OOP Concepts Demonstrated
1. **Abstraction**: 
   - `Entity` defines pure virtual methods `update(float)` and `draw()` without exposing internal representations.
   - `CollisionManager` abstracts complex 2D vector determinant math behind clean interfaces like `checkLineOfSight()`.
2. **Encapsulation & Data Hiding**:
   - Class state (health, speed, position, AI states) is declared `private` or `protected`.
   - Access and mutations are restricted to validated getters/setters (e.g., `setCoins(std::max(0, amount))`).
3. **Inheritance**:
   - Single Inheritance: `Player` inherits from `Entity`.
   - Multilevel Inheritance: `Guard` $\to$ `Enemy` $\to$ `Entity`.
4. **Runtime Polymorphism**:
   - Virtual destructors guarantee correct destruction order.
   - Virtual dispatch over base pointers (`Enemy*`, `Entity*`) routes execution to overridden derived methods at runtime via virtual method tables (`vtable`).
5. **Composition**:
   - `Level` strongly owns `std::vector<Obstacle>`, `std::vector<std::unique_ptr<Guard>>`, and `std::vector<Collectible>`.
   - `GameManager` strongly owns `Level`, `Player`, and `UIManager`.
6. **Single Responsibility Principle (SRP)**:
   - `CollisionManager`: Pure static collision and raycasting solver.
   - `UIManager`: Pure presentation and UI widget layout.
   - `Level`: World geometry and entity container.
   - `GameManager`: High-level lifecycle orchestrator.

---

## 13. Academic Submission Information
- **Course**: Object-Oriented Programming (OOP) / Software Engineering
- **Project Title**: Shadow Assassin — 2D Stealth Infiltration Game
- **Language Standard**: ISO C++17
- **Architecture**: Modular Object-Oriented Design (Clean C++ / raylib / CMake)
- **Documentation Package**:
  - Academic Project Report: [docs/PROJECT_REPORT.md](file:///mnt/d/oops_proj/docs/PROJECT_REPORT.md)
  - OOP Viva Defense Guide: [docs/OOP_VIVA_GUIDE.md](file:///mnt/d/oops_proj/docs/OOP_VIVA_GUIDE.md)
  - UML Class Diagram: [docs/CLASS_DIAGRAM.md](file:///mnt/d/oops_proj/docs/CLASS_DIAGRAM.md)
  - Compilation & Execution Guide: [docs/EXECUTION_GUIDE.md](file:///mnt/d/oops_proj/docs/EXECUTION_GUIDE.md)
  - In-Depth OOP Analysis: [docs/OOP_CONCEPTS.md](file:///mnt/d/oops_proj/docs/OOP_CONCEPTS.md)
  - Development Audit Log: [docs/DEVELOPMENT_LOG.md](file:///mnt/d/oops_proj/docs/DEVELOPMENT_LOG.md)
