#include "../include/commands.h"
#include "../include/bot.h"
void CommandsInterface::execute(TgBot::Message::Ptr msg,telegramBot& tgbot){
    try{
        auto userID = tgbot.getTgUserId(msg);
        run(msg, tgbot, userID);
    }catch(std::exception&e){std::cerr << e.what();return;}
}
void AddCommand::run(TgBot::Message::Ptr message, telegramBot& tgbot, int userID){
std::string taskText = getArgs(message->text);
if (taskText.empty()) {
       tgbot.getApi().getApi().sendMessage(message->chat->id, "Введите текст задачи");
        return;
    }
    auto res = tgbot.stringWorker(taskText); 
    if (!res) {
        tgbot.getApi().getApi().sendMessage(message->chat->id, "Ошибка формата");
        return;
    }

    auto [taskName, deadlineDate] = res.value();

    bool success = false;
    if (deadlineDate.has_value()) {
        success = tgbot.getDbConnection().addTaskWithDeadline(taskName, userID, deadlineDate.value());
    } else {
        success = tgbot.getDbConnection().addTask(taskName, userID);
    }

    if (success) {
        tgbot.getApi().getApi().sendMessage(message->chat->id, "Задача добавлена");
    }
}
void DeleteCommand::run(TgBot::Message::Ptr message, telegramBot& tgbot, int userId) {
    std::string args = getArgs(message->text);
    if (args.empty()) {
        tgbot.getApi().getApi().sendMessage(message->chat->id, "Укажите номер задачи: /delete 1");
        return;
    }
    int localIndex = std::stoi(args);
    auto tasks = tgbot.getDbConnection().getUserTasks(userId, false);

    if (localIndex < 1 || localIndex > (int)tasks.size()) {
        tgbot.getApi().getApi().sendMessage(message->chat->id, "Задача с таким номером не найдена");
        return;
    }
    int realId = tasks[localIndex - 1].taskID;
    if (tgbot.getDbConnection().deleteTask(realId)) {
        tgbot.getApi().getApi().sendMessage(message->chat->id, "Задача удалена");
    }
}
std::string CommandsInterface::getArgs(const std::string& text) {
    size_t spacePos = text.find(' ');
    if (spacePos == std::string::npos) return "";
    return text.substr(spacePos + 1);


}