/* Import modules */
import {
    ActiveLobby,
    ActiveSession,
    PlayerScore,
    MatchHistory,
    Account
} from '../model/indexModel.js';

/* Create a new lobby */
const createLobby = async (req, res) => {
    const { name, port, max_players } = req.body;

    if (!name) {
        return res.status(400).json({ success: false, error: 'Lobby name is required' });
    }

    try {
        const lobby = await ActiveLobby.create({
            name,
            port: port || null,
            max_players: max_players || 4,
            player_count: 0
        });

        res.status(201).json({ success: true, message: 'Lobby created', lobby });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Add player session to lobby */
const addPlayerSession = async (req, res) => {
    const { lobbyId, playerName, accountId, ipAddress } = req.body;

    if (!lobbyId || !playerName) {
        return res.status(400).json({ success: false, error: 'Lobby ID and player name required' });
    }

    try {
        const lobby = await ActiveLobby.findByPk(lobbyId);
        if (!lobby) {
            return res.status(404).json({ success: false, error: 'Lobby not found' });
        }

        // Generate session token
        const sessionToken = `session_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;

        const session = await ActiveSession.create({
            lobby_id: lobbyId,
            player_name: playerName,
            account_id: accountId || null,
            session_token: sessionToken,
            ip_address: ipAddress || null
        });

        // Create player score entry
        await PlayerScore.create({
            session_id: session.id,
            lobby_id: lobbyId,
            kills: 0,
            deaths: 0,
            score: 0
        });

        // Increment lobby player count
        lobby.player_count += 1;
        await lobby.save();

        res.status(201).json({
            success: true,
            message: 'Player session created',
            session: {
                id: session.id,
                sessionToken,
                playerName: session.player_name
            }
        });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Update player score in real-time */
const updatePlayerScore = async (req, res) => {
    const { sessionId, kills, deaths, score } = req.body;

    if (!sessionId) {
        return res.status(400).json({ success: false, error: 'Session ID required' });
    }

    try {
        const playerScore = await PlayerScore.findOne({ where: { session_id: sessionId } });
        if (!playerScore) {
            return res.status(404).json({ success: false, error: 'Player score not found' });
        }

        // Update individual fields or use totals
        if (kills !== undefined) playerScore.kills = kills;
        if (deaths !== undefined) playerScore.deaths = deaths;
        if (score !== undefined) playerScore.score = score;
        playerScore.updated_at = new Date();

        await playerScore.save();

        res.status(200).json({ success: true, message: 'Score updated', playerScore });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Get live lobby data (for public dashboard) */
const getLiveLobbies = async (req, res) => {
    try {
        const lobbies = await ActiveLobby.findAll({
            include: [{
                model: ActiveSession,
                attributes: ['id', 'player_name', 'account_id']
            }]
        });

        // Format response for public display
        const formattedLobbies = lobbies.map(lobby => ({
            id: lobby.id,
            name: lobby.name,
            player_count: lobby.player_count,
            max_players: lobby.max_players,
            created_at: lobby.created_at,
            players: lobby.ActiveSessions.map(s => s.player_name)
        }));

        res.status(200).json({ success: true, lobbies: formattedLobbies });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Get live leaderboard for a lobby */
const getLobbyLeaderboard = async (req, res) => {
    const { lobbyId } = req.params;

    try {
        const players = await PlayerScore.findAll({
            where: { lobby_id: lobbyId },
            include: [{ model: ActiveSession }],
            order: [['score', 'DESC']]
        });

        const leaderboard = players.map(p => ({
            playerName: p.ActiveSession.player_name,
            kills: p.kills,
            deaths: p.deaths,
            score: p.score
        }));

        res.status(200).json({ success: true, leaderboard });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Finalize match and save to history */
const finalizeMatch = async (req, res) => {
    const { lobbyId } = req.body;

    if (!lobbyId) {
        return res.status(400).json({ success: false, error: 'Lobby ID required' });
    }

    try {
        // Get all player scores for this lobby
        const playerScores = await PlayerScore.findAll({
            where: { lobby_id: lobbyId },
            include: [{ model: ActiveSession }]
        });

        // Save each to match_history and update account stats
        for (const ps of playerScores) {
            const accountId = ps.ActiveSession.account_id;

            // Save to match history
            await MatchHistory.create({
                account_id: accountId,
                player_name: ps.ActiveSession.player_name,
                lobby_id: lobbyId,
                kills: ps.kills,
                deaths: ps.deaths,
                score: ps.score
            });

            // Update account total stats (if account exists)
            if (accountId) {
                const account = await Account.findByPk(accountId);
                if (account) {
                    account.total_kills += ps.kills;
                    account.total_deaths += ps.deaths;
                    account.total_score += ps.score;
                    await account.save();
                }
            }
        }

        // Delete player scores for this lobby (cleanup)
        await PlayerScore.destroy({ where: { lobby_id: lobbyId } });

        // Delete active sessions for this lobby
        await ActiveSession.destroy({ where: { lobby_id: lobbyId } });

        // Delete lobby
        await ActiveLobby.destroy({ where: { id: lobbyId } });

        res.status(200).json({ success: true, message: 'Match finalized and archived' });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

/* Update lobby player count */
const updateLobbyPlayerCount = async (req, res) => {
    const { lobbyId, playerCount } = req.body;

    if (!lobbyId || playerCount === undefined) {
        return res.status(400).json({ success: false, error: 'Lobby ID and player count required' });
    }

    try {
        const lobby = await ActiveLobby.findByPk(lobbyId);
        if (!lobby) {
            return res.status(404).json({ success: false, error: 'Lobby not found' });
        }

        lobby.player_count = playerCount;
        await lobby.save();

        res.status(200).json({ success: true, message: 'Player count updated' });
    } catch (error) {
        res.status(500).json({ success: false, error: error.message });
    }
};

export default {
    createLobby,
    addPlayerSession,
    updatePlayerScore,
    updateLobbyPlayerCount,
    getLiveLobbies,
    getLobbyLeaderboard,
    finalizeMatch
};
