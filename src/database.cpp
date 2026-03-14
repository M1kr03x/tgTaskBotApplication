#include "database.h"
#include <iostream>

pqxx::work Database::createTransaction(){
    return pqxx::work(*conn);
}
Database::Database(const std::string&dbname, const std::string&user,const std::string& password, const std::string& host, const std::string&port){
      std::lock_guard<std::mutex> lock(dbMutex);
    std::string conn_prm = "host=" + host +" port="+port +" dbname="+dbname+" user="+user+" password="+ password;
    try{
        conn = std::make_unique<pqxx::connection>(conn_prm);
        if(conn->is_open()){
            std::cout << "DB: "<< conn->dbname() <<" is active";
        }
        else{
            std::cerr << "Failed to stable DB connection";
        }}
        catch(const std::exception& e) { std::cerr <<"Error: " << e.what() <<std::endl;
        conn = nullptr;
        }
    }
Database::~Database(){
        if(conn and conn->is_open()){
            conn->close();
        }
    }

bool Database::isConnected() const{return conn && conn->is_open();}
                                                                                                    //USER QUERY MANIPULATION

bool Database::deleteUser(const std::string&login){
    
      std::lock_guard<std::mutex> lock(dbMutex);
     if(!isConnected()) {std::cout << "DB connection is closed.. \n"; return false;}
     try{
        auto transaction = createTransaction();
        auto result = transaction.exec_params("DELETE FROM \"User\" WHERE login = $1 RETURNING userID",login);
          if (result.empty()) {
            std::cout << "User " << login << " not found\n";
            transaction.abort();
            return false;
        }
        transaction.commit();
        std::cout<< "User: "<< login <<" with ID "<< result[0][0].as<int>() <<" has been succesfuly deleted\n";
        return true;
     } catch(const pqxx::foreign_key_violation&){std::cerr << "User have tasks, cannot delete them\n"; return false;}
     catch(const std::exception&e){ std::cerr<< "Unpredictable error: " << e.what()<< std::endl;return false;}
}
std::vector<Database::TaskData>Database::getUserTasks(int userID,bool onlyUncompleted){
     std::lock_guard<std::mutex> lock(dbMutex);
    std::vector<TaskData> tasks;
    try {
        auto transaction = createTransaction();
        
        std::string query = "SELECT taskID, taskName, taskStatus,deadline,notify FROM \"Task\" WHERE userID = $1";
        if (onlyUncompleted) {
            query += " AND taskStatus = 'uncompleted'";
        }
        query += " ORDER BY taskID";
        auto result = transaction.exec_params(query, userID);
        for(const auto& row : result){
            TaskData task;
            task.taskStatus = row[2].as<std::string>();
            task.taskID = row[0].as<int>();
            task.taskName = row[1].as<std::string>();
             if (row[3].is_null()) {
                task.deadline = 0;
            } else {
                task.deadline = row[3].as<time_t>();
            }
            task.notifible = row[4].as<bool>();
            tasks.push_back(task);
        } 
        return tasks;
        }catch (const std::exception&e){std::cerr<<"Unexpected error.. "<< e.what() <<std::endl;}
        return tasks;
}
                                                                                        //TASKS QUERY MANIPULATION
bool Database::addTask(const std::string&taskN,int userID){
      std::lock_guard<std::mutex> lock(dbMutex);
    if(!isConnected()) {std::cout << "DB connection is closed.. \n"; return false;}
    try{
    auto transaction = createTransaction();
    auto result = transaction.exec_params("INSERT INTO \"Task\"(taskName,taskStatus,userID) VALUES($1,$2,$3) RETURNING taskID",taskN,"Uncompleted",userID);
    transaction.commit();
    std::cout<< "Task " << taskN <<" with ID " << result[0][0].as<int>() << " has been succesfuly added\n"; 
    return true;
    } catch(std::exception&e){std::cerr<<"Unexpected error.. "<< e.what()<<std::endl; return false;}

}
bool Database::addTaskWithDeadline(const std::string&taskN,int userID,time_t deadline){
      std::lock_guard<std::mutex> lock(dbMutex);
    if(!isConnected()) {std::cout << "DB connection is closed.. \n"; return false;}
    try{
    auto transaction = createTransaction();
    auto result = transaction.exec_params("INSERT INTO \"Task\"(taskName,taskStatus,userID,deadline,notify) VALUES($1,$2,$3,$4,$5) RETURNING taskID",taskN,"Uncompleted",userID,static_cast<long long>(deadline),false);
    transaction.commit();
    std::cout<< "Task " << taskN <<" with ID " << result[0][0].as<int>() << " has been succesfuly added\n"; 
    return true;
    } catch(std::exception&e){std::cerr<<"Unexpected error.. "<< e.what()<<std::endl; return false;}

}
bool Database::deleteTask(int id){
      std::lock_guard<std::mutex> lock(dbMutex);
    if(!isConnected()) {std::cout << "DB connection is closed.. \n"; return false;}
    try{
    auto transaction = createTransaction();
    auto result = transaction.exec_params("DELETE FROM \"Task\" WHERE taskID = $1 RETURNING taskID",id);
    transaction.commit();
    if (result.empty()) {
            std::cout << "Task with ID " << id << " not found\n";
            return false;
        }
        
    std::cout<<"Task #" <<result[0][0].as<int>() << " succesfuly deleted\n";
        return true;
    }catch(const std::exception&e){ std::cerr<< "Unpredictable error: " << e.what()<< std::endl;return false;}
}
bool Database::completeTask(int taskID){
      std::lock_guard<std::mutex> lock(dbMutex);
    if(!isConnected()) {std::cout << "DB connection is closed.. \n"; return false;}
    try{
    auto transaction = createTransaction();
    auto result = transaction.exec_params("UPDATE \"Task\" SET taskStatus = 'Completed' WHERE taskID = $1 RETURNING taskID",taskID);
    transaction.commit();
    if(result.empty()){
        std::cout << "Task with ID " << taskID << " not found\n";
            return false;
    } std::cout << "Task #" << taskID << " marked as completed\n";
    return true;

} catch(const std::exception&e){ std::cerr<< "Unpredictable error: " << e.what()<< std::endl;return false;}

}
std::optional<int> Database::findUserByTelegramId(const int64_t tgID){
      std::lock_guard<std::mutex> lock(dbMutex);
    try{
    if(!isConnected()) {std::cout<< "DB connection is closed.. \n"; return std::nullopt;}
    auto transaction = createTransaction();
    auto result = transaction.exec_params("SELECT userid from \"User\" WHERE telegramid = $1",tgID);
    transaction.commit();
    if(result.empty()) return std::nullopt;
    return result[0][0].as<int>();
    }catch(std::exception&){return std::nullopt;}
}
std::optional<int> Database::addTgUser(const std::string& login, const int64_t tgID){
      std::lock_guard<std::mutex> lock(dbMutex);
    try{
        if(!isConnected()) return std::nullopt;
         auto transaction = createTransaction();
         auto result = transaction.exec_params("INSERT INTO \"User\"(login,telegramid) VALUES ($1,$2) RETURNING userid",login,tgID);
         transaction.commit();
         return result[0][0].as<int>();


    }catch(std::exception&e){return std::nullopt;}
}
bool Database::setNotificationsEnabled(const int userID, bool condition){
    std::lock_guard<std::mutex> lock(dbMutex);
    try{
        if(!isConnected()){ return false;  }
        auto transaction = createTransaction();
        auto result = transaction.exec_params("UPDATE \"User\" set \"notifications_enabled\" = $1 where userid = $2 RETURNING userid",condition,userID);
        transaction.commit();
        if(!result.empty()){
            return true;
        }
        else return false;

    } catch (std::exception&e){std::cerr << "Something went wrong.." << e.what(); return false;}
}
bool Database::setTaskNotify(int taskID, bool condition){
    std::lock_guard<std::mutex> lock(dbMutex);
     try{
        if(!isConnected()){ return false;  }
         if(!isConnected()){ return false;  }
        auto transaction = createTransaction();
        auto result = transaction.exec_params("UPDATE \"Task\" set notify = $1 where taskid = $2 RETURNING taskid",condition,taskID);
        transaction.commit();
        if(!result.empty()){
            return true;
        }
        else return false;

     }catch (std::exception&e){std::cerr << "Something went wrong.." << e.what(); return false;}
}