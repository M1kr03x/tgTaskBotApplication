#pragma once
#include "user.h"
class taskManager{
    private:
    User* _userPtr;
    public:
    void addTask(const std::string&);
    bool deleteTask(int);
    bool completeTask(int);
    void getAllTasks() const;
    taskManager(User*);

};