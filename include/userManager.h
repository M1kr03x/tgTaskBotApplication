#pragma once
#include <vector>
#include "user.h"
class userManager{
private:
    std::vector<User> users;
    User* currentUser = nullptr;
    public:
    userManager();
    bool registration(std::string,std::string);
    bool login(std::string,std::string);
    void logout();
    User* getCurrentUser();
};