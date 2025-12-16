#pragma once

// Does nothing at compile time, but is needed for IDE to link .tcc to .h properly
#include "assets_manager.h"

#include <any>
#include <optional>
#include "utils/logger.h"

namespace engn {

template<typename TAsset>
std::optional<TAsset> AssetsManager::get_asset(const std::string& asset_id)
{
    auto it = m_assets.find(asset_id);
    if (it == m_assets.end()) {
        LOG_ERROR("Asset with id '{}' not found", asset_id);
        return std::nullopt;
    }
    try {
        TAsset asset = std::any_cast<TAsset>(*(it->second));
        return asset;
    } catch (const std::bad_any_cast& e) {
        LOG_ERROR("Failed to cast asset with id '{}': {}", asset_id, e.what());
        return std::nullopt;
    }
}

} // namespace engn
