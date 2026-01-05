#pragma once

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace engn {

class AssetsManager {
  public:
    using Asset = std::string;

    AssetsManager() = default;
    ~AssetsManager() = default;
    AssetsManager(const AssetsManager&) = delete;
    AssetsManager& operator=(const AssetsManager&) = delete;
    AssetsManager(AssetsManager&&) = delete;
    AssetsManager& operator=(AssetsManager&&) = delete;

    // Specific load functions for each asset type
    bool load_music(const std::string& asset_id, const std::string& file_path);
    bool load_sound(const std::string& asset_id, const std::string& file_path);
    bool load_texture(const std::string& asset_id, const std::string& file_path);

    void unload_asset(const std::string& asset_id);

    // Generic get function
    template <typename TAsset> std::optional<TAsset> get_asset(const std::string& asset_id);

  private:
    std::unordered_map<std::string, std::unique_ptr<std::any>> m_assets;
};

} // namespace engn

#include "assets_manager.tcc"
