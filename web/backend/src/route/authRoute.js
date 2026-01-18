/* Import modules */
import express from 'express';
import authController from '../controller/authController.js';

/* Initialize router */
const router = express.Router();

/* Define routes */
router.post('/register', authController.register);
router.post('/login', authController.login);
router.post('/logout', authController.logout);
router.get('/isConnected', authController.isConnected);

/* Export router */
export default router;
