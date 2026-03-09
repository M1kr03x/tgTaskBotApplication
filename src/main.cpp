#include "../include/bot.h"
#include <iostream>
#include <locale>
#include <nlohmann/json.hpp>
#include "../include/config.h"
int main() {

  try {
        auto cfg = Config::load("../config.json");
        Database db(cfg.dbname, cfg.dbUser, cfg.dbPassword, 
                    cfg.dbHost, cfg.dbPort);

        if (!db.isConnected()) {
            std::cerr << "Failed to connect to database.\n";
            return 1;
        }

        telegramBot bot(cfg.botToken, db);
        bot.start();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}