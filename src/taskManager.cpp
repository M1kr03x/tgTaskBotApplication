#include <iostream>
#include "taskManager.h"
taskManager::taskManager(Database& database, User* user) 
    : db(database), _userPtr(user) {}

void taskManager::addTask(const std::string& taskName) {
     if (!_userPtr) {
        std::cerr << "No user logged in\n";
        return;
    }
    db.addTask(taskName, _userPtr->getId());
}

void taskManager::getAllTasks() const {
   if (!_userPtr) {
        std::cerr << "No user logged in\n";
        return;
    }
    
    auto tasks = db.getUserTasks(_userPtr->getId(), false);
    
    if (tasks.empty()) {
        std::cout << "No tasks found\n";
        return;
    }
    
    for (const auto& task : tasks) {
        std::cout << task.taskID << ". " 
                  << (task.taskStatus == "Completed" ? " [Completed] " : " [Uncompleted] ")
                  << task.taskName << "\n";
    }
}
bool taskManager::deleteTask(int id){
     return db.deleteTask(id); 

}
bool taskManager::completeTask(int ID){
 return db.completeTask(ID); 
}