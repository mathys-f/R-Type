# R-Type Admin Frontend

React + TypeScript admin dashboard for managing R-Type game lobbies and viewing statistics.

## What It Does

- View active game lobbies with player counts
- Create new lobbies with custom names and player limits
- Stop running lobbies
- View leaderboard statistics
- User authentication (login/logout with JWT)

## Installation

### Prerequisites
- Node.js 20+
- npm or yarn

### Steps

1. Install dependencies:
```bash
npm install
```

2. Start development server:
```bash
npm run dev          # Vite dev server with hot reload
npm run build        # Build for production
npm run preview      # Preview production build
```

Frontend runs on **http://localhost:3001**

## Features

- **Dashboard** - Overview of active lobbies, player counts, leaderboard
- **Lobby Management** - Create/stop lobbies with simple form
- **Authentication** - Login/logout with token-based auth
- **Real-time Updates** - Polls lobby list every 3 seconds

## Pages

- `/login` - User login
- `/dashboard` - Main admin panel with lobbies and stats
- Protected routes require valid JWT token

## Authentication

- Login with email/password (gets JWT token)
- Token stored in browser localStorage
- Auto-logout on token expiration (7 days)
- Logout button to clear session

## Backend Communication

Admin frontend communicates with:
- **REST API** (port 8081) - List lobbies, create, stop
- **C++ Server** (port 8082) - Handled by backend, not directly

## Build & Deploy

Development:
```bash
npm run dev
```

Production:
```bash
npm run build
npm run preview

# Or run with Docker:
docker build -t rtype-admin .
docker run -p 3001:3001 rtype-admin
```

## Technology Stack

- React 18 + TypeScript
- Vite 5 (bundler)
- Tailwind CSS (styling)
- Axios (HTTP client)

## Ports

- **3001** - Admin frontend (this app)
- **3000** - Public frontend (separate app)
- **8081** - Backend API
