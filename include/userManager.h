#pragma once
#include <vector>
#include "user.h"
#include "database.h"
class userManager{
private:
    Database& db;
    std::vector<User> users;
    User* currentUser = nullptr;
    public:
    userManager(Database& database); 
    // bool registration(std::string,std::string);
    // bool login(std::string,std::string);
    void logout();
    User* getCurrentUser();
};