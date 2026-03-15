#include "bot.h"
#include <iostream>
#include <regex>
#include <ctime>
#include <optional>
#include <string>
#include <cstdio>

tm get_localtime(const time_t* timer) {
    tm result;
#ifdef _WIN32
    localtime_s(&result, timer);
#else
    localtime_r(timer, &result);
#endif
    return result;
}

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
            "   *Available commands:*\n"
            "/start  - Welcome message\n"
            "/help - Show this help\n"
            "/add <task> <date time (optional)> - Add new task\n"
            "/setTaskNotify <task id> <on|off> - set notify status for concrete tasks\n"
            "/setNotifyTime <seconds> - set ping time between notifiers\n"
            "/setUserNotify <on|off> - set notify mode for all tasks\n"
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
            response += "Task status - ";
            response += (task.taskStatus == "Completed" ? "[ completed ]   " : "[ uncompleted ]   ");
            response +="\nTask name: "+ task.taskName;
            if (task.deadline != 0) {
                char buffer[60];
                struct tm* timeinfo = localtime(&task.deadline);
                strftime(buffer, sizeof(buffer), " | Выполнить до %d.%m.%Y %H:%M)", timeinfo);
                response += buffer;
            }
            response+= "\nNotifieble - ";
            response += (task.notifible == true ? " [ On ] \n" : " [ Off ]  \n");
            response += "\n\n";
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
bot->getEvents().onCommand("setnotifytime", [this](TgBot::Message::Ptr message) {
    try {
        int64_t tgUserId = message->from->id; 
        std::string text = message->text;

        size_t spacePos = text.find(' ');
        if (spacePos == std::string::npos || spacePos + 1 >= text.size()) {
            bot->getApi().sendMessage(message->chat->id, "Usage: /setNotifyTime <seconds>");
            return;
        }

        std::string numStr = text.substr(spacePos + 1);
        
        int seconds;
        try {
            seconds = std::stoi(numStr);
        } catch (...) {
            bot->getApi().sendMessage(message->chat->id, "Error: Please enter a valid number.");
            return;
        }

        if (seconds < 0) {
            bot->getApi().sendMessage(message->chat->id, "Error: Seconds must be greater than 0.");
            return;
        }

        if (database.setPingTime(seconds, tgUserId)) {
            bot->getApi().sendMessage(message->chat->id, "Ping time successfully changed.");
        } else {
            bot->getApi().sendMessage(message->chat->id, "Error.");
        }

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        bot->getApi().sendMessage(message->chat->id, "⚠️ Internal error occurred.");
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
bot->getEvents().onCommand("settasknotify", [this](TgBot::Message::Ptr message) {
    try {
        int userid = getTgUserId(message);
        std::string text = message->text;
        if (text.length() < 15) { 
            bot->getApi().sendMessage(message->chat->id, 
                "Usage: /settasknotify <task_number> <on/off>");
            return;
        }
        std::string args = text.substr(15);
        size_t spacePos = args.find(' ');
        
        if (spacePos == std::string::npos) {
            bot->getApi().sendMessage(message->chat->id, 
                "Usage: /settasknotify <task_number> <on/off>");
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
bot->getEvents().onCommand("setusernotify", [this](TgBot::Message::Ptr message) {
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
        database.checkDeadlines([this](int64_t chat_id, const std::string& text, int sec) {
            try {
                this->bot->getApi().sendMessage(chat_id, text); 
            } catch (...) {}
        });
        std::this_thread::sleep_for(std::chrono::seconds(10));
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
    std::istringstream iss(userInput);
    std::string s;
    while (iss >> s) words.push_back(s);

    if (words.empty()) return std::nullopt;

    std::regex timeRegex(R"(^([0-9]|0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$)");
    std::regex dateRegex(R"(^(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[012])\.(19|[2-9][0-9])\d\d$)");

    int endIndex = static_cast<int>(words.size()) - 1;
    std::optional<time_t> deadline = std::nullopt;
    int taskLastWordIndex = endIndex; 

    bool isTime = std::regex_match(words[endIndex], timeRegex);

    if (isTime and endIndex >=1) {
        if (std::regex_match(words[endIndex - 1], dateRegex)) {
            deadline = parseDateTime(words[endIndex - 1], words[endIndex]);
            taskLastWordIndex = endIndex - 2;
        } else {
            deadline = parseTime(words[endIndex]);
            taskLastWordIndex = endIndex - 1;
        }
    } 
    else if (std::regex_match(words[endIndex], dateRegex)) {
        deadline = parseDate(words[endIndex]);
        taskLastWordIndex = endIndex - 1;
    }

    std::string taskName;
    for (int i = 0; i <= taskLastWordIndex; ++i) {
        taskName += words[i];
        if (i < taskLastWordIndex) taskName += " ";
    }

    if (taskName.empty()) return std::nullopt;
    
    return std::make_pair(taskName, deadline);
}



std::optional<time_t> telegramBot::parseDate(const std::string& str) {
    int d, m, y;
    if (sscanf(str.c_str(), "%d.%d.%d", &d, &m, &y) != 3) return std::nullopt;

    if (m < 1 || m > 12 || d < 1 || d > 31 || y < 1900) return std::nullopt;

    tm t = {};
    t.tm_mday = d;
    t.tm_mon = m - 1;
    t.tm_year = y - 1900;
    t.tm_isdst = -1; 

    time_t res = mktime(&t);
    return (res == -1) ? std::nullopt : std::make_optional(res);
}

std::optional<time_t> telegramBot::parseDateTime(const std::string& dateStr, const std::string& timeStr) {
    auto date_opt = parseDate(dateStr);
    if (!date_opt) return std::nullopt;

    int hh, mm;
    if (sscanf(timeStr.c_str(), "%d:%d", &hh, &mm) != 2) return std::nullopt;
    if (hh < 0 || hh > 23 || mm < 0 || mm > 59) return std::nullopt;

    tm t = get_localtime(&date_opt.value());
    t.tm_hour = hh;
    t.tm_min = mm;
    t.tm_sec = 0;
    t.tm_isdst = -1; 

    time_t res = mktime(&t);
    return (res == -1) ? std::nullopt : std::make_optional(res);
}
std::optional<time_t> telegramBot::parseTime(const std::string& str) {
    int hour, minute;
    if (sscanf(str.c_str(), "%d:%d", &hour, &minute) != 2) 
        return std::nullopt;

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
        return std::nullopt;
    
    time_t now = time(nullptr);
    tm t;
#ifdef _WIN32
    localtime_s(&t, &now);
#else
    localtime_r(&now, &t);
#endif

    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = 0;
    t.tm_isdst = -1; 

    time_t res = mktime(&t);
    return (res == -1) ? std::nullopt : std::make_optional(res);
}
