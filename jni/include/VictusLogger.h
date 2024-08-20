//
// Created by bapan on 18/3/2024.
//
#include <iostream>
#include <string>

enum LogLevel { INFO, WARNING, ERROR, SUCCESS, FAILED };

void LogV(LogLevel level, const std::string& message) {
    std::string color;
    switch(level) {
        case INFO:
            color = "\033[1;36m";
            break;
        case WARNING:
            color = "\033[1;33m";
            break;
        case ERROR:
            color = "\033[1;31m";
            break;
        case SUCCESS:
            color = "\033[1;32m";
            break;
        case FAILED:
            color = "\033[1;35m";
            break;
        default:
            color = "\033[0m";
            break;
    }
    std::cout << color << message << "\033[0m" << std::endl;
}

void LogV(LogLevel level, const std::string& message, std::string &input) {
    std::string color;
    char c;
    switch(level) {
        case INFO:
            color = "\033[1;32m";
            break;
        case WARNING:
            color = "\033[1;33m";
            break;
        case ERROR:
            color = "\033[1;31m";
            break;
        case SUCCESS:
            color = "\033[1;32m";
            break;
        case FAILED:
            color = "\033[1;35m";
            break;
        default:
            color = "\033[0m";
            break;
    }
    std::cout << color << message << "\033[0m";
    while (true) {
        c = std::cin.get();
        if (c == '\n')
            break;
        std::cout << '*';
        input += c;
    }
    std::cout << std::endl;
}

void InputKey(const std::string& message) {
    std::string color = "\033[1;32m";
    std::cout << color << message;
}