#pragma once
#include "user.h"
#include "database.h"
class taskManager{
    private:
    Database& db;
    User* _userPtr;
    public:
    taskManager(Database& database, User* user);
    void addTask(const std::string&);
    bool deleteTask(int);
    bool completeTask(int);
    void getAllTasks() const;
   
};