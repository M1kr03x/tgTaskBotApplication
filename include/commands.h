#pragma once
#include <tgbot/tgbot.h>
class telegramBot;
class CommandsInterface{
    public:
virtual ~CommandsInterface() = default;
 void execute(TgBot::Message::Ptr,telegramBot&);
 protected:
 virtual void run(TgBot::Message::Ptr,telegramBot&,int);
 std::string getArgs(const std::string&);
};
class AddCommand : public CommandsInterface{
protected:
    void run(TgBot::Message::Ptr, telegramBot&, int) override;
};

class DeleteCommand : public CommandsInterface{
    protected:
void run(TgBot::Message::Ptr, telegramBot&, int) override;
};