#include <iostream>
#include "taskManager.h"
taskManager::taskManager(User* user) : _userPtr(user) {}

void taskManager::addTask(const std::string& taskName) {
    _userPtr->userTasks.push_back(taskName);
}

void taskManager::getAllTasks() const {
    const auto& tasks = _userPtr->getTasksVec();  
    for (size_t i = 0; i < tasks.size(); i++) {
        std::cout << i + 1 << ". " << tasks[i].getTaskName() << (tasks[i].isCompleted()? "  |  [Completed]":"   |  [Uncompleted]") << std::endl;
    }
}
bool taskManager::deleteTask(int id){
    auto& tasksVector = _userPtr->getTasksVec();
    for(unsigned i{0};i<tasksVector.size();i++){
        if(tasksVector[i].getTaskId() == id) {tasksVector.erase(tasksVector.begin() + i); return true;}
    }
    return false;

}
bool taskManager::completeTask(int ID){
    auto& tasksVector = _userPtr->getTasksVec();
     for(unsigned i{0};i<tasksVector.size();i++){
        if(tasksVector[i].getTaskId() == ID) {tasksVector[i].setCompleted(); return true;}
    }
    return false;

}