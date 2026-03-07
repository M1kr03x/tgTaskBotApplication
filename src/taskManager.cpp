#include <iostream>
#include "taskManager.h"
taskManager::taskManager(User* user) : _userPtr(user) {}

void taskManager::addTask(const std::string& taskName) {
    _userPtr->userTasks.push_back(taskName);
}

void taskManager::getAllTasks() const {
    for (size_t i = 0; i < _userPtr->userTasks.size(); i++) {
        std::cout << i + 1 << ". " <<_userPtr->userTasks[i].getTaskName() << std::endl;
    }
}