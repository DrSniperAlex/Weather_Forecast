#pragma once

#include <cpr/cpr.h>
#include <string>

using cpr::Get;
using cpr::Parameters;
using cpr::Url;
using cpr::Header;

const std::string CityData = "https://api.api-ninjas.com/v1/city";
const std::string ForecastData = "https://api.open-meteo.com/v1/forecast";

cpr::Response GetCoordinates(const std::string& city, const std::string& api_key) {
    return Get(Url{CityData},
            Parameters{{"name", city}},
            Header{{"X-Api-Key", api_key}});
}

cpr::Response GetForecast(const std::string& latitude, const std::string& longitude, const std::string& num_days) {
    return Get(Url{ForecastData},
            Parameters{{"latitude", latitude}, {"longitude", longitude}, {"forecast_days", num_days},
                        {"hourly", "temperature_2m,relativehumidity_2m,apparent_temperature,weathercode,windspeed_10m"},
                        {"timezone", "auto"}, {"daily", "weathercode"}});
}
