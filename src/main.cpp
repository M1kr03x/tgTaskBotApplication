#include "userInterface.h"
#include <iostream>
#include <locale>

int main() {
    // Для поддержки русского языка в консоли Windows
    #ifdef _WIN32
        setlocale(LC_ALL, "Russian");
    #endif
    
    std::cout << "╔════════════════════════════╗\n";
    std::cout << "║   TASK MANAGER BOT v1.0    ║\n";
    std::cout << "╚════════════════════════════╝\n\n";
    
    UserInterface ui;
    ui.start();
    
    std::cout << "\nGoodbye! Have a nice day!\n";
    
    return 0;
}