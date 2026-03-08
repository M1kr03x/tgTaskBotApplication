#include "userInterface.h"
#include <iostream>
#include <locale>

int main() {
   try {
        Database db("tgBotDb",     
                   "postgres",          
                   "324520525",             
                   "localhost",        
                   "5432");            

        if (!db.isConnected()) {
            std::cerr << "Failed to connect to database.\n";
            return 1;
        }
      
        UserInterface ui(db);
        ui.start();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nGoodbye\n";
    return 0;
}