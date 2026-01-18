/* Import modules */
import {
    AdminAccount,
    BannedPlayer,
    ActiveLobby,
    ActiveSession,
    PlayerScore,
    EventLog,
    Account
} from '../model/indexModel.js';
import bcrypt from 'bcryptjs';
import jwt from 'jsonwebtoken';

/* Admin login */
const adminLogin = async (req, res) => {
    const { username, password } = req.body;

    if (!username || !password) {
        return res.status(400).json({ success: false, error: 'Username and password are required' });
    }

    try {
        const admin = await AdminAccount.findOne({ where: { username } });
        if (!admin || !(await bcrypt.compare(password, admin.password_hash))) {
            return res.status(401).json({ success: false, error: 'Invalid credentials' });
        }

        admin.last_login = new Date();
        await admin.save();

        const token = jwt.sign({ id: admin.id, role: 'admin' }, process.env.JWT_SECRET, { expiresIn: '24h' });

        res.status(200).json({
            success: true,
            message: 'Admin login successful',
            token,
            admin: { id: admin.id, username: admin.username, email: admin.email }
        });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Get all lobbies */
const getLobbies = async (req, res) => {
    try {
        const lobbies = await ActiveLobby.findAll({
            include: [{ model: ActiveSession, attributes: ['id', 'player_name'] }]
        });
        res.status(200).json({ success: true, lobbies });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Get all players in active sessions */
const getPlayers = async (req, res) => {
    try {
        const players = await ActiveSession.findAll({
            include: [{ model: PlayerScore }]
        });
        res.status(200).json({ success: true, players });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Kick a player from lobby */
const kickPlayer = async (req, res) => {
    const { sessionId } = req.params;
    const adminId = req.user.id;

    try {
        const session = await ActiveSession.findByPk(sessionId);
        if (!session) {
            return res.status(404).json({ success: false, error: 'Session not found' });
        }

        // Log event
        await EventLog.create({
            event_type: 'PLAYER_KICKED',
            details: `Kicked player ${session.player_name} from session ${sessionId}`,
            admin_id: adminId
        });

        // Delete session (should trigger delete of player_scores via cascade)
        await session.destroy();

        res.status(200).json({ success: true, message: 'Player kicked successfully' });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Ban a player */
const banPlayer = async (req, res) => {
    const { playerName, reason, accountId } = req.body;
    const adminId = req.user.id;

    try {
        const ban = await BannedPlayer.create({
            player_name: playerName,
            account_id: accountId || null,
            reason,
            banned_by: adminId
        });

        // Log event
        await EventLog.create({
            event_type: 'PLAYER_BANNED',
            details: `Banned ${playerName}: ${reason}`,
            admin_id: adminId
        });

        res.status(201).json({ success: true, message: 'Player banned', ban });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Get banned players */
const getBannedPlayers = async (req, res) => {
    try {
        const banned = await BannedPlayer.findAll();
        res.status(200).json({ success: true, banned });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Unban player */
const unbanPlayer = async (req, res) => {
    const { banId } = req.params;
    const adminId = req.user.id;

    try {
        const ban = await BannedPlayer.findByPk(banId);
        if (!ban) {
            return res.status(404).json({ success: false, error: 'Ban not found' });
        }

        const playerName = ban.player_name;
        await ban.destroy();

        // Log event
        await EventLog.create({
            event_type: 'PLAYER_UNBANNED',
            details: `Unbanned ${playerName}`,
            admin_id: adminId
        });

        res.status(200).json({ success: true, message: 'Player unbanned' });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Stop a lobby */
const stopLobby = async (req, res) => {
    const { lobbyId } = req.params;
    const adminId = req.user.id;

    try {
        const lobby = await ActiveLobby.findByPk(lobbyId);
        if (!lobby) {
            return res.status(404).json({ success: false, error: 'Lobby not found' });
        }

        // First, tell C++ game server to stop the lobby
        try {
            const gameServerUrl = process.env.GAME_SERVER_URL || 'http://localhost:8082';
            const gameServerResponse = await fetch(`${gameServerUrl}/admin/lobby/stop`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ lobby_id: parseInt(lobbyId) })
            });

            if (!gameServerResponse.ok) {
                const errorData = await gameServerResponse.json().catch(() => ({}));
                console.error('Failed to stop lobby on game server:', errorData);
                // Continue anyway to clean up database
            }
        } catch (gameServerError) {
            console.error('Error communicating with game server:', gameServerError.message);
            // Continue anyway to clean up database
        }

        // Delete all sessions in this lobby (cascades to player_scores)
        await ActiveSession.destroy({ where: { lobby_id: lobbyId } });
        await lobby.destroy();

        // Log event
        await EventLog.create({
            event_type: 'LOBBY_STOPPED',
            details: `Stopped lobby ${lobby.name}`,
            admin_id: adminId
        });

        res.status(200).json({ success: true, message: 'Lobby stopped' });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Create a lobby (admin only) */
const createLobby = async (req, res) => {
    const { name, maxPlayers } = req.body;
    const adminId = req.user.id;

    if (!name || !maxPlayers) {
        return res.status(400).json({ success: false, error: 'Name and maxPlayers are required' });
    }

    try {
        // Tell C++ game server to create the lobby
        const gameServerUrl = process.env.GAME_SERVER_URL || 'http://localhost:8082';
        const gameServerResponse = await fetch(`${gameServerUrl}/admin/lobby/create`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name, max_players: parseInt(maxPlayers) })
        });

        if (!gameServerResponse.ok) {
            const errorData = await gameServerResponse.json().catch(() => ({}));
            return res.status(500).json({ 
                success: false, 
                error: `Game server error: ${errorData.error || 'Unknown error'}` 
            });
        }

        const gameServerData = await gameServerResponse.json();
        const cppLobbyId = gameServerData.lobby_id;

        // The C++ server will automatically sync the lobby to our database via existing API
        // Wait a moment for it to appear in the database
        await new Promise(resolve => setTimeout(resolve, 100));

        // Log event
        await EventLog.create({
            event_type: 'LOBBY_CREATED',
            details: `Admin created lobby '${name}' with ${maxPlayers} max players`,
            admin_id: adminId
        });

        res.status(201).json({ 
            success: true, 
            message: 'Lobby created successfully',
            lobby_id: cppLobbyId
        });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Get server stats */
const getServerStats = async (req, res) => {
    try {
        const totalLobbies = await ActiveLobby.count();
        const totalPlayers = await ActiveSession.count();
        const totalBanned = await BannedPlayer.count();
        const totalAccounts = await Account.count();

        res.status(200).json({
            success: true,
            stats: {
                active_lobbies: totalLobbies,
                active_players: totalPlayers,
                banned_count: totalBanned,
                total_accounts: totalAccounts
            }
        });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

export default {
    adminLogin,
    getLobbies,
    getPlayers,
    kickPlayer,
    banPlayer,
    getBannedPlayers,
    unbanPlayer,
    stopLobby,
    createLobby,
    getServerStats
};
