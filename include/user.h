#include <string>
#include <vector>
#include "../include/task.h"
class User{
private:
std::string _login;
std::string password;

public:
std::vector<Task> userTasks;
User(std::string,std::string);
std::string getLogin();
};