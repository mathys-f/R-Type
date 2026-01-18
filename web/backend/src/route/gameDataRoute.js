/* Import modules */
import express from 'express';
import gameDataController from '../controller/gameDataController.js';

const router = express.Router();

/* Public game data routes (for public dashboard and C++ server) */

// Public endpoints (no auth required)
router.get('/lobbies', gameDataController.getLiveLobbies);
router.get('/lobby/:lobbyId/leaderboard', gameDataController.getLobbyLeaderboard);

// Internal endpoints (called by C++ server - could add token auth later)
router.post('/lobby/create', gameDataController.createLobby);
router.post('/lobby/update-count', gameDataController.updateLobbyPlayerCount);
router.post('/session/add', gameDataController.addPlayerSession);
router.post('/score/update', gameDataController.updatePlayerScore);
router.post('/match/finalize', gameDataController.finalizeMatch);

export default router;
