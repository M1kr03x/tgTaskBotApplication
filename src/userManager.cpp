#include <iostream>
#include "userManager.h"

userManager::userManager(Database& database) : db(database){}

void userManager::logout() {
    currentUser = nullptr;
    std::cout << "You have been logouted" << std::endl;
    
}
User* userManager::getCurrentUser(){
    return this->currentUser;
};