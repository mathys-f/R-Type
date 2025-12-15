#pragma once

// Does nothing at compile time, but is needed for IDE to link .tcc to .h properly
#include "assets_manager.h"

#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <optional>

#include "utils/logger.h"

namespace engn {

template<typename TAsset, typename... TArgs>
bool AssetsManager::load_asset(const std::string& asset_id, TArgs&&... args)
{
    auto asset = TAsset(std::forward<TArgs>(args)...);

    if (asset == nullptr) {
        LOG_ERROR("Failed to load asset with id '{}'", asset_id);
        return false;
    }
    m_assets[asset_id] = std::make_unique<std::any>(std::move(asset));
    return true;
}

template<typename TAsset>
std::optional<TAsset&> AssetsManager::get_asset(const std::string& asset_id)
{
    auto it = m_assets.find(asset_id);
    if (it == m_assets.end()) {
        LOG_ERROR("Asset with id '{}' not found", asset_id);
        return std::nullopt;
    }
    try {
        TAsset& asset = std::any_cast<TAsset&>(*(it->second));
        return asset;
    } catch (const std::bad_any_cast& e) {
        LOG_ERROR("Failed to cast asset with id '{}': {}", asset_id, e.what());
        return std::nullopt;
    }
}

} // namespace engn
