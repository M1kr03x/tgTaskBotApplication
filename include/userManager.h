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
    void logout();
    User* getCurrentUser();
};