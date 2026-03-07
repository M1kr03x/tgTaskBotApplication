#pragma once
#include <string>
#include <vector>
class Task{
private:
std::string _taskName;
bool _isCompleted;
inline static int _counter;
int _taskId;
public:
Task(std::string);
std::string getTaskName() const { return _taskName; }
bool isCompleted() const { return _isCompleted; }
int getTaskId() const {return _taskId;}
void setCompleted();
};