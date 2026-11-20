# Compilation, Execution & Verification Guide
## Project: Shadow Assassin (C++17 / raylib 5.5 / CMake)

---

## 1. System Requirements & Prerequisites

### Windows
To compile and run Shadow Assassin on a standard Windows machine, you need:
1. **C++17 Compiler**:
   - **MinGW-w64** (GCC 9+), available via [MSYS2](https://www.msys2.org/) or [w64devkit](https://github.com/skeeto/w64devkit), OR
   - **Microsoft Visual C++ (MSVC)**: Included with Visual Studio 2019 / 2022 (with the *"Desktop development with C++"* workload selected).
2. **CMake**: Version 3.20 or newer, accessible via system `PATH` ([Download CMake](https://cmake.org/download/)).
3. **Git**: Required for CMake to automatically download raylib via `FetchContent` if the local vendored copy is absent.

### Linux / WSL2 (Ubuntu / Debian)
Install standard development tools and the required X11 and OpenGL libraries:
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git \
    libgl1-mesa-dev libx11-dev libxcursor-dev \
    libxinerama-dev libxrandr-dev libxi-dev
```

---

## 2. Compilation Instructions

### A. Command Line Build (Cross-Platform)

```bash
# 1. Navigate to the project root directory
cd D:/oops_proj      # On Linux/WSL: cd /mnt/d/oops_proj

# 2. Generate the build configuration (Release mode recommended for 60 FPS)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Compile the game executable and all test suites
cmake --build build --config Release
```

### B. Building with Visual Studio Code (Windows)
1. Launch VS Code and open the folder `D:/oops_proj`.
2. Ensure the following extensions are installed:
   - **C/C++** (`ms-vscode.cpptools`)
   - **CMake Tools** (`ms-vscode.cmake-tools`)
3. Open the Command Palette (`Ctrl + Shift + P`) and run:
   `CMake: Select a Kit` $\to$ Choose your installed compiler (e.g., `GCC for x86_64-w64-mingw32` or `Visual Studio Community 2022 Release - amd64`).
4. Set build type to `Release` on the VS Code status bar.
5. Click **Build** (or press `F7`).

---

## 3. How to Execute the Game

### Running the Executable
After compiling, run the generated binary:

```bash
# On Windows (Command Prompt / PowerShell / Git Bash)
./build/ShadowAssassin.exe
# If using Visual Studio multi-configuration generator:
./build/Release/ShadowAssassin.exe

# On Linux / WSL
./build/ShadowAssassin
```

*(Note for WSL users: Ensure you have WSLg running on Windows 11, or an X-Server such as VcXsrv active on Windows 10 with `export DISPLAY=:0` set).*

---

## 4. Running the Automated Test Suites
Shadow Assassin includes 6 dedicated test binaries verifying every phase of development:

```bash
# Run all 6 test suites sequentially from the project root
./build/test_phase2 && \
./build/test_phase3 && \
./build/test_phase4 && \
./build/test_phase5 && \
./build/test_phase6 && \
./build/test_phase7
```

### Expected Test Output
```
[TEST] Running Phase 2 Unit Tests...
  - Player health & economy: PASSED
  - Guard FSM & Vision Cone: PASSED
  - Runtime Polymorphism (Enemy* -> Guard): PASSED
  - Obstacle Collision interfaces: PASSED
  - Collectible Lifecycle: PASSED
ALL PHASE 2 UNIT TESTS PASSED SUCCESSFULLY!

[TEST] Running Phase 3 Unit Tests: Player Mechanics & Assassination...
  [1/8] Diagonal Movement Normalization: PASSED (Speed = 200 px/s)
  [2/8] Circle vs Rect Collision Resolution: PASSED
  [3/8] Wall Sliding Physics: PASSED
  [4/8] Map Boundary Clamping: PASSED
  [5/8] Assassination Distance Boundary: PASSED
  [6/8] Rear-Approach vs Front-Facing Check: PASSED
  [7/8] Assassination Execution & Rewards: PASSED
  [8/8] Prevention of Repeated Assassination: PASSED
>>> ALL 8 PHASE 3 UNIT TESTS PASSED SUCCESSFULLY! <<<

[TEST] Running Phase 4 Unit Tests: Guard AI System & Vision Raycasting...
  [1/14] Patrol Waypoint Progression: PASSED
  [2/14] Guard Facing Direction Orientation: PASSED
  [3/14] Detection Within Vision Radius & Angle: PASSED
  [4/14] Detection Outside Vision Radius Rejection: PASSED
  [5/14] Detection Outside Vision Angle Rejection: PASSED
  [6/14] Line of Sight Raycast & Obstacle Occlusion: PASSED
  [7/14] State Transition PATROLLING -> ALERTED: PASSED
  [8/14] State Transition ALERTED -> CHASING on Timer: PASSED
  [9/14] Chasing Mechanics & Pursuit Movement: PASSED
  [10/14] Search Mode at Last Known Position: PASSED
  [11/14] Return to PATROLLING After Search Timeout: PASSED
  [12/14] Attack Cooldown Throttling: PASSED
  [13/14] Combat Damage Application: PASSED
  [14/14] Dead Guard Inactivity & Neutralization: PASSED
>>> ALL 14 PHASE 4 UNIT TESTS PASSED SUCCESSFULLY! <<<

[TEST] Running Phase 5 Unit Tests: Level Implementation & Progression...
  [1/10] Exactly Three Discrete Levels Defined: PASSED
  [2/10] Expected Guard Count per Level (2, 3, 4): PASSED
  [3/10] Valid Player Spawn Positions: PASSED
  [4/10] Expected Obstacles and Collectibles Configuration: PASSED
  [5/10] Level State Reset & Entity Restoration: PASSED
  [6/10] Level Progression Sequencing (1 -> 2 -> 3): PASSED
  [7/10] Level 3 Completion Triggers VICTORY State: PASSED
  [8/10] Player Movement & Obstacle Collision Resolution: PASSED
  [9/10] Guard Vision & Line-of-Sight Raycasting Across Map: PASSED
  [10/10] Assassination and Coin Collection in Handcrafted Level: PASSED
>>> ALL 10 PHASE 5 UNIT TESTS PASSED SUCCESSFULLY! <<<

[TEST] Running Phase 6 Unit Tests: Game Interface & Visual Polish...
  [1/10] UIManager Initialization & Dimension Configuration: PASSED
  [2/10] Reusable Button Hit-Testing & Hover Trigger Logic: PASSED
  [3/10] Floating Notification Lifecycle & Auto-Cleanup: PASSED
  [4/10] Complete Game State Flow (MENU <-> PLAYING <-> PAUSED): PASSED
  [5/10] Main Menu Controls Modal Toggle State: PASSED
  [6/10] Level Complete Transition & Progression to Sector 2: PASSED
  [7/10] Game Over State Trigger & Sector Retry Restoration: PASSED
  [8/10] Campaign Victory Trigger & Replay Flow: PASSED
  [9/10] UI Input Non-Interference & Simulation Pause: PASSED
  [10/10] In-Game HUD Telemetry & Data Integrity: PASSED
>>> ALL 10 PHASE 6 UNIT TESTS PASSED SUCCESSFULLY! <<<

[TEST] Running Phase 7 Comprehensive Campaign Integration Test Suite...
  --> Stage 1: Initial Launch & Main Menu State
  --> Stage 2: Infiltration of Sector 1 (Training Ground)
  --> Stage 3: Infiltration of Sector 2 (Restricted Facility)
  --> Stage 4: Infiltration of Sector 3 (Final Mission)
  --> Stage 5: Campaign Victory Clearance Verification
  [PASS] Full Campaign End-to-End Playthrough & Victory Sequence.
  --> Testing Checkpoint Restoration & Anti-Farming Logic...
  [PASS] Checkpoint Restoration & Anti-Farming Safeguard.
  --> Testing Pause Freeze & Simulation Isolation...
  [PASS] Pause Freeze & Simulation Isolation.
  --> Testing Combat Invariants & Elimination Invariance...
  [PASS] Combat Invariants, Vision Occlusion & Boundary Safety.
>>> ALL PHASE 7 INTEGRATION TESTS PASSED WITH ZERO REGRESSIONS! <<<
```

---

## 5. Verification with AddressSanitizer (ASan) & UBSan
To run the full campaign test with dynamic memory safety checks:

```bash
# Compile and run test_phase7 with AddressSanitizer and UndefinedBehaviorSanitizer
g++ -fsanitize=address,undefined -g -std=c++17 \
    -Iinclude -Iexternal/raylib/src \
    tests/test_phase7_integration.cpp \
    src/Entity.cpp src/Player.cpp src/Enemy.cpp src/Guard.cpp \
    src/Obstacle.cpp src/Collectible.cpp src/CollisionManager.cpp \
    src/Level.cpp src/UIManager.cpp src/GameManager.cpp \
    -Lbuild/external/raylib/raylib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -o build/test_asan && ./build/test_asan
```
*Expected Result*: Zero memory leaks, zero use-after-free, zero heap buffer overflows.

---

## 6. Controls & Debug Modes

### Primary Player Controls
- `W` / `A` / `S` / `D` or **Arrow Keys**: Move operative.
- `SPACE`: Silent assassination (when within 65px and behind a living guard) / Modal confirmation.
- `ENTER`: Proceed to next level upon clearance.
- `P` / `ESC`: Pause / Resume game; dismiss modals.
- `M`: Return to Main Menu from Pause, Game Over, or Victory screens.
- `R`: Restart active sector (restores guards and checkpoint metrics).
- `Q`: Abort / Exit game.
- `F11` / `ALT+ENTER`: Toggle between Fullscreen and Windowed mode.
- `Mouse Left Click`: Select UI buttons on menus.

### Developer Debug Mode
Press `[F1]` to toggle Developer Debug Mode on/off. When enabled:
- `1`: Instantly load Level 1 (Training Ground).
- `4`: Instantly load Level 2 (Restricted Facility).
- `5`: Instantly load Level 3 (Final Mission).
- `2`: Apply -20 damage to player operative.
- `3`: Heal +20 health to player operative.

---

## 7. Troubleshooting Guide

### 1. `Could NOT find X11` or `cannot find -lGL` (Linux/WSL)
- **Cause**: Missing Linux graphics development headers.
- **Solution**: Run:
  ```bash
  sudo apt-get install -y libgl1-mesa-dev libx11-dev libxcursor-dev libxinerama-dev libxrandr-dev libxi-dev
  ```

### 2. `CMake 3.20 or higher is required`
- **Cause**: Outdated CMake installed on host machine.
- **Solution**: Install modern CMake via `pip install cmake` or download the binary package from cmake.org.

### 3. Git FetchContent Network Failure
- **Cause**: Offline build environment blocking git clone of raylib repository.
- **Solution**: Shadow Assassin includes vendored raylib 5.5 source in `external/raylib`. If the internet is unavailable, CMake automatically detects and uses the local vendored tree.

### 4. Headless Unit Test Segmentation Fault
- **Cause**: Drawing functions called without an active OpenGL window context.
- **Solution**: All drawing routines in `UIManager` are guarded with `if (IsWindowReady())`. If writing new test suites, avoid calling raw raylib drawing routines before `InitWindow()`.
