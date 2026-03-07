#include "userInterface.h"
#include <iostream>
#include <locale>

int main() {
    // Для поддержки русского языка в консоли Windows
    
    UserInterface ui;
    ui.start();
    
    std::cout << "\nGoodbye! Have a nice day!\n";
    
    return 0;
}