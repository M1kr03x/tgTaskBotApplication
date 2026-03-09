#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cerrno>

struct Config {
    std::string botToken;
    std::string dbname;
    std::string dbUser;
    std::string dbPassword;
    std::string dbHost;
    std::string dbPort;
    
    static Config load(const std::string& filename = "../config.json") {
        std::cout << "Trying to open: " << filename << std::endl;
        
        std::ifstream f(filename);
        if (!f.is_open()) {
            std::cerr << "Failed to open file. Error: " << strerror(errno) << std::endl;
            throw std::runtime_error("Cannot open config file");
        }
        
        std::cout << "File opened successfully" << std::endl;
        
        nlohmann::json data;
        try {
            data = nlohmann::json::parse(f);
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            throw;
        }
        
        Config cfg;
        cfg.botToken = data["bot_token"];
        cfg.dbname = data["database"]["dbname"];
        cfg.dbUser = data["database"]["user"];
        cfg.dbPassword = data["database"]["password"];
        cfg.dbHost = data["database"]["host"];
        cfg.dbPort = data["database"]["port"];
        
        return cfg;
    }
};