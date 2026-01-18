/* Import modules */
import express from 'express';
import cookieParser from 'cookie-parser';
import corsSetup from './config/cors.js';
import authRouter from './route/authRoute.js';
import adminRouter from './route/adminRoute.js';
import gameDataRouter from './route/gameDataRoute.js';
import notFound from './middleware/notFound.js';

/* App initialization */
const app = express();

/* CORS configuration */
app.use(corsSetup);

/* Body parsers */
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(cookieParser());

/* Health check endpoint */
app.get('/health', (req, res) => {
  res.status(200).json({ success: true, message: 'Backend is running' });
});

/* Define routes */
app.use('/api/auth', authRouter);
app.use('/api/admin', adminRouter);
app.use('/api/game', gameDataRouter);

/* 404 Middleware */
app.use(notFound);

/* Export application */
export default app;
