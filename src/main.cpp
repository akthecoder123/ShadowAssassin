#include "GameManager.h"
#include <iostream>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SHADOW ASSASSIN - 2D STEALTH ENGINE  " << std::endl;
    std::cout << "  C++17 OOP Academic Project            " << std::endl;
    std::cout << "========================================" << std::endl;

    GameManager game(1024, 768, "Shadow Assassin - Stealth Infiltration");
    game.run();

    std::cout << "[main] Game terminated normally." << std::endl;
    return 0;
}
