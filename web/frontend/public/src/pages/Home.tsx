import { useEffect, useMemo, useState } from 'react';
import {
	loginAccount,
	registerAccount,
	getLobbies,
	getLobbyLeaderboard,
	type Lobby,
	type LeaderboardEntry,
} from '../services/auth';

const Home = () => {
	const [authMode, setAuthMode] = useState<'login' | 'register'>('login');
	const [email, setEmail] = useState('');
	const [username, setUsername] = useState('');
	const [password, setPassword] = useState('');
	const [authMessage, setAuthMessage] = useState<string | null>(null);
	const [token, setToken] = useState(() => localStorage.getItem('token') || '');

	const [lobbies, setLobbies] = useState<Lobby[]>([]);
	const [loadingLobbies, setLoadingLobbies] = useState(false);
	const [selectedLobby, setSelectedLobby] = useState<number | null>(null);
	const [leaderboard, setLeaderboard] = useState<LeaderboardEntry[]>([]);
	const [loadingLeaderboard, setLoadingLeaderboard] = useState(false);
	const [error, setError] = useState<string | null>(null);

	const isLoggedIn = useMemo(() => Boolean(token), [token]);

	const handleLogout = () => {
		setToken('');
		localStorage.removeItem('token');
		setAuthMessage('Logged out successfully');
	};

	const handleAuth = async () => {
		try {
			setAuthMessage(null);
			if (authMode === 'register') {
				const { token: newToken, user } = await registerAccount({ email, password, username });
				setToken(newToken);
				localStorage.setItem('token', newToken);
				setAuthMessage(`Welcome ${user.username}, account created.`);
			} else {
				const { token: newToken, user } = await loginAccount({ email, password });
				setToken(newToken);
				localStorage.setItem('token', newToken);
				setAuthMessage(`Welcome back ${user.username}.`);
			}
		} catch (err: any) {
			setAuthMessage(err.message || 'Something went wrong');
		}
	};

	const refreshLobbies = async () => {
		try {
			setLoadingLobbies(true);
			const data = await getLobbies();
			setLobbies(data);
			if (selectedLobby && !data.find((l) => l.id === selectedLobby)) {
				setSelectedLobby(null);
				setLeaderboard([]);
			}
		} catch (err: any) {
			setError(err.message || 'Failed to load lobbies');
		} finally {
			setLoadingLobbies(false);
		}
	};

	const loadLeaderboard = async (lobbyId: number) => {
		try {
			setLoadingLeaderboard(true);
			const data = await getLobbyLeaderboard(lobbyId);
			setLeaderboard(data);
		} catch (err: any) {
			setError(err.message || 'Failed to load leaderboard');
		} finally {
			setLoadingLeaderboard(false);
		}
	};

	useEffect(() => {
		refreshLobbies();
	}, []);

	useEffect(() => {
		if (selectedLobby) {
			loadLeaderboard(selectedLobby);
		}
	}, [selectedLobby]);

	return (
		<main className="min-h-screen bg-slate-950 text-slate-50">
			<div className="mx-auto flex max-w-6xl flex-col gap-8 px-4 py-10 lg:flex-row">
				<section className="flex-1 rounded-xl bg-slate-900/60 p-6 shadow-lg ring-1 ring-slate-800">
					<div className="flex items-center justify-between">
						<h2 className="text-xl font-semibold">{authMode === 'login' ? 'Player Login' : 'Create Account'}</h2>
						<button
							className="text-sm text-indigo-300 underline underline-offset-2"
							onClick={() => setAuthMode(authMode === 'login' ? 'register' : 'login')}
						>
							{authMode === 'login' ? 'Need an account?' : 'Already registered?'}
						</button>
					</div>

					<div className="mt-6 space-y-4">
						<label className="block space-y-2 text-sm">
							<span className="text-slate-200">Email</span>
							<input
								className="w-full rounded-md border border-slate-800 bg-slate-800/60 px-3 py-2 text-slate-50 outline-none focus:border-indigo-400"
								value={email}
								onChange={(e) => setEmail(e.target.value)}
								placeholder="player@example.com"
								type="email"
							/>
						</label>

						{authMode === 'register' && (
							<label className="block space-y-2 text-sm">
								<span className="text-slate-200">Username</span>
								<input
									className="w-full rounded-md border border-slate-800 bg-slate-800/60 px-3 py-2 text-slate-50 outline-none focus:border-indigo-400"
									value={username}
									onChange={(e) => setUsername(e.target.value)}
									placeholder="Pilot42"
								/>
							</label>
						)}

						<label className="block space-y-2 text-sm">
							<span className="text-slate-200">Password</span>
							<input
								className="w-full rounded-md border border-slate-800 bg-slate-800/60 px-3 py-2 text-slate-50 outline-none focus:border-indigo-400"
								value={password}
								onChange={(e) => setPassword(e.target.value)}
								placeholder="••••••••"
								type="password"
							/>
						</label>

						<button
							className="w-full rounded-md bg-indigo-500 px-4 py-2 text-center font-medium text-white shadow hover:bg-indigo-400"
							onClick={handleAuth}
						disabled={isLoggedIn}
					>
						{authMode === 'login' ? 'Login' : 'Register'}
					</button>

					{authMessage && (
						<p className="text-sm text-indigo-200">{authMessage}</p>
					)}
					{isLoggedIn && (
						<div className="space-y-2">
							<p className="text-sm text-emerald-300">✓ Authenticated - Ready to play</p>
							<button
								className="w-full rounded-md border border-slate-700 px-4 py-2 text-sm text-slate-300 hover:bg-slate-800"
								onClick={handleLogout}
							>
								Logout
							</button>
						</div>
					)}
					</div>
				</section>

				<section className="flex-[1.5] space-y-6">
					<div className="rounded-xl bg-slate-900/60 p-6 shadow-lg ring-1 ring-slate-800">
						<div className="mb-4 flex items-center justify-between">
							<h2 className="text-xl font-semibold">Active Lobbies</h2>
							<button
								className="rounded-md border border-slate-700 px-3 py-1 text-sm text-slate-100 hover:border-indigo-400"
								onClick={refreshLobbies}
								disabled={loadingLobbies}
							>
								{loadingLobbies ? 'Refreshing…' : 'Refresh'}
							</button>
						</div>
						{error && <p className="mb-3 text-sm text-rose-300">{error}</p>}
						<div className="grid gap-3 md:grid-cols-2">
							{lobbies.length === 0 && !loadingLobbies && (
								<p className="text-sm text-slate-300">No active lobbies yet.</p>
							)}
							{lobbies.map((lobby) => (
								<button
									key={lobby.id}
									className={`rounded-lg border px-4 py-3 text-left transition hover:border-indigo-400 ${
										selectedLobby === lobby.id ? 'border-indigo-500 bg-indigo-500/10' : 'border-slate-800 bg-slate-900'
									}`}
									onClick={() => setSelectedLobby(lobby.id)}
								>
									<div className="flex items-center justify-between">
										<span className="text-sm font-semibold text-indigo-200">{lobby.name}</span>
										<span className="text-xs text-slate-300">{lobby.player_count}/{lobby.max_players}</span>
									</div>
									<p className="mt-1 text-xs text-slate-400">Players: {lobby.players.join(', ') || 'None'}</p>
									<p className="mt-1 text-[11px] text-slate-500">Lobby ID: {lobby.id}</p>
								</button>
							))}
						</div>
					</div>

					<div className="rounded-xl bg-slate-900/60 p-6 shadow-lg ring-1 ring-slate-800">
						<div className="mb-4 flex items-center justify-between">
							<h2 className="text-xl font-semibold">Leaderboard</h2>
							<span className="text-xs text-slate-400">Click a lobby to view standings.</span>
						</div>
						{selectedLobby === null && <p className="text-sm text-slate-300">No lobby selected.</p>}
						{selectedLobby !== null && (
							<div className="space-y-3">
								{loadingLeaderboard && <p className="text-sm text-slate-300">Loading leaderboard…</p>}
								{!loadingLeaderboard && leaderboard.length === 0 && (
									<p className="text-sm text-slate-300">No scores yet.</p>
								)}
								{!loadingLeaderboard && leaderboard.length > 0 && (
									<div className="space-y-2">
										{leaderboard.map((entry, idx) => (
											<div
												key={entry.playerName + idx}
												className="flex items-center justify-between rounded-lg border border-slate-800 bg-slate-900 px-4 py-2"
											>
												<div>
													<p className="text-sm font-semibold text-indigo-200">{entry.playerName}</p>
													<p className="text-xs text-slate-400">Kills {entry.kills} · Deaths {entry.deaths}</p>
												</div>
												<span className="text-lg font-semibold text-emerald-300">{entry.score}</span>
											</div>
										))}
									</div>
								)}
							</div>
						)}
					</div>
				</section>
			</div>
		</main>
	);
};

export default Home;
