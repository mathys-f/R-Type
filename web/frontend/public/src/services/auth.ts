export type Lobby = {
	id: number;
	name: string;
	player_count: number;
	max_players: number;
	players: string[];
	created_at: string;
};

export type LeaderboardEntry = {
	playerName: string;
	kills: number;
	deaths: number;
	score: number;
};

export type GlobalLeaderboardEntry = {
	playerName: string;
	matchesPlayed: number;
	kills: number;
	deaths: number;
	score: number;
};

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8081';

const baseHeaders = { 'Content-Type': 'application/json' } as const;

async function request<T>(path: string, options: RequestInit = {}): Promise<T> {
	const response = await fetch(`${API_URL}${path}`, {
		credentials: 'include',
		...options,
		headers: { ...baseHeaders, ...(options.headers || {}) },
	});

	const data = await response.json().catch(() => ({}));
	if (!response.ok) {
		const message = (data as any)?.error || (data as any)?.message || 'Request failed';
		throw new Error(message);
	}
	return data as T;
}

export async function getLobbies(): Promise<Lobby[]> {
	const res = await request<{ lobbies: Lobby[] }>('/api/game/lobbies');
	return res.lobbies || [];
}

export async function getLobbyLeaderboard(lobbyId: number): Promise<LeaderboardEntry[]> {
	const res = await request<{ leaderboard: LeaderboardEntry[] }>(`/api/game/lobby/${lobbyId}/leaderboard`);
	return res.leaderboard || [];
}

export async function getGlobalLeaderboard(limit = 20): Promise<GlobalLeaderboardEntry[]> {
	const res = await request<{ leaderboard: GlobalLeaderboardEntry[] }>(
		`/api/game/leaderboard/global?limit=${limit}`,
	);
	return res.leaderboard || [];
}

export { API_URL };
