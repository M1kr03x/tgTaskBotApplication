#include <iostream>
#include <string>
#include <limits>
#include "../include/userInterface.h"

UserInterface::UserInterface(Database& database) 
    : db(database), userMng(database) {}

void UserInterface::start() {
    while (true) {
        showMenu();
        int choice;
        std::cout << "Enter number to choose-> ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        

        switch (choice) {
            case 1: handleLogin(); break;
            case 2: handleRegistration(); break;
            case 3: return;
            default: std::cout << "\n";  

        }
    }
}

void UserInterface::showMenu() {
    std::cout << "\n========== MAIN MENU ==========\n";
    std::cout << "1. Log in\n";
    std::cout << "2. Registration\n";
    std::cout << "3. Exit\n";
}

void UserInterface::handleRegistration() {
    std::string login, password;
    std::cout << "\nEnter Login: ";
    std::getline(std::cin, login);
    std::cout << "Enter Password: ";
    std::getline(std::cin, password);
    
    if (userMng.registration(login, password)) {
        userMng.login(login, password);
        runTaskManager();
    }
}

void UserInterface::handleLogin() {
    std::string login, password;
    std::cout << "\nEnter Login: ";
    std::getline(std::cin, login);
    std::cout << "Enter Password: ";
    std::getline(std::cin, password);
    
    if (userMng.login(login, password)) {
        runTaskManager();
    }
}

void UserInterface::runTaskManager() {
    
    User* currentUser = userMng.getCurrentUser();
    if (!currentUser) return;
    
    taskManager taskMgr(db, currentUser);
    std::string cmd;
    
    std::cout << "\nWelcome, " << currentUser->getLogin() << "\n";

    std::cout << "Available commands:\n";
    std::cout << "  add <text>     - add task\n";
    std::cout << "  list           - show tasks\n";
    std::cout << "  complete <id>  - mark task as completed\n";
    std::cout << "  delete <id>    - delete task\n";
    std::cout << "  logout         - exit\n";
    std::cout << "  help           - show this menu\n";
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, cmd);
        
        if (cmd.empty()) continue;
        
        if (cmd == "logout") {
            userMng.logout();
            break;
        }
        else if (cmd.rfind("add ", 0) == 0 && cmd.size() > 4) {
            taskMgr.addTask(cmd.substr(4));
        }
        else if (cmd == "list") {
            taskMgr.getAllTasks();
        }
        else if (cmd.rfind("complete ", 0) == 0 && cmd.length() > 9) {
            try {
                int id = std::stoi(cmd.substr(9));
                taskMgr.completeTask(id);
            } catch (...) {}
        }
        else if (cmd.rfind("delete ", 0) == 0 && cmd.length() > 7) {
            try {
                int id = std::stoi(cmd.substr(7));
                taskMgr.deleteTask(id);
            } catch (...) {}
        }
        else if (cmd == "help") {
    std::cout << "\n=== Available commands ===\n";
    std::cout << "add <text>     - add a new task\n";
    std::cout << "list           - show all tasks\n";
    std::cout << "complete <id>  - mark task as completed\n";
    std::cout << "delete <id>    - delete task\n";
    std::cout << "logout         - exit to main menu\n";
    std::cout << "help           - show this help\n";
}
    }
}