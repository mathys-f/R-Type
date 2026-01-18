#include "admin_http_server.h"
#include "utils/logger.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

AdminHTTPServer::AdminHTTPServer(LobbyManager* lobby_manager, std::uint16_t port)
    : m_lobby_manager(lobby_manager), m_port(port), m_server(std::make_unique<httplib::Server>()), m_server_thread(nullptr) {
    setup_routes();
}

AdminHTTPServer::~AdminHTTPServer() {
    stop();
}

void AdminHTTPServer::setup_routes() {
    // Health check endpoint
    m_server->Get("/health", [](const httplib::Request& /*req*/, httplib::Response& res) {
        json response = {{"status", "ok"}, {"service", "r-type-game-server"}};
        res.set_content(response.dump(), "application/json");
        res.status = http::k_status_ok;
    });

    // Stop lobby endpoint (called by Node.js admin API)
    m_server->Post("/admin/lobby/stop", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto request_json = json::parse(req.body);
            
            if (!request_json.contains("lobby_id")) {
                json error = {{"success", false}, {"error", "Missing lobby_id"}};
                res.set_content(error.dump(), "application/json");
                res.status = http::k_status_bad_request;
                return;
            }

            std::uint32_t lobby_id = request_json["lobby_id"].get<std::uint32_t>();
            
            // Check if lobby exists
            auto lobby = m_lobby_manager->get_lobby(lobby_id);
            if (!lobby) {
                json error = {{"success", false}, {"error", "Lobby not found"}};
                res.set_content(error.dump(), "application/json");
                res.status = http::k_status_not_found;
                return;
            }

            // Remove the lobby
            m_lobby_manager->remove_lobby(lobby_id);
            LOG_INFO("Admin command: Stopped lobby ID {} via HTTP", lobby_id);

            json response = {{"success", true}, {"message", "Lobby stopped successfully"}};
            res.set_content(response.dump(), "application/json");
            res.status = http::k_status_ok;

        } catch (const json::exception& e) {
            json error = {{"success", false}, {"error", std::string("JSON error: ") + e.what()}};
            res.set_content(error.dump(), "application/json");
            res.status = http::k_status_bad_request;
        } catch (const std::exception& e) {
            json error = {{"success", false}, {"error", std::string("Server error: ") + e.what()}};
            res.set_content(error.dump(), "application/json");
            res.status = http::k_status_internal_error;
        }
    });

    // Create lobby endpoint (called by Node.js admin API)
    m_server->Post("/admin/lobby/create", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto request_json = json::parse(req.body);
            
            if (!request_json.contains("name") || !request_json.contains("max_players")) {
                json error = {{"success", false}, {"error", "Missing name or max_players"}};
                res.set_content(error.dump(), "application/json");
                res.status = http::k_status_bad_request;
                return;
            }

            std::string name = request_json["name"].get<std::string>();
            std::uint8_t max_players = request_json["max_players"].get<std::uint8_t>();
            
            // Create the lobby
            std::uint32_t lobby_id = m_lobby_manager->create_lobby(name, max_players);
            LOG_INFO("Admin command: Created lobby '{}' with ID {} via HTTP", name, lobby_id);

            json response = {
                {"success", true},
                {"lobby_id", lobby_id},
                {"message", "Lobby created successfully"}
            };
            res.set_content(response.dump(), "application/json");
            res.status = http::k_status_created;

        } catch (const json::exception& e) {
            json error = {{"success", false}, {"error", std::string("JSON error: ") + e.what()}};
            res.set_content(error.dump(), "application/json");
            res.status = http::k_status_bad_request;
        } catch (const std::exception& e) {
            json error = {{"success", false}, {"error", std::string("Server error: ") + e.what()}};
            res.set_content(error.dump(), "application/json");
            res.status = http::k_status_internal_error;
        }
    });
}

void AdminHTTPServer::start() {
    if (m_running) {
        LOG_WARNING("Admin HTTP server already running");
        return;
    }

    m_running = true;
    m_server_thread = std::make_unique<std::thread>(&AdminHTTPServer::run_server, this);
    LOG_INFO("Admin HTTP server starting on port {}", m_port);
}

void AdminHTTPServer::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
    if (m_server) {
        m_server->stop();
    }
    if (m_server_thread && m_server_thread->joinable()) {
        m_server_thread->join();
    }
    LOG_INFO("Admin HTTP server stopped");
}

void AdminHTTPServer::run_server() {
    try {
        LOG_INFO("Admin HTTP server listening on http://localhost:{}", m_port);
        m_server->listen("0.0.0.0", static_cast<int>(m_port));
    } catch (const std::exception& e) {
        LOG_ERROR("Admin HTTP server error: {}", e.what());
        m_running = false;
    }
}
