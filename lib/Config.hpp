#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Config
{
    std::vector<std::string> cities_;
    uint8_t num_days_;
};

class ConfigParser
{
private:
    Config config_;
    json data_;
    std::ifstream file_;

public:
    ConfigParser() {}

    ConfigParser(const std::filesystem::path& path) 
        : file_(path) 
    {}

    ~ConfigParser() {
        file_.close();
    }

    void Parse() {
        try {
            data_ = json::parse(file_);
        } catch (...) {
            throw std::invalid_argument("Parsing config file failed.");
        }
        config_.cities_ = data_["cities"].get<std::vector<std::string>>();
        config_.num_days_ = data_["days"].get<uint8_t>();
    }

    Config GetConfig() const {
        return config_;
    }
};
