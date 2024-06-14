#pragma once

#include <functional>
#include "Weather.hpp"

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/dom/table.hpp"

using namespace ftxui;

const uint8_t kMinDays = 1;
const uint8_t kMaxDays = 16;
const uint8_t kHoursPerDay = 24;
const uint8_t kInfoOfDay = 4;
const uint8_t kBoxSize = 80;
const uint8_t kSmallBoxSize = 25;
const uint8_t kUnknownCode = 145;

const uint8_t kStartOfNight = 0;
const uint8_t kEndOfNight = 5;

const uint8_t kStartOfMorning = 6;
const uint8_t kEndOfMorning = 12;

const uint8_t kStartOfAfternoon = 13;
const uint8_t kEndOfAfternoon = 18;

const uint8_t kStartOfEvening = 19;
const uint8_t kEndOfEvening = 23;

const std::vector<std::string> daytime = {"Morning", "Noon", "Evening", "Night"};

class Console 
{
private:
    Config cfg_;
    std::unordered_map<uint8_t, std::string> names_;
    std::unordered_map<uint8_t, std::function<std::vector<Element>()>> images_;

    const std::string text_0 = "This project is a simple weather forecast application, that allows users to get weather forecast for any city in the world based on the city's coordinates.";
    const std::string text_1 = "    How to use the application:";
    const std::string text_2 = "1. Enter your API key when prompted.";
    const std::string text_3 = "2. Provide a path to the configuration file if available.";
    const std::string text_4 = "3. Press \"Enter\" and wait a bit for the application to get needed information.";
    const std::string text_5 = "4. Use keyboard commands to navigate through cities and adjust the forecast duration:";
    const std::string text_6 = "`+` : Increase the number of forecasted days (up to a maximum limit).";
    const std::string text_7 = "`-` : Decrease the number of forecasted days (down to a minimum limit).";
    const std::string text_8 = "`n` : Move to the next city.";
    const std::string text_9 = "`p` : Move to the previous city.";
    const std::string text_10 = "`Esc` : Exit the application";


    std::pair<std::string, std::string> data = {"", std::filesystem::path(__FILE__).remove_filename().string() += "cfg.json"}; // first - API, second - config path
    int has_error = 0; // 0 - no error, 1 - empty api key, 2 - invalid config path

public:
    Console() {
        FillMaps();
    }

    std::pair<std::string, std::string> GetUserInfo() {
        ClearScreen();
        auto screen = ScreenInteractive::TerminalOutput();
        // Search Engine Title
        auto title = Renderer([&] {
            return text("Weather Forecast") | color(Color::White) 
                   | flex | bold | center;
        });
        // Instructions
        auto instructions = Renderer([&] {
            return window(text("Instructions:"), 
                          vbox({
                            text(text_0) | color(Color::White),
                            text(text_1) | color(Color::DarkSeaGreen1),
                            text(text_2) | color(Color::GrayLight),
                            text(text_3) | color(Color::GrayLight),
                            text(text_4) | color(Color::GrayLight),
                            text(text_5) | color(Color::GrayLight),
                            text(text_6) | color(Color::DarkSeaGreen3),
                            text(text_7) | color(Color::DarkSeaGreen3),
                            text(text_8) | color(Color::DarkSeaGreen3),
                            text(text_9) | color(Color::DarkSeaGreen3),
                            text(text_10) | color(Color::DarkSeaGreen3),
                          })
                          ) | color(Color::DarkSeaGreen1)
                            | size(HEIGHT, EQUAL, 12);
        });
        // API key Input
        std::string api_key = "";
        auto api_input = Input(&api_key, "Enter your API key here");
        api_input = Wrap("(Required) Api-Ninjas api key:", api_input);
        api_input |= CatchEvent([&](Event event) {
            if (event.input() == "\n") {
                screen.ExitLoopClosure()();
                return true;
            }  
            return false;
        });
        // Config File Input
        std::string config_path = "";
        auto config_input = Input(&config_path, "C:\\\\folder\\sub_folder\\config_file.json");
        config_input = Wrap("(Optional) Config file path:", config_input);
        config_input |= CatchEvent([&](Event event) {
            if (event.input() == "\n") {
                screen.ExitLoopClosure()();
                return true;
            }  
            return false;
        });
        // Exit Button
        bool exit_is_active = false;
        auto exit_action = [&] { 
            exit_is_active = true;
            screen.ExitLoopClosure()();
        };
        auto exit_button = Button("Exit Weather Forecast", exit_action, ButtonOption::Animated(Color::DarkSeaGreen3)) 
                        | border
                        | size(WIDTH, EQUAL, 25);

        // Whole lower layout
        auto lower_layout = Container::Horizontal({
            exit_button
        }) | xflex;
        // Error message (if it occurs)
        Element error_message;
        if (has_error == 1) {
            error_message = text("Error: Empty api key. Please enter api key.") 
                | bgcolor(Color::DarkSeaGreen3) | color(Color::Black) | xflex;
        } else if (has_error == 2) {
            error_message = text("Error: Invalid config file path. Please enter valid config file path.") 
                | bgcolor(Color::DarkSeaGreen3) | color(Color::Black) | xflex;
        } else {
            error_message = text("") 
                | xflex;
        }
        auto error = Renderer([&] {
                return hbox({
                    error_message
                }) | xflex;
            });
        // Layout
        auto layout = Container::Vertical({
            title,
            instructions,
            api_input,
            config_input,
            lower_layout,
            error
        });
        // Main render component
        auto component = Renderer(layout, [&] {
            return vbox({
                    title->Render(),
                    instructions->Render(),
                    api_input->Render(),
                    config_input->Render(),
                    lower_layout->Render(),
                    separator(),
                    error->Render()
                }) | flex | border | color(Color::White);
        });
        screen.Loop(component);
        // Check if the user wants to quit
        if (exit_is_active)
            exit(0);
        // Check if the api key has been entered
        if (api_key.empty()) {
            has_error = 1;
            ClearScreen();
            data = GetUserInfo(); // Inform user and try again
        } else if (!config_path.empty() && !CheckFile(config_path)) {
            has_error = 2;
            ClearScreen();
            data = GetUserInfo(); // Inform user and try again
        } else if (!config_path.empty()) {
            has_error = 0;
            data = {api_key, config_path};
        } else {
            has_error = 0;
            data.first = api_key;
        }
        return data;
    }

    void PrintResults(Weather& weather) {
        ClearScreen();
        auto screen = ScreenInteractive::FitComponent();
        auto current_city = cfg_.cities_.begin();
        // Main table
        auto table = Renderer([&] {
            json forecast;
            forecast = weather.ParseWeather(*current_city, cfg_.num_days_);
            Elements windows;
            windows.push_back(text("Weather forecast for: " + *current_city) | center | bold | color(Color::White));
            
            for (size_t day = 0; day < cfg_.num_days_; ++day) {
            Elements row;
            row.reserve(daytime.size());

            for (const auto& time : daytime) {
                row.push_back(vbox({text(time) | center | bold,
                                    separator(),
                                    hbox({
                                    vbox(GetImage(GetWeatherCode(forecast, day, time))),
                                    vbox(GetDescription(forecast, day, time)) | border | size(WIDTH, EQUAL, kSmallBoxSize)
                                    })}) | border | size(WIDTH, EQUAL, kBoxSize));
            }

            windows.push_back(window(
                text(forecast["daily"]["time"][day].get<std::string>()) | color(Color::DarkSeaGreen1) | center,
                vbox(hbox(std::move(row)))));
            }
            return vbox(std::move(windows));
        });
        // Layout
        auto layout = Container::Vertical({
            table
        });
        // Main render component
        auto component = Renderer(layout, [&] {
            return vbox({
                    table->Render() | vscroll_indicator | yframe | size(HEIGHT, LESS_THAN, 50)
                }) | flex | border;
        });
        component = CatchEvent(component, [&](const Event& event) {
            if (event == Event::Escape)
                screen.ExitLoopClosure()();
            else if (event.input() == "+") {
                if (cfg_.num_days_ < kMaxDays) cfg_.num_days_++;
            } else if (event.input() == "-") {
                if (cfg_.num_days_ > kMinDays) cfg_.num_days_--;
            } else if (event.input() == "n") {
                ++current_city;
                if (current_city == cfg_.cities_.end())
                    current_city = cfg_.cities_.begin();
            } else if (event.input() == "p") {
                if (current_city == cfg_.cities_.begin())
                    current_city = cfg_.cities_.end();
                --current_city;
            }
            return false;
        });

        Loop loop(&screen, component);
        while(!loop.HasQuitted()) {
            screen.Post(ftxui::Event::Custom);
            loop.RunOnce();
        }
    }

    void SetConfig(const Config& cfg) {
        cfg_ = cfg;
    }

private:
    inline bool CheckFile(const std::string& file_path) {
        std::ifstream f(file_path.c_str());
        return f.good();
    }

    void ClearScreen() const {
        std::system("cls");
    }

    std::string GetName(const uint8_t code) {
        if (names_.find(code) != names_.end()) {
            return names_[code];
        }
        return names_[kUnknownCode];
    }

    std::vector<Element> GetImage(const uint8_t code) {
        if (images_.find(code) != images_.end()) {
            return images_[code]();
        }
        return images_[kUnknownCode]();
    }

    std::vector<Element> GetDescription(const json& weather, const uint8_t day, const std::string& time) {
        uint8_t begin{};
        uint8_t end{};

        if (time == "Morning") {
            begin = kStartOfMorning;
            end = kEndOfMorning;
        } else if (time == "Noon") {
            begin = kStartOfAfternoon;
            end = kEndOfAfternoon;
        } else if (time == "Evening") {
            begin = kStartOfEvening;
            end = kEndOfEvening;
        } else {
            begin = kStartOfNight;
            end = kEndOfNight;
        }

        std::vector<Element> description;
        description.reserve(kInfoOfDay);

        size_t current_day = kHoursPerDay * day;
        description.push_back(text(GetName(weather["hourly"]["weathercode"][current_day + end].get<uint8_t>())) | color(Color::DarkSeaGreen1));
        Element temperatures = hbox(GetTempColor(GetAverage(weather["hourly"]["temperature_2m"],
                                            current_day + begin,
                                            current_day + end)),
                                    text("("),
                                    GetTempColor(GetAverage(weather["hourly"]["apparent_temperature"],
                                            current_day + begin,
                                            current_day + end)),
                                    text(")°C"));
        description.push_back(temperatures);

        description.push_back(text(GetAverage(weather["hourly"]["windspeed_10m"],
                                            current_day + begin,
                                            current_day + end) + " км/ч"));

        description.push_back(text(GetAverage(weather["hourly"]["relativehumidity_2m"],
                                            current_day + begin,
                                            current_day + end) + "%"));
        return description;
    }

    uint8_t GetWeatherCode(const json& weather, const uint8_t day, const std::string& time) const {
        uint8_t end = 0;
        if (time == "Morning") {
            end = kEndOfMorning;
        } else if (time == "Noon") {
            end = kEndOfAfternoon;
        } else if (time == "Evening") {
            end = kEndOfEvening;
        } else {
            end = kEndOfNight;
        }

        size_t current = day * kHoursPerDay + end;
        return weather["hourly"]["weathercode"][current].get<size_t>();
    }

    std::string GetAverage(const nlohmann::basic_json<>& current, size_t left, size_t right) const {
        return current != 0 ? std::to_string(int((current[left].get<double>() + current[right].get<double>()) / 2)) : "error";
    }

    Element GetTempColor(const std::string& temperature) const {
        int temp = std::stoi(temperature);
        Element result;
        if (temp < -10) {
            return text(temperature) | color(Color::NavyBlue);
        } else if (temp >= -10 && temp < -5) {
            return text(temperature) | color(Color::Blue);
        } else if (temp >= -5 && temp < 0) {
            return text(temperature) | color(Color::BlueLight);
        } else if (temp >= 0 && temp < 5) {
            return text(temperature) | color(Color::Green);
        } else if (temp >= 5 && temp < 10) {
            return text(temperature) | color(Color::GreenLight);
        } else if (temp >= 10 && temp < 15) {
            return text(temperature) | color(Color::Yellow);
        } else if (temp >= 15 && temp < 20) {
            return text(temperature) | color(Color::YellowLight);
        } else if (temp >= 20 && temp < 25) {
            return text(temperature) | color(Color::Salmon1);
        } else {
            return text(temperature) | color(Color::RedLight);
        }
    }

    Component Wrap(std::string name, Component component) {
        return Renderer(component, [name, component] {
            return hbox({
                    text(name) | color(Color::DarkSeaGreen1) | size(WIDTH, EQUAL, 30),
                    separator(),
                    component->Render() | xflex,
                }) | xflex | borderDouble | bold;
        });
    }

    void FillMaps() {
        names_[0] = "Clear sky";
        names_[1] = "Mainly clear";
        names_[2] = "Partly cloudy";
        names_[3] = "Overcast";

        names_[45] = names_[48] = "Fog";

        names_[51] = "Light drizzle";
        names_[53] = "Moderate drizzle";
        names_[55] = "Dense drizzle";
        names_[56] = "Light freezing drizzle";
        names_[57] = "Dense freezing drizzle";

        names_[61] = "Slight rain";
        names_[63] = "Moderate rain";
        names_[65] = "Heavy rain";

        names_[66] = "Light freezing rain";
        names_[67] = "Heavy freezing rain";

        names_[71] = "Slight snow fall";
        names_[73] = "Moderate snow fall";
        names_[75] = "Heavy snow fall";

        names_[77] = "Snow grains";

        names_[80] = "Slight rain showers";
        names_[81] = "Moderate rain showers";
        names_[82] = "Violent rain showers";

        names_[95] = "Thunderstorm";
        names_[96] = "Thunderstorm with slight hail";
        names_[99] = "Thunderstorm with heavy hail";

        names_[kUnknownCode] = "Неизвестный Код";


        images_[0] = []() {
            return std::vector<Element> {
            text("    \\    |    /   ") | color(Color::YellowLight),
            text("     '.-----.'     ") | color(Color::YellowLight),
            text("     /       \\    ") | color(Color::YellowLight),
            text(" ---|         |--- ") | color(Color::YellowLight),
            text("     \\       /    ") | color(Color::YellowLight),
            text("     ,'-----'.     ") | color(Color::YellowLight),
            text("    /    |    \\   ") | color(Color::YellowLight)};
        };

        images_[1] = images_[2] = []() {
            return std::vector<Element> {
            hbox(text("    \\    ") | color(Color::YellowLight), text("|") | color(Color::White), text("~       ~~") | color(Color::GrayLight)),
            hbox(text("     '.--") | color(Color::YellowLight), text("|") | color(Color::White), text(",----,    ") | color(Color::GrayLight)),
            hbox(text("     /   ") | color(Color::YellowLight), text("|") | color(Color::White), text(" --   )~  ") | color(Color::GrayLight)),
            hbox(text(" ---|    ") | color(Color::YellowLight), text("|") | color(Color::White), text(" '-'   )  ") | color(Color::GrayLight)),
            hbox(text("     \\   ") | color(Color::YellowLight), text("|") | color(Color::White), text("--/    )  ") | color(Color::GrayLight)),
            hbox(text("     ,'--") | color(Color::YellowLight), text("|") | color(Color::White), text("\"-----'    ") | color(Color::GrayLight)),
            hbox(text("    /    ") | color(Color::YellowLight), text("|") | color(Color::White), text("     ~~  ") | color(Color::GrayLight))};
        };

        images_[3] = images_[45] = images_[48] = []() {
            return std::vector<Element> {
            hbox(text("  ~~     ~~~      ~~") | color(Color::White)),
            hbox(text("   ,-----,,----,") | color(Color::GrayLight), text("~~  ") | color(Color::White)),
            hbox(text("  (  --    --   )") | color(Color::GrayLight) , text("~~ ") | color(Color::White)),
            hbox(text(" (   '-'   '-'   )  ") | color(Color::GrayLight)),
            hbox(text(" ~") | color(Color::White), text("(     ---/    )   ") | color(Color::GrayLight)),
            hbox(text("   '-----\"\"----' ") | color(Color::GrayLight), text("~~ ") | color(Color::White)),
            hbox(text("     ~~~      ~~~   ") | color(Color::White))};
        };

        images_[51] = images_[53] = images_[55] = images_[56] = images_[57] = images_[61] = []() {
            return std::vector<Element> {
            text("   ,-----,,----,    ") | color(Color::GrayLight),
            text("  (  --    --   )   ") | color(Color::GrayLight),
            text(" (   '-'   '-'   )  ") | color(Color::GrayLight),
            text("  (      ---/    )  ") | color(Color::GrayLight),
            text("   '---,-\"\"--,--'   ") | color(Color::GrayLight),
            text("  , , ,  ,  , ,     ") | color(Color::BlueLight),
            text(" , ,   ,  , ,       ") | color(Color::BlueLight)};
        };

        images_[63] = images_[65] = images_[80] = images_[81] = images_[82] = []() {
            return std::vector<Element> {
            text("   ,-----,,----,    ") | color(Color::GrayDark),
            text("  (  -^-   -^-  )   ") | color(Color::GrayDark),
            text(" (   '.'   '.'   )  ") | color(Color::GrayDark),
            text("  (     _---_    )   ") | color(Color::GrayDark),
            text("   '---,-\"\"--,--'  ") | color(Color::GrayDark),
            text("  /  /    / /  /    ") | color(Color::Blue),
            text("  /  / /   /   /    ") | color(Color::Blue)};
        };

        images_[66] = images_[67] = []() {
            return std::vector<Element> {
            text("   ,-----,,----,    ") | color(Color::GrayDark),
            text("  (  -^-   -^-  )   ") | color(Color::GrayDark),
            text(" (   '.'   '.'   )  ") | color(Color::GrayDark),
            text("  (     _---_    )  ") | color(Color::GrayDark),
            text("   '---,-\"\"--,--'   ") | color(Color::GrayDark),
            hbox(text("  *  ") | color(Color::White), text("/  ") | color(Color::BlueLight), 
                text("* ") | color(Color::White), text("/ ") | color(Color::BlueLight), 
                text("*  ") | color(Color::White), text("/ ") | color(Color::BlueLight), 
                text("*  ") | color(Color::White)),
            hbox(text(" /  ") | color(Color::BlueLight), text("* ") | color(Color::White), text("/ ") | color(Color::BlueLight), 
                text("* ") | color(Color::White), text("/  ") | color(Color::BlueLight), text("* ") | color(Color::White), 
                text("/    ") | color(Color::BlueLight))};
        };

        images_[71] = images_[73] = images_[75] = images_[77] = []() {
            return std::vector<Element> {
            text("   ,-----,,----,   ") | color(Color::GrayLight),
            text("  (  --    --   )  ") | color(Color::GrayLight),
            text(" (   '-'   '-'   ) ") | color(Color::GrayLight),
            text("  (      ---/    ) ") | color(Color::GrayLight),
            text("   '---,-\"\"--,-'   ") | color(Color::GrayLight),
            text("   *  *   * *  *   ") | color(Color::White),
            text("  *  *  *  *  *    ") | color(Color::White)};
        };

        images_[95] = images_[96] = images_[99] = []() {
            return std::vector<Element> {
            text("   ,-----,,----,      ") | color(Color::GrayDark),
            text("  (  -^-   -^-  )     ") | color(Color::GrayDark),
            text(" (    .     .    )    ") | color(Color::GrayDark),
            text("  (   \\^-^-^/    )   ") | color(Color::GrayDark),
            text("   ',,-,--,,--,-'     ") | color(Color::GrayDark),
            text("    //    //  //      ") | color(Color::YellowLight),
            text("    \\\\   \\\\   \\\\  ") | color(Color::YellowLight),
            text("    /    /     /      ") | color(Color::YellowLight)};
        };

        images_[kUnknownCode] = []() {
            return std::vector<Element> {
            text("   ,------.   ") | color(Color::Magenta),
            text("  '  .--.  '  ") | color(Color::Magenta),
            text("  '--' _|  |  ") | color(Color::Magenta),
            text("    .--' __'  ") | color(Color::Magenta),
            text("    `---'     ") | color(Color::Magenta),
            text("    .---.     ") | color(Color::Magenta),
            text("    '---'     ") | color(Color::Magenta)};
        };
    }
};
