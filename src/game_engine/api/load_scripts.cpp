#include "api/lua.h"
#include "utils/logger.h"

using namespace engn;

void lua::load_lua_scripts_from_folder_recurse(sol::state& lua, const std::string& scripts_dir) {
    std::vector<std::string> scripts_paths;
    int scripts_loaded = 0;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(scripts_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".lua") {
            if (entry.path().filename().string().front() == '_') {
                LOG_DEBUG("Skipping lua script {}", entry.path().string());
                continue;
            }
            scripts_paths.push_back(entry.path().string());
        }
    }
    LOG_INFO("Loading {} lua scripts from {}", scripts_paths.size(), scripts_dir);

    for (const std::string& entry : scripts_paths) {
        try {
            lua.script_file(entry);
            LOG_DEBUG("Loaded lua script from {}", entry);
            scripts_loaded++;
        } catch (const sol::error& e) {
            LOG_ERROR("Failed to load lua script from {}: {}", entry, e.what());
        }
    }

    LOG_INFO("Succesfully loaded {}/{} lua scripts", scripts_loaded, scripts_paths.size());
}

void lua::load_lua_script_from_file(sol::state& lua, const std::string& file_path) {
    try {
        lua.script_file(file_path);
        LOG_INFO("Loaded lua script from {}", file_path);
    } catch (const sol::error& e) {
        LOG_ERROR("Failed to load lua script from {}: {}", file_path, e.what());
    }
}
