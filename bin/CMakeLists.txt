add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} 
        PRIVATE cpr::cpr
        PRIVATE tlib
        PRIVATE nlohmann_json::nlohmann_json
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})