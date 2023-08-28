cmake_minimum_required(VERSION 3.27)
project(system)

set(CMAKE_CXX_STANDARD 17)
find_package(ftxui CONFIG REQUIRED)
add_executable(system main.cpp)

#链接库
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ftxui::screen)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ftxui::dom)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ftxui::component)

#将exe程序复制到根目录下
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/"
        "${PROJECT_SOURCE_DIR}/file")