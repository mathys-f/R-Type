#pragma once

#include "formatters.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
#include <unordered_map>

#define LOG_DEBUG(format, ...)                                                                                         \
    logs::Logger::get_instance().log(logs::LogLevel::Debug, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)                                                                                          \
    logs::Logger::get_instance().log(logs::LogLevel::Info, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)                                                                                       \
    logs::Logger::get_instance().log(logs::LogLevel::Warning, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)                                                                                         \
    logs::Logger::get_instance().log(logs::LogLevel::Error, std::source_location::current(), format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...)                                                                                         \
    logs::Logger::get_instance().log(logs::LogLevel::Fatal, std::source_location::current(), format, ##__VA_ARGS__)

namespace logs {

enum class LogLevel { Debug, Info, Warning, Error, Fatal };

inline const char* to_color(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "\033[0;35m"; // Purple
        case LogLevel::Info:
            return "\033[0;36m"; // Cyan
        case LogLevel::Warning:
            return "\033[0;93m"; // Yellow
        case LogLevel::Error:
            return "\033[1;91m"; // Red
        case LogLevel::Fatal:
            return "\033[1;91m"; // Bright red
    }
    return "\033[0;37m";
}

inline const char* to_string(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Fatal:
            return "FATAL";
    }
    return "UNKNOWN";
}

class Logger {
  public:
    static Logger& get_instance() {
        static Logger s_instance;
        return s_instance;
    }

    void set_minimum_log_level(LogLevel level) {
        minimum_log_level = level;
    }

    template <typename... TArgs>
    void log(LogLevel level, const std::source_location& location, std::string_view formatStr, const TArgs&... args) {
        if (level < minimum_log_level)
            return;

        std::string message = std::vformat(formatStr, std::make_format_args(args...));
        std::string timestamp = get_timestamp();
        auto file = std::filesystem::path(location.file_name()).filename().string();

        std::lock_guard<std::mutex> lock(log_mutex);
        std::cout << timestamp << " [" << to_color(level) << to_string(level) << "\033[0m]"
                  << " [" << file << ":" << location.line() << "] " << message << std::endl;
    }

  private:
    Logger() = default;
    std::mutex log_mutex;
    LogLevel minimum_log_level = LogLevel::Debug;
    static constexpr std::size_t k_timestamp_buffer_size = 20;

    std::string get_timestamp() const {
        std::time_t now = std::time(nullptr);
        std::array<char, k_timestamp_buffer_size> buffer{};
        std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &now); // Windows: thread-safe
#else
        localtime_r(&now, &tm); // Linux/Unix: thread-safe
#endif
        std::strftime(buffer.data(), buffer.size(), "%F %T", &tm);
        return std::string(buffer.data());
    }
};

} // namespace logs
