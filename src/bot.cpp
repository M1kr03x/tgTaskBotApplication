#include "bot.h"
#include <iostream>

telegramBot::telegramBot(const std::string& token, Database& db) : bot(std::make_unique<TgBot::Bot>(token)), database(db), uMng(db){
    setupHandlers();
    bgThread = std::thread(&telegramBot::backgroundWorker, this);
}
int telegramBot::getTgUserId(TgBot::Message::Ptr mes){
    if (mes->chat->type != TgBot::Chat::Type::Private) {
        bot->getApi().sendMessage(mes->chat->id, 
            "Bot works only in private chats.");
        throw std::runtime_error("Not a private chat");
    }
      auto result = database.findUserByTelegramId(mes->chat->id);
        if(result!=std::nullopt) return result.value();
        auto logResult = database.addTgUser(mes->chat->username,mes->chat->id);
        if(logResult!=std::nullopt) return logResult.value();
        throw std::runtime_error("Failed to get or create user");
}
void telegramBot::start(){
    try{
    std::cout<< "Bot started\n";
    TgBot::TgLongPoll longPoll(*bot);
    while(true){
        longPoll.start();
    }
}catch(std::exception&e){std::cerr<<"Something went wrong.. " << e.what() << std::endl;}
}
void telegramBot::setupHandlers(){
    bot->getEvents().onCommand("start",[this](TgBot::Message::Ptr message) {
        bot->getApi().sendMessage(
            message->chat->id, "Hello, welcome to taskManagerBot.\n Use /help to see commands."
        );
    });
    bot->getEvents().onCommand("help", [this](TgBot::Message::Ptr message) {
        std::string helpText = 
            "📋 *Available commands:*\n"
            "/start - Welcome message\n"
            "/help - Show this help\n"
            "/add <task> - Add new task\n"
            "/list - Show all tasks\n"
            "/complete <id> - Mark task as completed\n"
            "/delete <id> - Delete task";
            
        bot->getApi().sendMessage(message->chat->id, helpText);
    });
    bot->getEvents().onCommand("add",[this](TgBot::Message::Ptr message){
        try{
        auto userid = getTgUserId(message);
        std::string text{message->text};
        if(text.size()<=5){bot->getApi().sendMessage(message->chat->id,"Please, use /add <task name>\n"); return;}
        std::string taskText = text.substr(5);
        if(taskText.empty()) {
        bot->getApi().sendMessage(message->chat->id, "Task name cannot be empty");
        return;
        }
        if(database.addTask(taskText, userid)){bot->getApi().sendMessage(message->chat->id, "Task " + taskText + " added");}
        else{bot->getApi().sendMessage(message->chat->id,"Failed to add task");}
        } catch(std::exception&e){std::cerr<<"Unexpected error: " << e.what() << std::endl;
        bot->getApi().sendMessage(message->chat->id,"Internal error");
        }
    });
    bot->getEvents().onCommand("list",[this](TgBot::Message::Ptr message){
        try{
            int local =1;
        auto userID = getTgUserId(message);
        auto tasks = database.getUserTasks(userID, false);
         if (tasks.empty()) {
            bot->getApi().sendMessage(message->chat->id, "No tasks found\n");
            return;
        }
        std::string stringtasks{"Tasks\n"};
        for(const auto&vec : tasks){
           stringtasks+="Task #" + std::to_string((local++)) + " : " + vec.taskName + " status: " + vec.taskStatus + "\n";
        }
        bot->getApi().sendMessage(message->chat->id,stringtasks);
    }catch (const std::exception& e){std::cerr<< "Unexpected error: " << e.what(); bot->getApi().sendMessage(message->chat->id,"Unexpected error :("); }
}
);
bot->getEvents().onCommand("complete",[this](TgBot::Message::Ptr message){
 try {
        int userid = getTgUserId(message);
        std::string text = message->text;
        if (text.length() < 10) { 
            bot->getApi().sendMessage(message->chat->id, 
                "Usage: /complete <task_number>");
            return;
        }
        std::string numStr = text.substr(10);
        int localNumber = std::stoi(numStr);
        auto tasks = database.getUserTasks(userid, false);
        if (localNumber < 1 || localNumber > tasks.size()) {
            bot->getApi().sendMessage(message->chat->id, 
                "Task #" + numStr + " not found");
            return;
        }
        int realTaskId = tasks[localNumber - 1].taskID;
        if (database.completeTask(realTaskId)) {
            bot->getApi().sendMessage(message->chat->id, 
                "Task #" + numStr + " completed");
        } else {
            bot->getApi().sendMessage(message->chat->id, 
                "Failed to complete task");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in /complete: " << e.what() << std::endl;
        bot->getApi().sendMessage(message->chat->id, "Invalid task number");
    }

});
bot->getEvents().onCommand("delete",[this](TgBot::Message::Ptr message){
 try {
        int userid = getTgUserId(message);
        std::string text = message->text;
        if (text.length() < 8) { 
            bot->getApi().sendMessage(message->chat->id, 
                "Usage: /delete <task_number>");
            return;
        }
        std::string numStr = text.substr(8);
        int localNumber = std::stoi(numStr);
        auto tasks = database.getUserTasks(userid, false);
        if (localNumber < 1 || localNumber > tasks.size()) {
            bot->getApi().sendMessage(message->chat->id, 
                "Task #" + numStr + " not found");
            return;
        }
        int realTaskId = tasks[localNumber - 1].taskID;
        if (database.deleteTask(realTaskId)) {
            bot->getApi().sendMessage(message->chat->id, 
                "Task #" + numStr + " deleted");
        } else {
            bot->getApi().sendMessage(message->chat->id, 
                "Failed to delete task");
        }
        //
    } catch (const std::exception& e) {
        std::cerr << "Error in /delete: " << e.what() << std::endl;
        bot->getApi().sendMessage(message->chat->id, "Invalid task number");
    }

});
}
void telegramBot::backgroundWorker() {
    while (bgRunning) {
        //some Stub
    }
}
telegramBot::~telegramBot() {
    bgRunning = false;
    if (bgThread.joinable()) {
        bgThread.join();
    }
}