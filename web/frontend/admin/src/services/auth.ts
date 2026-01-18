export type AdminStats = {
	active_lobbies: number;
	active_players: number;
	banned_count: number;
	total_accounts: number;
};

export type AdminLobby = {
	id: number;
	name: string;
	player_count: number;
	max_players: number;
	ActiveSessions?: { id: number; player_name: string }[];
};

export type AdminPlayer = {
	id: number;
	player_name: string;
	lobby_id: number;
	PlayerScores?: { kills: number; deaths: number; score: number }[];
};

export type BannedPlayer = {
	id: number;
	player_name: string;
	reason: string;
	banned_at: string;
};

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8081';
const baseHeaders = { 'Content-Type': 'application/json' } as const;

async function request<T>(path: string, token: string | null, options: RequestInit = {}): Promise<T> {
	const response = await fetch(`${API_URL}${path}`, {
		credentials: 'include',
		...options,
		headers: {
			...baseHeaders,
			...(token ? { Authorization: `Bearer ${token}` } : {}),
			...(options.headers || {}),
		},
	});

	const data = await response.json().catch(() => ({}));
	if (!response.ok) {
		const message = (data as any)?.error || (data as any)?.message || 'Request failed';
		throw new Error(message);
	}
	return data as T;
}

export async function loginAdmin(payload: { username: string; password: string }) {
	return request<{ token: string; admin: { username: string } }>('/api/admin/login', null, {
		method: 'POST',
		body: JSON.stringify(payload),
	});
}

export async function fetchStats(token: string): Promise<AdminStats> {
	const res = await request<{ stats: AdminStats }>('/api/admin/stats', token);
	return res.stats;
}

export async function fetchLobbies(token: string): Promise<AdminLobby[]> {
	const res = await request<{ lobbies: AdminLobby[] }>('/api/admin/lobbies', token);
	return res.lobbies || [];
}

export async function fetchPlayers(token: string): Promise<AdminPlayer[]> {
	const res = await request<{ players: AdminPlayer[] }>('/api/admin/players', token);
	return res.players || [];
}

export async function fetchBanned(token: string): Promise<BannedPlayer[]> {
	const res = await request<{ banned: BannedPlayer[] }>('/api/admin/banned', token);
	return res.banned || [];
}

export async function kickPlayer(token: string, sessionId: number) {
	return request('/api/admin/kick/' + sessionId, token, { method: 'POST' });
}

export async function stopLobby(token: string, lobbyId: number) {
	return request('/api/admin/lobby/stop/' + lobbyId, token, { method: 'POST' });
}

export async function createLobby(token: string, payload: { name: string; maxPlayers: number }) {
	return request<{ lobby_id: number }>('/api/admin/lobby/create', token, {
		method: 'POST',
		body: JSON.stringify(payload),
	});
}

export async function banPlayer(token: string, payload: { playerName: string; reason?: string; accountId?: number }) {
	return request('/api/admin/ban', token, {
		method: 'POST',
		body: JSON.stringify(payload),
	});
}

export async function unbanPlayer(token: string, banId: number) {
	return request('/api/admin/unban/' + banId, token, { method: 'DELETE' });
}

export { API_URL };
