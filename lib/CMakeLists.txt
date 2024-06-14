add_library(
        tlib
        API.hpp
        Config.hpp
        Console.hpp
        Weather.hpp
        Forecast.hpp
        test.cpp
)

target_link_libraries(
        tlib
        PRIVATE cpr::cpr
        PRIVATE nlohmann_json::nlohmann_json
        PRIVATE ftxui::dom ftxui::screen ftxui::component
)