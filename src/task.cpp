#include "../include/task.h"

Task::Task(std::string name) : _taskName(name), _taskId(++_counter), _isCompleted(false){

}
void Task::setCompleted(){
    _isCompleted = true;
}
