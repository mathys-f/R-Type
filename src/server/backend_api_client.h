#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace engn {

/**
 * HTTP client for communicating with Node.js backend API
 * Handles lobby creation, player sessions, scores, and match finalization
 */
class BackendAPIClient {
  public:
    /**
     * Initialize the API client with backend host and port
     * @param host Backend server hostname or IP (e.g., "localhost" or "127.0.0.1")
     * @param port Backend server port (default 8081)
     */
    BackendAPIClient(const std::string& host = "localhost", std::uint16_t port = 8081);
    
    /**
     * Create a new lobby via the backend API
     * @param name Lobby name
     * @param max_players Max players allowed
     * @return Lobby ID on success, std::nullopt on failure
     */
    std::optional<std::uint32_t> create_lobby(const std::string& name, std::uint8_t max_players);
    
    /**
     * Add a player session to the backend
     * @param lobby_id The lobby ID from create_lobby
     * @param player_name Name of the player
     * @param account_id Optional account ID if player logged in
     * @param ip_address Optional IP address of player
     * @return Session ID on success, std::nullopt on failure
     */
    std::optional<std::uint32_t> add_player_session(
        std::uint32_t lobby_id,
        const std::string& player_name,
        std::optional<std::uint32_t> account_id = std::nullopt,
        std::optional<std::string> ip_address = std::nullopt
    );
    
    /**
     * Update a player's score in real-time
     * @param session_id The session ID from add_player_session
     * @param kills Number of kills
     * @param deaths Number of deaths
     * @param score Total score
     * @return true on success, false on failure
     */
    bool update_player_score(
        std::uint32_t session_id,
        std::uint32_t kills,
        std::uint32_t deaths,
        std::uint32_t score
    );
    
    /**
     * Finalize a match and save results to backend
     * @param lobby_id The lobby ID
     * @return true on success, false on failure
     */
    bool finalize_match(std::uint32_t lobby_id);
    
    /**
     * Update lobby player count in backend database
     * @param lobby_id The lobby ID
     * @param player_count Current number of players
     * @return true on success, false on failure
     */
    bool update_lobby_player_count(std::uint32_t lobby_id, std::uint8_t player_count);
    
    /**
     * Get the last error message
     */
    const std::string& get_last_error() const { return m_last_error; }

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
    std::optional<json> post_request(const std::string& endpoint, const json& body);
};

} // namespace engn
