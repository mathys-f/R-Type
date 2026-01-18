/* Import modules */
import express from 'express';
import adminController from '../controller/adminController.js';
import { verifyAdminToken } from '../middleware/verifyAdminToken.js';

const router = express.Router();

/* Public admin login route */
router.post('/login', adminController.adminLogin);

/* Protected admin routes */
router.get('/lobbies', verifyAdminToken, adminController.getLobbies);
router.get('/players', verifyAdminToken, adminController.getPlayers);
router.post('/kick/:sessionId', verifyAdminToken, adminController.kickPlayer);
router.post('/ban', verifyAdminToken, adminController.banPlayer);
router.get('/banned', verifyAdminToken, adminController.getBannedPlayers);
router.delete('/unban/:banId', verifyAdminToken, adminController.unbanPlayer);
router.post('/lobby/create', verifyAdminToken, adminController.createLobby);
router.post('/lobby/stop/:lobbyId', verifyAdminToken, adminController.stopLobby);
router.get('/stats', verifyAdminToken, adminController.getServerStats);

export default router;
