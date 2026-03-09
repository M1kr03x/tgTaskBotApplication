#pragma once
#include "taskManager.h"
#include "userManager.h"
#include "database.h"
class UserInterface{
private:
 Database& db;
userManager userMng;
void showMenu();
void runTaskManager();
void handleLogin();
void handleRegistration();
public:
UserInterface(Database& database);
void start();

};