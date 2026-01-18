# R-Type Public Frontend

React + TypeScript public dashboard for R-Type multiplayer game.

## What It Does

- Browse available game lobbies
- View lobby details (name, player count, max players)
- View leaderboard and player statistics
- User authentication (login/signup with JWT)

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

Frontend runs on **http://localhost:3000**

## Features

- **Lobby Browser** - Real-time list of active lobbies
- **Leaderboard** - Top players with stats
- **Authentication** - Sign up, login, logout
- **User Profile** - View player statistics
- **Real-time Updates** - Refreshes lobby list every 3 seconds

## Pages

- `/` - Home (redirects to login if not authenticated)
- `/login` - User login
- `/signup` - Create new account
- `/dashboard` - Lobby browser and stats
- Protected routes require valid JWT token

## Authentication

- Sign up with email/password
- Login with credentials (gets JWT token)
- Token stored in browser localStorage
- Auto-logout on token expiration (7 days)
- Logout button to clear session

## Backend Communication

Public frontend communicates with:
- **REST API** (port 8081) - Auth, list lobbies, leaderboard

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
docker build -t rtype-public .
docker run -p 3000:3000 rtype-public
```

## Technology Stack

- React 18 + TypeScript
- Vite 5 (bundler)
- Tailwind CSS (styling)
- Axios (HTTP client)

## Ports

- **3000** - Public frontend (this app)
- **3001** - Admin frontend (separate app)
- **8081** - Backend API
