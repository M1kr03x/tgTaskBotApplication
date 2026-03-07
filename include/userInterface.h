#pragma once
#include "taskManager.h"
#include "userManager.h"

class UserInterface{
private:
userManager userMng;
void showMenu();
void runTaskManager();
void handleLogin();
void handleRegistration();
public:
void start();
User* getCurrentUser();
};