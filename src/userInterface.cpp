#include <iostream>
#include <string>
#include <limits>
#include "../include/userInterface.h"
void UserInterface::start(){
    while(true){
        showMenu();
        int choice;
        std::cout<< "Enter number to choise-> ";
        std::cin>>choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch(choice){
            case(1):handleLogin(); break;
            case(2): handleRegistration(); break;
            case(3): exit(0);
            default: std::cout<< "\n Wrong choise, try again";
        }

        

    }
}
void UserInterface::showMenu(){
std::cout<< "==========Main menu==========\n";
std::cout << "1. Log in\n";
std::cout<< "2. Registration\n";
std::cout<< "3. Exit\n";

}
void UserInterface::handleRegistration(){
    std::cout<< "==========Registration==========\n";
    std::string login;
    std::string password;
    std::cout<<"Enter Login: ";
    std::getline(std::cin,login);
    std::cout <<"\nEnter Password: ";
    std::getline(std::cin,password);
    if(userMng.registration(login,password)){
        std::cout << "\nSuccesful registration! Authorization..";
        userMng.login(login,password);
        runTaskManager();
    }
    else{
        std::cout << "\n Error. Login already taken";
    }

}
void UserInterface::handleLogin(){
     std::cout<< "==========Login==========\n";
    std::string login;
    std::string password;
    std::cout<<"Enter Login: ";
    std::getline(std::cin,login);
    std::cout <<"\nEnter Password: ";
    std::getline(std::cin,password);
    if(userMng.login(login,password)){
        std::cout << "Succesful authorization\n";
        runTaskManager();
    }
    else{
        std::cout << "\nWrong login or password\n";
    }
}
void UserInterface::runTaskManager(){
    User* currentUser = this->userMng.getCurrentUser();
     taskManager taskMgr(currentUser);
     std::string change;
     std::cout<< "==========Welcome, " << currentUser->getLogin() << "==========\n";
     std::cout<< "Task selection\n";
     std::cout<<"add <text> - add task\n";
     std::cout<<"list - show all tasks\n";
     std::cout<<"complete <task id> - mark task as completed\n";
     std::cout<<"delete <task id> - delete task\n";
     std::cout<<"logout - exit from account\n";
     std::cout<<"help - show all commands\n";
    while(true){
        std::cout<< "Enter command-> ";
        std::getline(std::cin, change);
         if(change.empty()) continue;
        if(change == "help") std::cout <<"Commands: add <text> || list || complete<task id> || delete<task id> || logout || help\n";
        else if(change =="list") taskMgr.getAllTasks();
        else if(change == "logout") {userMng.logout(); std::cout<<"See ya soon..!\n"; break;}
        else if(change.rfind("add ",0) == 0 and change.size()>4) {taskMgr.addTask(change.substr(4));}
       else if(change.rfind("complete ", 0) == 0 && change.length() > 9) {
    try {
        int taskId = std::stoi(change.substr(9)); 
        if (taskMgr.completeTask(taskId)) {
            std::cout << "Task completed!\n";
        } else {
            std::cout << "No task with this ID\n";
        }
    } catch (...) {
        std::cout << "Wrong ID format\n";
    }
}       else if (change.rfind("delete ", 0) == 0 && change.length() > 7) {
            try {
                int taskId = std::stoi(change.substr(7));
                if (taskMgr.deleteTask(taskId)) {
                    std::cout << "Task deleted\n";
                } else {
                    std::cout << "No task with this ID\n";
                }
            } catch (...) {
                std::cout << "Wrong ID format\n";
            }
        }
    }
}