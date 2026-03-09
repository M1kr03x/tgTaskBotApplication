#pragma once
#include <string>
#include <vector>
#include "../include/task.h"
class User{
private:
std::string _login;
std::string _password;
 int _id; 
public:
std::vector<Task> userTasks;
User(std::string,std::string);
User();
std::string getLogin() const;
void setLogin(std::string);
void setPassword(std::string);
std::string getPassword() const;
std::vector<Task>&getTasksVec(){return userTasks;}
void setId(int id) { _id = id; }
int getId() const { return _id; }
};