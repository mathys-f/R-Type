/* Import modules */
import cors from 'cors';

/* Allowed origins resolved from env (comma-separated) with sane defaults for docker/dev */
const allowedOrigins = (process.env.CORS_ORIGINS || 'http://localhost:3000,http://localhost:3001,http://localhost:5173,http://localhost:5174,http://localhost:8081')
    .split(',')
    .map((o) => o.trim())
    .filter(Boolean);

/* CORS middleware */
const corsSetup = cors({
    origin(origin, callback) {
        if (!origin || allowedOrigins.includes(origin)) {
            return callback(null, true);
        }
        return callback(new Error('Not allowed by CORS'));
    },
    methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
    credentials: true,
    allowedHeaders: ['Origin', 'X-Requested-With', 'Content-Type', 'Accept', 'Authorization'],
});

/* Export CORS middleware */
export default corsSetup;
