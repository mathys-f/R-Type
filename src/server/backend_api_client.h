#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using Json = nlohmann::json;

namespace engn::backend_api {
inline constexpr std::uint16_t k_default_backend_port = 8081;
}

namespace engn {

/**
 * HTTP client for communicating with Node.js backend API
 * Handles lobby creation, Player sessions, scores, and match finalization
 */
class BackendAPIClient {
  public:
  struct BanCheckResult {
    bool m_is_banned = false;
    std::string m_reason;
  };

    /**
     * Initialize the API client with backend host and port
     * @param host Backend server hostname or IP (e.g., "localhost" or "127.0.0.1")
     * @param port Backend server port (default 8081)
     */
    BackendAPIClient(const std::string& host = "localhost", std::uint16_t port = backend_api::k_default_backend_port);

    /**
     * Create a new lobby via the backend API
     * @param name Lobby name
     * @param max_players Max players allowed
     * @return Lobby ID on success, std::nullopt on failure
     */
    std::optional<std::uint32_t> create_lobby(const std::string& name, std::uint8_t max_players);

    /**
     * Add a Player session to the backend
     * @param lobby_id The lobby ID from create_lobby
     * @param player_name Name of the Player
     * @param account_id Optional account ID if Player logged in
     * @param ip_address Optional IP address of Player
     * @return Session ID on success, std::nullopt on failure
     */
    std::optional<std::uint32_t> add_player_session(std::uint32_t lobby_id, const std::string& player_name,
                                                    std::optional<std::uint32_t> account_id = std::nullopt,
                                                    std::optional<std::string> ip_address = std::nullopt);

    /**
     * Update a Player's score in real-time
     * @param session_id The session ID from add_player_session
     * @param kills Number of kills
     * @param deaths Number of deaths
     * @param score Total score
     * @return true on success, false on failure
     */
    bool update_player_score(std::uint32_t session_id, std::uint32_t kills, std::uint32_t deaths, std::uint32_t score);

    /**
     * Finalize a match and save results to backend
     * @param lobby_id The lobby ID
     * @return true on success, false on failure
     */
    bool finalize_match(std::uint32_t lobby_id);

    /**
     * Update lobby Player count in backend database
     * @param lobby_id The lobby ID
     * @param player_count Current number of players
     * @return true on success, false on failure
     */
    bool update_lobby_player_count(std::uint32_t lobby_id, std::uint8_t player_count);

    /**
     * Check if a player is banned by username.
     * @param player_name Player username to verify
     * @return Ban check result on success, std::nullopt on request failure
     */
    std::optional<BanCheckResult> check_player_ban(const std::string& player_name);

    /**
     * Get the last error message
     */
    const std::string& get_last_error() const {
        return m_last_error;
    }

  private:
    std::string m_host;
    std::uint16_t m_port;
    std::string m_last_error;

    /**
     * Make an HTTP POST request to the backend
     * @param endpoint API endpoint path (e.g., "/api/game/lobby/create")
     * @param body JSON request body
     * @return Response JSON on success, std::nullopt on failure
     */
    std::optional<Json> post_request(const std::string& endpoint, const Json& body);
};

} // namespace engn
