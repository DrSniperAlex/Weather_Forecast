#pragma once

#include "API.hpp"
#include "Config.hpp"
#include "Console.hpp"
#include "Weather.hpp"
#include <iostream>

class Forecast
{
private:
    ConfigParser config_parser;
    Config cfg;
    Weather weather;
    Console printer;

public:
    Forecast() {};

    void Start() {
        std::pair<std::string, std::string> data = printer.GetUserInfo();

        std::string api_key = data.first;
        std::filesystem::path cfg_path = data.second;
        
        Weather weather(cfg_path, api_key);
        ConfigParser config_parser(cfg_path);

        config_parser.Parse();
        cfg = config_parser.GetConfig();

        printer.SetConfig(cfg);
        printer.PrintResults(weather);
    }
};
