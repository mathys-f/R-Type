#include <codecvt>
#include <filesystem>
#include <format>
#include <locale>
#include <string>

template <> struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
    auto format(const std::filesystem::path& p, auto& ctx) const {
        return std::formatter<std::string>::format(p.string(), ctx);
    }
};
