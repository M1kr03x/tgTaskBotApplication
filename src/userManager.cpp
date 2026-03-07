#include <iostream>
#include "userManager.h"

userManager::userManager(){
    User admin;
    admin.setLogin("admin");
    admin.setPassword("12345");
    users.push_back(admin);
}
bool userManager::registration(std::string log, std::string password){
    for(int i = 0; i < users.size(); i++) {
        if(users[i].getLogin() == log) {
            return false;  // такой логин уже есть - регистрация невозможна
        }
    }
    
    User newUser;
    newUser.setLogin(log);
    newUser.setPassword(password);
    users.push_back(newUser);
    return true;
    
}

bool userManager::login(std::string log, std::string pas){
    for(int i=0; i <users.size();i++){
        if(users[i].getLogin() == log and users[i].getPasword() == pas){
            currentUser = &users[i]; 
            return true;
        }
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