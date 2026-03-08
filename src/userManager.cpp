#include <iostream>
#include "userManager.h"

userManager::userManager(Database& database) : db(database){}

bool userManager::registration(std::string log, std::string password){
  return db.addUser(log,password);
    
}

bool userManager::login(std::string log, std::string pas){
     std::cout << "DEBUG: login called for " << log << "\n";
    auto userId = db.findUserID(log, pas);
     std::cout << "DEBUG: userId = " << (userId.has_value() ? std::to_string(userId.value()) : "nullopt") << "\n";
     if (userId.has_value()) {
        User user;
        user.setLogin(log);
        user.setPassword(pas);
        user.setId(userId.value());
        users.push_back(user);
        currentUser = &users.back();
        return true;
    }
    return false;
}
void userManager::logout() {
    currentUser = nullptr;
    std::cout << "You have been logouted" << std::endl;
    
}
User* userManager::getCurrentUser(){
    return this->currentUser;
};