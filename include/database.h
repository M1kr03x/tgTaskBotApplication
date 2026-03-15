
#pragma once
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <mutex>
#include<chrono>
class Database{
private:
std::mutex dbMutex;  
std::unique_ptr<pqxx::connection> conn;
pqxx::work createTransaction();
public:
Database (const std::string&,const std::string&,const std::string&,const std::string&,const std::string&);
~Database();
//dbconn
bool isConnected() const;
//userQuery


std::optional<int> addTgUser(const std::string&,const int64_t);
std::optional<int> findUserByTelegramId(int64_t);

bool userExists(const std::string&);
bool deleteUser(const std::string&);
//task query
bool addTask(const std::string&,int);
bool addTaskWithDeadline(const std::string&, int, time_t);
bool completeTask(int);
bool deleteTask(int);
//structs
 struct TaskData {
        int taskID;
        std::string taskName;
        std::string taskStatus;
        time_t deadline;
        bool notifible;
    };
//getters
std::vector<TaskData> getUserTasks(int,bool);
pqxx::connection& getConnecton();
//Notifications
bool setNotificationsEnabled(int, bool);
bool getNotificationsEnabled(int userId);

bool setTaskNotify(int, bool);
bool getTaskNotify(int);

//threadNotficationsSender
void checkDeadlines(std::function<void(int64_t, const std::string&,int)> sendCallback);
bool setPingTime(int,int);
};
