#include "backend_api_client.h"
#include "utils/logger.h"
#include <httplib.h>

namespace {
    constexpr int k_http_ok = 200;
    constexpr int k_http_created = 201;
    constexpr int k_http_timeout_seconds = 2;
}

namespace engn {

BackendAPIClient::BackendAPIClient(const std::string& host, std::uint16_t port)
    : m_host(host), m_port(port) {}

std::optional<json> BackendAPIClient::post_request(const std::string& endpoint, const json& body) {
    try {
        httplib::Client cli(m_host, m_port);
        cli.set_connection_timeout(k_http_timeout_seconds, 0);
        cli.set_read_timeout(k_http_timeout_seconds, 0);
        
        auto res = cli.Post(endpoint, body.dump(), "application/json");
        
        if (!res) {
            m_last_error = "HTTP request failed: no response from backend";
            LOG_ERROR("Backend API error: {}", m_last_error);
            return std::nullopt;
        }
        
        if (res->status != k_http_ok && res->status != k_http_created) {
            m_last_error = "HTTP error " + std::to_string(res->status) + ": " + res->body;
            LOG_ERROR("Backend API error: {}", m_last_error);
            return std::nullopt;
        }
        
        return json::parse(res->body);
    } catch (const std::exception& e) {
        m_last_error = std::string("Exception: ") + e.what();
        LOG_ERROR("Backend API exception: {}", m_last_error);
        return std::nullopt;
    }
}

std::optional<std::uint32_t> BackendAPIClient::create_lobby(const std::string& name, std::uint8_t max_players) {
    json body;
    body["name"] = name;
    body["max_players"] = max_players;
    
    auto res = post_request("/api/game/lobby/create", body);
    if (!res.has_value()) {
        return std::nullopt;
    }
    
    try {
        const auto& json_res = res.value();
        if (json_res.contains("success") && json_res["success"].is_boolean()) {
            if (!json_res["success"].get<bool>()) {
                m_last_error = json_res.contains("error") ? json_res["error"].get<std::string>() : "Unknown error";
                LOG_ERROR("Failed to create lobby: {}", m_last_error);
                return std::nullopt;
            }
        }
        
        if (json_res.contains("lobby") && json_res["lobby"].contains("id")) {
            std::uint32_t lobby_id = json_res["lobby"]["id"].get<std::uint32_t>();
            LOG_INFO("Created lobby via backend API: ID {}", lobby_id);
            return lobby_id;
        }
        
        m_last_error = "Invalid response format from backend";
        return std::nullopt;
    } catch (const std::exception& e) {
        m_last_error = std::string("JSON parsing error: ") + e.what();
        LOG_ERROR("Backend response parsing failed: {}", m_last_error);
        return std::nullopt;
    }
}

std::optional<std::uint32_t> BackendAPIClient::add_player_session(
    std::uint32_t lobby_id,
    const std::string& player_name,
    std::optional<std::uint32_t> account_id,
    std::optional<std::string> ip_address
) {
    json body;
    body["lobbyId"] = lobby_id;
    body["playerName"] = player_name;
    if (account_id.has_value()) {
        body["accountId"] = account_id.value();
    }
    if (ip_address.has_value()) {
        body["ipAddress"] = ip_address.value();
    }
    
    auto res = post_request("/api/game/session/add", body);
    if (!res.has_value()) {
        return std::nullopt;
    }
    
    try {
        const auto& json_res = res.value();
        if (json_res.contains("success") && !json_res["success"].get<bool>()) {
            m_last_error = json_res.contains("error") ? json_res["error"].get<std::string>() : "Unknown error";
            LOG_ERROR("Failed to add player session: {}", m_last_error);
            return std::nullopt;
        }
        
        if (json_res.contains("session") && json_res["session"].contains("id")) {
            std::uint32_t session_id = json_res["session"]["id"].get<std::uint32_t>();
            LOG_INFO("Added player session to backend: Session ID {}", session_id);
            return session_id;
        }
        
        m_last_error = "Invalid response format from backend";
        return std::nullopt;
    } catch (const std::exception& e) {
        m_last_error = std::string("JSON parsing error: ") + e.what();
        LOG_ERROR("Backend response parsing failed: {}", m_last_error);
        return std::nullopt;
    }
}

bool BackendAPIClient::update_player_score(
    std::uint32_t session_id,
    std::uint32_t kills,
    std::uint32_t deaths,
    std::uint32_t score
) {
    json body;
    body["sessionId"] = session_id;
    body["kills"] = kills;
    body["deaths"] = deaths;
    body["score"] = score;

    auto res = post_request("/api/game/score/update", body);
    if (!res.has_value()) {
        return false;
    }

    try {
        const auto& json_res = res.value();
        bool success = json_res.contains("success") ? json_res["success"].get<bool>() : false;
        if (success) {
            LOG_DEBUG("Updated player score: Session {} - K:{} D:{} S:{}", session_id, kills, deaths, score);
        } else {
            m_last_error = json_res.contains("error") ? json_res["error"].get<std::string>() : "Unknown error";
            LOG_WARNING("Failed to update score: {}", m_last_error);
        }
        return success;
    } catch (const std::exception& e) {
        m_last_error = std::string("JSON parsing error: ") + e.what();
        LOG_ERROR("Backend response parsing failed: {}", m_last_error);
        return false;
    }
}

bool BackendAPIClient::finalize_match(std::uint32_t lobby_id) {
    json body;
    body["lobbyId"] = lobby_id;
    
    auto res = post_request("/api/game/match/finalize", body);
    if (!res.has_value()) {
        return false;
    }
    
    try {
        const auto& json_res = res.value();
        bool success = json_res.contains("success") ? json_res["success"].get<bool>() : false;
        if (success) {
            LOG_INFO("Finalized match for lobby {}", lobby_id);
        } else {
            m_last_error = json_res.contains("error") ? json_res["error"].get<std::string>() : "Unknown error";
            LOG_WARNING("Failed to finalize match: {}", m_last_error);
        }
        return success;
    } catch (const std::exception& e) {
        m_last_error = std::string("JSON parsing error: ") + e.what();
        LOG_ERROR("Backend response parsing failed: {}", m_last_error);
        return false;
    }
}

bool BackendAPIClient::update_lobby_player_count(std::uint32_t lobby_id, std::uint8_t player_count) {
    json body;
    body["lobbyId"] = lobby_id;
    body["playerCount"] = player_count;
    
    auto res = post_request("/api/game/lobby/update-count", body);
    if (!res.has_value()) {
        return false;
    }
    
    try {
        const auto& json_res = res.value();
        bool success = json_res.contains("success") ? json_res["success"].get<bool>() : false;
        return success;
    } catch (const std::exception& e) {
        m_last_error = std::string("JSON parsing error: ") + e.what();
        return false;
    }
}

} // namespace engn
