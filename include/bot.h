#pragma once
#include <tgbot/tgbot.h>
#include <memory>
#include "database.h"
#include "userManager.h"

class telegramBot{
    private:
    std::unique_ptr<TgBot::Bot> bot;
    Database &database;
    userManager uMng;
    void setupHandlers();
    public:
    telegramBot(const std::string&,Database&);
    void start();

    int getTgUserId(TgBot::Message::Ptr);
};