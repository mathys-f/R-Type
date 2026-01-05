#include "assets_manager.h"

#include "raylib.h"
#include "utils/logger.h"

using namespace engn;

bool AssetsManager::load_music(const std::string& asset_id, const std::string& file_path) {
    Music music = LoadMusicStream(file_path.c_str());
    if (music.stream.buffer == nullptr) {
        LOG_ERROR("Failed to load Music asset '{}' from '{}'", asset_id, file_path);
        return false;
    }
    m_assets[asset_id] = std::make_unique<std::any>(std::move(music));
    return true;
}

bool AssetsManager::load_sound(const std::string& asset_id, const std::string& file_path) {
    Sound sound = LoadSound(file_path.c_str());
    if (sound.stream.buffer == nullptr) {
        LOG_ERROR("Failed to load Sound asset '{}' from '{}'", asset_id, file_path);
        return false;
    }
    m_assets[asset_id] = std::make_unique<std::any>(std::move(sound));
    return true;
}

bool AssetsManager::load_texture(const std::string& asset_id, const std::string& file_path) {
    Texture2D texture = LoadTexture(file_path.c_str());
    if (texture.id == 0) {
        LOG_ERROR("Failed to load Texture asset '{}' from '{}'", asset_id, file_path);
        return false;
    }
    if (m_assets.find(asset_id) != m_assets.end()) {
        LOG_WARNING("Overwriting existing asset with id '{}'", asset_id);
    }
    m_assets[asset_id] = std::make_unique<std::any>(std::move(texture));
    return true;
}

void AssetsManager::unload_asset(const std::string& asset_id) {
    auto it = m_assets.find(asset_id);
    if (it != m_assets.end()) {
        m_assets.erase(it);
    } else {
        // LOG_WARNING("Attempted to unload non-existing asset with id '{}'", asset_id);
    }
}
