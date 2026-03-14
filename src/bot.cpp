#include "bot.h"
#include <iostream>
#include <regex>

telegramBot::telegramBot(const std::string& token, Database& db) : bot(std::make_unique<TgBot::Bot>(token)), database(db), uMng(db){
    setupHandlers();
    //bgThread = std::thread(&telegramBot::backgroundWorker, this);
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
            "   *Available commands:*\n"
            "/start  - Welcome message\n"
            "/help - Show this help\n"
            "/add <task> <date time (optional)> - Add new task\n"
            "/setTaskNotify <task id> <true|false> - set notify status for concrete tasks\n"
            "/setUserNotify <true|false> set notify mode for all tasks\n"
            "/list - Show all tasks\n"
            "/complete <id> - Mark task as completed\n"
            "/delete <id> - Delete task";
            
        bot->getApi().sendMessage(message->chat->id, helpText);
    });
    bot->getEvents().onCommand("add",[this](TgBot::Message::Ptr message){
        try{
        auto userid = getTgUserId(message);
        std::string text{message->text};
        if(text.size()<=5){bot->getApi().sendMessage(message->chat->id,"Usage /add <task name> <date time>. \nExample: Some task 13.02.2004 15:00\n"); return;}
        std::string taskText = text.substr(5);
        auto res = stringWorker(taskText);
        if(!res){
            bot->getApi().sendMessage(message->chat->id,"Wrong input");
            return;
        }
        auto[taskName, deadlineDate] = res.value();
        if (deadlineDate.has_value()) {
    std::cout << "DEBUG: deadline = " << deadlineDate.value() << std::endl;
} else {
    std::cout << "DEBUG: no deadline" << std::endl;
}

       
        if (taskName.empty()) {
            bot->getApi().sendMessage(message->chat->id, "Task name cannot be empty");
            return;
        }

        if(deadlineDate.has_value()){
            if(database.addTaskWithDeadline(taskName,userid,deadlineDate.value())) { bot->getApi().sendMessage(message->chat->id,"Task [ " + taskName + " ]" + " with deadline " + "added");}
            else{ bot->getApi().sendMessage(message->chat->id, "Failed to add task"); }
        }
        else {
            if (database.addTask(taskName, userid)) {
                bot->getApi().sendMessage(message->chat->id, "Task [ " + taskName + " ] added");
            } else {
                bot->getApi().sendMessage(message->chat->id, "Failed to add task");
            }
        }
    }  catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        bot->getApi().sendMessage(message->chat->id, "Internal error");
    }
});
   bot->getEvents().onCommand("list", [this](TgBot::Message::Ptr message) {
    try {
        int local = 1;
        auto userID = getTgUserId(message);
        auto tasks = database.getUserTasks(userID, false);
        
        if (tasks.empty()) {
            bot->getApi().sendMessage(message->chat->id, "No tasks found");
            return;
        }
        
        std::string response = "Your tasks:\n";
        
        for (const auto& task : tasks) {
            response += std::to_string(local++) + ". ";
            response += (task.taskStatus == "Completed" ? "[ completed ]   " : "[ uncompleted ]   ");
            response += task.taskName;
            if (task.deadline != 0) {
                char buffer[30];
                struct tm* timeinfo = localtime(&task.deadline);
                strftime(buffer, sizeof(buffer), " (до %d.%m.%Y %H:%M)", timeinfo);
                response += buffer;
            }
            response += (task.notifible == true ? " [ Notify enabled ]" : " [ Notify disabled ]");
            response += "\n";
        }
        
        bot->getApi().sendMessage(message->chat->id, response);
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in /list: " << e.what() << std::endl;
        bot->getApi().sendMessage(message->chat->id, "Internal error");
    }
});
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
bot->getEvents().onCommand("setTaskNotify", [this](TgBot::Message::Ptr message) {
    try {
        int userid = getTgUserId(message);
        std::string text = message->text;
        if (text.length() < 15) { 
            bot->getApi().sendMessage(message->chat->id, 
                "Usage: /setTaskNotify <task_number> <on/off>");
            return;
        }
        std::string args = text.substr(15);
        size_t spacePos = args.find(' ');
        
        if (spacePos == std::string::npos) {
            bot->getApi().sendMessage(message->chat->id, 
                "Usage: /setTaskNotify <task_number> <on/off>");
            return;
        }
        std::string numStr = args.substr(0, spacePos);
        std::string mode = args.substr(spacePos + 1);
        int localNumber = std::stoi(numStr);
        bool notify = (mode == "on");
        auto tasks = database.getUserTasks(userid, false);
        
        if (localNumber < 1 || localNumber > tasks.size()) {
            bot->getApi().sendMessage(message->chat->id, 
                "Task #" + numStr + " not found");
            return;
        }
        int realTaskId = tasks[localNumber - 1].taskID;
        if (database.setTaskNotify(realTaskId, notify)) {
            std::string status = notify ? "ON" : "OFF";
            bot->getApi().sendMessage(message->chat->id, 
                "Task #" + numStr + " notifications turned " + status);
        } else {
            bot->getApi().sendMessage(message->chat->id, 
                "Failed to update task");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
});
bot->getEvents().onCommand("setUserNotify", [this](TgBot::Message::Ptr message) {
    try {
        int userid = getTgUserId(message);
        std::string text = message->text;
        
        if (text.length() < 15) { 
            bot->getApi().sendMessage(message->chat->id, 
                "Usage: /setUserNotify <on/off>");
            return;
        }
        
        std::string mode = text.substr(15);
        bool notify = (mode == "on");
        
        if (database.setNotificationsEnabled(userid, notify)) {
            std::string status = notify ? "ON" : "OFF";
            bot->getApi().sendMessage(message->chat->id, 
                "Notifications for all tasks turned " + status);
        } else {
            bot->getApi().sendMessage(message->chat->id, 
                "Failed to update notification settings");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in /setUserNotify: " << e.what() << std::endl;
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
std::optional<std::pair<std::string, std::optional<time_t>>> telegramBot::stringWorker(std::string userInput) {
    
    std::vector<std::string> words;
    std::string symbol(" ");
    size_t start = 0;
    auto endIterator = userInput.find_first_of(symbol);
    
    while (endIterator != std::string::npos) {
        if (endIterator > start) 
            words.push_back(userInput.substr(start, endIterator - start));
        start = endIterator + 1;
        endIterator = userInput.find_first_of(symbol, start);
    }
    if (start < userInput.length()) {
        words.push_back(userInput.substr(start));
    }
    
    if (words.empty()) return std::nullopt;
    
    std::regex timeRegex(R"(^([0-9]|1[0-9]|2[0-3]):[0-5][0-9]$)");
    std::regex dateRegex(R"(^(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[012])\.(19|20)\d\d$)");
    
    size_t wordCount = words.size();
    std::string taskName;
    std::optional<time_t> deadline = std::nullopt;
    
    size_t nameEndIndex = wordCount; 
    
    if (wordCount >= 2) {
        bool lastIsTime = std::regex_match(words[wordCount - 1], timeRegex); //T    //F
        bool lastIsDate = std::regex_match(words[wordCount - 1], dateRegex); //F    //T
        bool prevIsDate = (wordCount >= 3) and std::regex_match(words[wordCount - 2], dateRegex);
        
        if (wordCount >= 3 and prevIsDate and lastIsTime) {
            nameEndIndex = wordCount - 2;
            deadline = parseDateTime(words[wordCount - 2], words[wordCount - 1]);
        }
        else if (lastIsDate || lastIsTime) {
            nameEndIndex = wordCount - 1;
            deadline = parseDate(words.back());
        }
        else if (lastIsTime) {
            nameEndIndex = wordCount - 1;
            deadline = parseTime(words.back());
        }
    }
    for (size_t i = 0; i < nameEndIndex; ++i) {
        if (i > 0) taskName += " ";
        taskName += words[i];
    }
    
    return std::make_pair(taskName, deadline);
}
std::optional<time_t> telegramBot::parseTime(const std::string& str) {
    int hour, minute;
    if (sscanf(str.c_str(), "%d:%d", &hour, &minute) != 2) 
        return std::nullopt;
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
        return std::nullopt;
    
    time_t now = time(nullptr);
    tm* tm_now = localtime(&now);
    tm_now->tm_hour = hour;
    tm_now->tm_min = minute;
    tm_now->tm_sec = 0;
    return mktime(tm_now);
}
std::optional<time_t> telegramBot::parseDate(const std::string& str) {
    int day, month, year;
    if (sscanf(str.c_str(), "%d.%d.%d", &day, &month, &year) != 3)
        return std::nullopt;
    
    tm tm_date = {};
    tm_date.tm_mday = day;
    tm_date.tm_mon = month - 1;
    tm_date.tm_year = year - 1900;
    tm_date.tm_hour = 0;
    tm_date.tm_min = 0;
    tm_date.tm_sec = 0;
    return mktime(&tm_date);
}
std::optional<time_t> telegramBot::parseDateTime(const std::string& dateStr, const std::string& timeStr) {
    auto date = parseDate(dateStr);
    if (!date) return std::nullopt;
    
    int hour, minute;
    if (sscanf(timeStr.c_str(), "%d:%d", &hour, &minute) != 2)
        return std::nullopt;
    
    tm* tm_time = localtime(&date.value());
    tm_time->tm_hour = hour;
    tm_time->tm_min = minute;
    tm_time->tm_sec = 0;
    return mktime(tm_time);
}