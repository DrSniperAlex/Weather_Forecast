#pragma once

#include "API.hpp"
#include "Config.hpp"

#include <cpr/cpr.h>

static const uint8_t kStatusCodeOK = 200;

class Weather
{
private:
    static inline std::string api_key_;
    static inline std::unordered_map<std::string, json> cities_locations_;
    std::filesystem::path file_;

public:
    Weather() {};

    Weather(const std::filesystem::path& path, const std::string& api) {
        file_ = path;
        api_key_ = api;
    }

    json ParseWeather(const std::string& city, uint8_t days) {
        json json_coordinates;
        if (cities_locations_.find(city) == cities_locations_.end()) {
            auto coordinates = GetCoordinates(city, api_key_);
            json_coordinates = json::parse(coordinates.text.substr(1, coordinates.text.size() - 2));
            cities_locations_[city] = json_coordinates;
        } else {
            json_coordinates = cities_locations_[city];
        }
        std::string coord_x = to_string(json_coordinates.at("latitude"));
        std::string coord_y = to_string(json_coordinates.at("longitude"));

        auto response_forecast = GetForecast(coord_x, coord_y, std::to_string(days));
        json json_forecast = json::parse(response_forecast.text);
        return json_forecast;
    }
};
