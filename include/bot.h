#pragma once
#include <tgbot/tgbot.h>
#include <memory>
#include <thread>
#include "database.h"
#include "userManager.h"

class telegramBot{
    private:
    std::optional<std::pair<std::string, std::optional<time_t>>> stringWorker(std::string);
    std::thread bgThread;
    std::atomic<bool> bgRunning{true};
    void backgroundWorker();
    std::unique_ptr<TgBot::Bot> bot;
    Database &database;
    userManager uMng;
    void setupHandlers();
    public:
    telegramBot(const std::string&,Database&);
    void start();
    std::optional<time_t> parseTime(const std::string&);
    std::optional<time_t> parseDate(const std::string&);
    std::optional<time_t> parseDateTime(const std::string&, const std::string&);
    int getTgUserId(TgBot::Message::Ptr);
     ~telegramBot();
};