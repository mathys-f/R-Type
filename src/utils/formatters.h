#include <format>
#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>

template <>
struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
    auto format(const std::filesystem::path &p, auto &ctx) const {
        return std::formatter<std::string>::format(p.string(), ctx);
    }
};

template <>
struct std::formatter<ecs::Entity> : std::formatter<size_t> {
    auto format(const ecs::Entity &entity, auto &ctx) const {
        return std::formatter<size_t>::format(static_cast<size_t>(entity), ctx);
    }
};
