#include "user.h"
class taskManager{
    private:
    User* _userPtr;
    public:
    void addTask(const std::string&);
    void setTaskStatus(int);
    void getAllTasks() const;
    taskManager(User*);

};