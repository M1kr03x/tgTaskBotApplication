
#pragma once
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <optional>
#include <memory>

class Database{
private:
std::unique_ptr<pqxx::connection> conn;
pqxx::work createTransaction();
public:
Database (const std::string&,const std::string&,const std::string&,const std::string&,const std::string&);
~Database();
//dbconn
bool isConnected() const;
//userQuery
bool addUser(const std::string&,const std::string&);
std::optional<int> findUserID(const std::string &, std::string&);
bool userExists(const std::string&);
bool deleteUser(const std::string&);
//task query
bool addTask(const std::string&,int);
bool completeTask(int);
bool deleteTask(int);
//structs
 struct TaskData {
        int taskID;
        std::string taskName;
        std::string taskStatus;
    };
//getters
std::vector<TaskData> getUserTasks(int,bool);
pqxx::connection& getConnecton();


};
