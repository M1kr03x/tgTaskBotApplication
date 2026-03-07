#pragma once
#include <string>
#include <vector>
#include "../include/task.h"
class User{
private:
std::string _login;
std::string _password;

public:
std::vector<Task> userTasks;
User(std::string,std::string);
User();
std::string getLogin();
void setLogin(std::string);
void setPassword(std::string);
std::string getPasword();
std::vector<Task>&getTasksVec() {return userTasks;}
};