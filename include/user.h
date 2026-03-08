#pragma once
#include <string>
#include <vector>
#include "../include/task.h"
class User{
private:
std::string _login;
std::string _password;
pqxx::work createTransaction();
public:
std::vector<Task> userTasks;
User(std::string,std::string);
User();
std::string getLogin() const;
void setLogin(std::string);
void setPassword(std::string);
std::string getPasword() const;
std::vector<Task>&getTasksVec(){return userTasks;}
};