# R-Type Backend

Node.js/Express REST API server for R-Type multiplayer game management.

## What It Does

- Manages lobby creation, listing, and deletion
- Handles user authentication (login/signup with JWT)
- Tracks active lobbies and player counts
- Communicates with the C++ game server for admin commands
- Stores data in PostgreSQL database

## Installation

### Prerequisites
- Node.js 20+
- PostgreSQL 15+
- Docker (optional, for containerized setup)

### Steps

1. Install dependencies:
```bash
npm install
```

2. Configure environment (see `.env` section below)

3. Start the server:
```bash
npm start        # Production
npm run dev      # Development with auto-reload
```

Server runs on **http://localhost:8081**

## Environment Variables

Create `.env` file in this directory:

```env
# Database
POSTGRES_URL=d_burl

# JWT
JWT_SECRET=your_secret_key_here

# CORS
CORS_ORIGIN=urls

# C++ Game Server (for admin commands)
GAME_SERVER_URL=ip
```

## API Routes

### Authentication
- `POST /api/auth/signup` - Create new user
- `POST /api/auth/login` - Login (returns JWT token)
- `POST /api/auth/logout` - Logout

### Lobbies
- `GET /api/game/lobby/list` - List all active lobbies
- `POST /api/admin/lobby/create` - Create new lobby (admin only)
- `POST /api/admin/lobby/stop` - Stop a lobby (admin only)
- `POST /api/game/lobby/update-count` - Update player count (called by C++ server)

### Leaderboard
- `GET /api/leaderboard` - Get top players

## Database

PostgreSQL with 9 tables:
- `users` - Player accounts
- `active_lobbies` - Current game lobbies
- `active_sessions` - Player sessions in lobbies
- `event_logs` - Admin actions
- `leaderboard` - Player statistics
- And more...

Tables auto-created on first run if migrations are configured.

## Docker

```bash
docker-compose up -d      # Start backend + database
docker-compose down       # Stop services
docker-compose logs -f    # View logs
```

## Ports

- **8081** - REST API server
- **5432** - PostgreSQL database (only exposed if needed)

## Notes

- JWT tokens expire after 7 days
- Admin operations (create/stop lobbies) require authentication
- C++ game server communicates via port 8082
