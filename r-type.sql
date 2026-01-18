-- R-Type Game Database Schema
-- Persistent tables (survive server restart): accounts, admin_accounts, banned_players, match_history, event_logs
-- Temporary tables (cleaned on restart): active_sessions, player_scores, active_lobbies

-- ============================================
-- PERMANENT TABLES
-- ============================================

-- Player accounts (optional for gameplay, required for persistent stats)
CREATE TABLE IF NOT EXISTS accounts (
    id SERIAL PRIMARY KEY,
    username VARCHAR(100) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    total_kills INTEGER DEFAULT 0,
    total_deaths INTEGER DEFAULT 0,
    total_score INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    CONSTRAINT unique_username_email UNIQUE(username, email)
);

-- Admin accounts (for admin panel)
CREATE TABLE IF NOT EXISTS admin_accounts (
    id SERIAL PRIMARY KEY,
    username VARCHAR(100) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(50) DEFAULT 'admin',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP
);

-- Banned players (permanent bans)
CREATE TABLE IF NOT EXISTS banned_players (
    id SERIAL PRIMARY KEY,
    player_name VARCHAR(255),
    account_id INTEGER REFERENCES accounts(id) ON DELETE SET NULL,
    reason TEXT,
    banned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    banned_by VARCHAR(255),
    expires_at TIMESTAMP
);

-- Match history (permanent record of all games)
CREATE TABLE IF NOT EXISTS match_history (
    id SERIAL PRIMARY KEY,
    account_id INTEGER REFERENCES accounts(id) ON DELETE SET NULL,
    player_name VARCHAR(255),
    lobby_id INTEGER,
    kills INTEGER DEFAULT 0,
    deaths INTEGER DEFAULT 0,
    score INTEGER DEFAULT 0,
    match_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Event logs (audit trail for admin actions)
CREATE TABLE IF NOT EXISTS event_logs (
    id SERIAL PRIMARY KEY,
    event_type VARCHAR(100) NOT NULL,
    details TEXT,
    admin_id INTEGER REFERENCES admin_accounts(id) ON DELETE SET NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Server configuration
CREATE TABLE IF NOT EXISTS server_config (
    id SERIAL PRIMARY KEY,
    config_key VARCHAR(255) UNIQUE NOT NULL,
    config_value TEXT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ============================================
-- TEMPORARY TABLES (cleaned on server restart)
-- ============================================

-- Active lobbies (current games)
CREATE TABLE IF NOT EXISTS active_lobbies (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    port INTEGER,
    max_players INTEGER DEFAULT 4,
    player_count INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Active sessions (connected clients)
CREATE TABLE IF NOT EXISTS active_sessions (
    id SERIAL PRIMARY KEY,
    account_id INTEGER REFERENCES accounts(id) ON DELETE SET NULL,
    player_name VARCHAR(255) NOT NULL,
    lobby_id INTEGER REFERENCES active_lobbies(id) ON DELETE CASCADE,
    session_token VARCHAR(255) UNIQUE,
    ip_address VARCHAR(45),
    connected_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Player scores (real-time game stats)
CREATE TABLE IF NOT EXISTS player_scores (
    id SERIAL PRIMARY KEY,
    session_id INTEGER REFERENCES active_sessions(id) ON DELETE CASCADE,
    lobby_id INTEGER REFERENCES active_lobbies(id) ON DELETE CASCADE,
    kills INTEGER DEFAULT 0,
    deaths INTEGER DEFAULT 0,
    score INTEGER DEFAULT 0,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ============================================
-- INDEXES FOR PERFORMANCE
-- ============================================

CREATE INDEX idx_accounts_email ON accounts(email);
CREATE INDEX idx_accounts_username ON accounts(username);
CREATE INDEX idx_admin_accounts_username ON admin_accounts(username);
CREATE INDEX idx_banned_players_account ON banned_players(account_id);
CREATE INDEX idx_banned_players_name ON banned_players(player_name);
CREATE INDEX idx_match_history_account ON match_history(account_id);
CREATE INDEX idx_match_history_date ON match_history(match_date);
CREATE INDEX idx_event_logs_created_at ON event_logs(timestamp);
CREATE INDEX idx_event_logs_admin ON event_logs(admin_id);
CREATE INDEX idx_active_sessions_lobby ON active_sessions(lobby_id);
CREATE INDEX idx_active_sessions_account ON active_sessions(account_id);
CREATE INDEX idx_player_scores_session ON player_scores(session_id);
CREATE INDEX idx_player_scores_lobby ON player_scores(lobby_id);

-- ============================================
-- DEFAULT DATA
-- ============================================

INSERT INTO admin_accounts (username, email, password_hash, role) VALUES
    ('admin', 'admin@rtype.local', '$2b$10$LtBeDP96qiZU/6nSoqeisu67m00KbaIMzncQOmmI6bnMcXegklVoG', 'admin')
ON CONFLICT (username) DO NOTHING;

-- ============================================
-- DEMO DATA FOR UI PREVIEW
-- ============================================

-- -- Player accounts (password for all: admin123)
-- INSERT INTO accounts (id, username, email, password_hash, total_kills, total_deaths, total_score, last_login) VALUES
--     (1, 'pilot_ace', 'ace@rtype.local', '$2b$10$LtBeDP96qiZU/6nSoqeisu67m00KbaIMzncQOmmI6bnMcXegklVoG', 42, 18, 9000, CURRENT_TIMESTAMP - INTERVAL '2 days'),
--     (2, 'laserfox', 'laser@rtype.local', '$2b$10$LtBeDP96qiZU/6nSoqeisu67m00KbaIMzncQOmmI6bnMcXegklVoG', 55, 30, 12500, CURRENT_TIMESTAMP - INTERVAL '1 days'),
--     (3, 'nebula', 'nebula@rtype.local', '$2b$10$LtBeDP96qiZU/6nSoqeisu67m00KbaIMzncQOmmI6bnMcXegklVoG', 12, 5, 3100, CURRENT_TIMESTAMP - INTERVAL '6 hours')
-- ON CONFLICT (id) DO NOTHING;

-- -- Active lobbies
-- INSERT INTO active_lobbies (id, name, port, max_players, player_count, created_at) VALUES
--     (1, 'Orion Run', 5001, 4, 3, CURRENT_TIMESTAMP - INTERVAL '25 minutes'),
--     (2, 'Nebula Clash', 5002, 6, 2, CURRENT_TIMESTAMP - INTERVAL '10 minutes')
-- ON CONFLICT (id) DO NOTHING;

-- -- Active sessions (in-lobby players)
-- INSERT INTO active_sessions (id, account_id, player_name, lobby_id, session_token, ip_address, connected_at) VALUES
--     (1, 1, 'pilot_ace', 1, 'session_demo_1', '10.0.0.11', CURRENT_TIMESTAMP - INTERVAL '20 minutes'),
--     (2, 2, 'laserfox', 1, 'session_demo_2', '10.0.0.22', CURRENT_TIMESTAMP - INTERVAL '15 minutes'),
--     (3, 3, 'nebula', 1, 'session_demo_3', '10.0.0.33', CURRENT_TIMESTAMP - INTERVAL '12 minutes'),
--     (4, NULL, 'guest_rogue', 2, 'session_demo_4', '10.0.0.44', CURRENT_TIMESTAMP - INTERVAL '8 minutes'),
--     (5, 2, 'laserfox', 2, 'session_demo_5', '10.0.0.22', CURRENT_TIMESTAMP - INTERVAL '5 minutes')
-- ON CONFLICT (id) DO NOTHING;

-- -- Player scores (live stats)
-- INSERT INTO player_scores (id, session_id, lobby_id, kills, deaths, score, updated_at) VALUES
--     (1, 1, 1, 12, 4, 3200, CURRENT_TIMESTAMP - INTERVAL '2 minutes'),
--     (2, 2, 1, 18, 9, 4100, CURRENT_TIMESTAMP - INTERVAL '3 minutes'),
--     (3, 3, 1, 7, 2, 2100, CURRENT_TIMESTAMP - INTERVAL '1 minutes'),
--     (4, 4, 2, 5, 1, 1500, CURRENT_TIMESTAMP - INTERVAL '1 minutes'),
--     (5, 5, 2, 9, 3, 2800, CURRENT_TIMESTAMP - INTERVAL '30 seconds')
-- ON CONFLICT (id) DO NOTHING;

-- -- Banned players list
-- INSERT INTO banned_players (id, player_name, account_id, reason, banned_at, banned_by) VALUES
--     (1, 'toxik', NULL, 'Griefing teammates', CURRENT_TIMESTAMP - INTERVAL '1 day', 'admin'),
--     (2, 'laserfox', 2, 'Repeated disconnects', CURRENT_TIMESTAMP - INTERVAL '6 hours', 'admin')
-- ON CONFLICT (id) DO NOTHING;

-- -- Match history examples
-- INSERT INTO match_history (id, account_id, player_name, lobby_id, kills, deaths, score, match_date) VALUES
--     (1, 1, 'pilot_ace', 101, 25, 12, 5200, CURRENT_TIMESTAMP - INTERVAL '3 days'),
--     (2, 2, 'laserfox', 101, 32, 14, 6400, CURRENT_TIMESTAMP - INTERVAL '3 days'),
--     (3, 3, 'nebula', 101, 8, 6, 1800, CURRENT_TIMESTAMP - INTERVAL '3 days'),
--     (4, 1, 'pilot_ace', 102, 17, 9, 3900, CURRENT_TIMESTAMP - INTERVAL '18 hours'),
--     (5, 2, 'laserfox', 102, 22, 15, 4100, CURRENT_TIMESTAMP - INTERVAL '18 hours')
-- ON CONFLICT (id) DO NOTHING;
