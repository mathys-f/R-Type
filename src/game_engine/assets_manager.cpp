#include "assets_manager.h"

#include "utils/logger.h"

using namespace engn;

void AssetsManager::unload_asset(const std::string& asset_id)
{
    auto it = m_assets.find(asset_id);
    if (it != m_assets.end()) {
        m_assets.erase(it);
    } else {
        LOG_WARNING("Attempted to unload non-existing asset with id '{}'", asset_id);
    }
}
