#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <optional>

namespace engn {

class AssetsManager {
 public:
    using Asset = std::string;

    AssetsManager() = default;
    ~AssetsManager() = default;

    // Specific load functions for each asset type
    bool load_music(const std::string& asset_id, const std::string& file_path);
    bool load_sound(const std::string& asset_id, const std::string& file_path);
    bool load_texture(const std::string& asset_id, const std::string& file_path);

    void unload_asset(const std::string& asset_id);

    // Generic get function
    template<typename TAsset>
    std::optional<TAsset> get_asset(const std::string& asset_id);

 private:
    std::unordered_map<std::string, std::unique_ptr<std::any>> m_assets;
};

}

#include "assets_manager.tcc"
