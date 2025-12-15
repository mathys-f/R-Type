#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <any>

namespace engn {

class AssetsManager {
 public:
    using Asset = std::string;

    AssetsManager() = default;
    ~AssetsManager() = default;

    template<typename TAsset, typename... TArgs>
    TAsset& load_asset(const std::string& asset_id, TArgs&&... args);

    void unload_asset(const std::string& asset_id);

    template<typename TAsset>
    TAsset& get_asset(const std::string& asset_id);

 private:
    std::unordered_map<std::string, std::shared_ptr<std::any>> m_assets;
};

}
