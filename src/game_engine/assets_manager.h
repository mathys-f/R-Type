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

    template<typename TAsset, typename... TArgs>
    bool load_asset(const std::string& asset_id, TArgs&&... args);

    void unload_asset(const std::string& asset_id);

    template<typename TAsset>
    std::optional<TAsset&> get_asset(const std::string& asset_id);

 private:
    std::unordered_map<std::string, std::unique_ptr<std::any>> m_assets;
};

}
