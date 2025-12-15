#pragma once

#include <format>
#include <iostream>
#include <string>
#include <mutex>
#include <unordered_map>
#include <source_location>
#include <filesystem>

#include "formatters.h"

#define LOG_DEBUG(format, ...) Logs::Logger::get_instance().log(Logs::LogLevel::debug, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Logs::Logger::get_instance().log(Logs::LogLevel::info, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) Logs::Logger::get_instance().log(Logs::LogLevel::warning, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logs::Logger::get_instance().log(Logs::LogLevel::error, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) Logs::Logger::get_instance().log(Logs::LogLevel::fatal, std::source_location::current(), format, ##__VA_ARGS__)

namespace Logs {

enum class LogLevel {
    debug,
    info,
    warning,
    error,
    fatal
};

inline const char* to_color(LogLevel level) {
    switch (level) {
        case LogLevel::debug: return "\033[0;35m";   // Purple
        case LogLevel::info: return "\033[0;36m";    // Cyan
        case LogLevel::warning: return "\033[0;93m"; // Yellow
        case LogLevel::error: return "\033[1;91m";   // Red
        case LogLevel::fatal: return "\033[1;91m";   // Bright red
    }
    return "\033[0;37m";
}

inline const char* to_string(LogLevel level) {
    switch (level) {
        case LogLevel::debug: return "DEBUG";
        case LogLevel::info: return "INFO";
        case LogLevel::warning: return "WARNING";
        case LogLevel::error: return "ERROR";
        case LogLevel::fatal: return "FATAL";
    }
    return "UNKNOWN";
}

class Logger {
public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    void set_minimum_log_level(LogLevel level) {
        minimumLogLevel = level;
    }

    template<typename... Args>
    void log(LogLevel level, const std::source_location& location, std::string_view formatStr, Args&&... args) {
        if (level < minimumLogLevel) return;

        std::string message = std::vformat(formatStr, std::make_format_args(args...));
        std::string timestamp = get_timestamp();
        auto file = std::filesystem::path(location.file_name()).filename().string();

        std::lock_guard<std::mutex> lock(logMutex);
        std::cout << timestamp
            << " [" << to_color(level) << to_string(level) << "\033[0m]"
            << " [" << file << ":" << location.line() << "] "
            << message << std::endl;
    }

private:
    Logger() = default;
    std::mutex logMutex;
    LogLevel minimumLogLevel = LogLevel::debug;

    std::string get_timestamp() const {
        std::time_t now = std::time(nullptr);
        char buffer[20];
        std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &now);  // Windows: thread-safe
#else
        localtime_r(&now, &tm);  // Linux/Unix: thread-safe
#endif
        std::strftime(buffer, sizeof(buffer), "%F %T", &tm);
        return buffer;
    }
};

} // namespace Logs
