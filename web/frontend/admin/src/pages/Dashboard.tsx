import { useEffect, useState } from 'react';
import {
	loginAdmin,
	fetchStats,
	fetchLobbies,
	fetchPlayers,
	fetchBanned,
	kickPlayer,
	stopLobby,
	createLobby,
	banPlayer,
	unbanPlayer,
	type AdminStats,
	type AdminLobby,
	type AdminPlayer,
	type BannedPlayer,
} from '../services/auth';

const Dashboard = () => {
	const [username, setUsername] = useState('');
	const [password, setPassword] = useState('');
	const [token, setToken] = useState<string | null>(() => localStorage.getItem('adminToken'));
	const [message, setMessage] = useState<string | null>(null);
	const [loadingAuth, setLoadingAuth] = useState(false);

	const [stats, setStats] = useState<AdminStats | null>(null);
	const [lobbies, setLobbies] = useState<AdminLobby[]>([]);
	const [players, setPlayers] = useState<AdminPlayer[]>([]);
	const [banned, setBanned] = useState<BannedPlayer[]>([]);
	const [loadingData, setLoadingData] = useState(false);
	const [error, setError] = useState<string | null>(null);

	const [banName, setBanName] = useState('');
	const [banReason, setBanReason] = useState('');

	const [lobbyName, setLobbyName] = useState('');
	const [lobbyMaxPlayers, setLobbyMaxPlayers] = useState(4);

	const handleLogin = async () => {
		try {
			setLoadingAuth(true);
			setMessage(null);
			const res = await loginAdmin({ username, password });
			setToken(res.token);
			localStorage.setItem('adminToken', res.token);
			setMessage(`Hello ${res.admin.username}`);
		} catch (err: any) {
			setMessage(err.message || 'Login failed');
		} finally {
			setLoadingAuth(false);
		}
	};

	const handleLogout = () => {
		setToken(null);
		localStorage.removeItem('adminToken');
		setMessage(null);
		setStats(null);
		setLobbies([]);
		setPlayers([]);
		setBanned([]);
	};

	const loadData = async () => {
		if (!token) return;
		setError(null);
		try {
			setLoadingData(true);
			const [statsRes, lobbiesRes, playersRes, bannedRes] = await Promise.all([
				fetchStats(token),
				fetchLobbies(token),
				fetchPlayers(token),
				fetchBanned(token),
			]);
			setStats(statsRes);
			setLobbies(lobbiesRes);
			setPlayers(playersRes);
			setBanned(bannedRes);
		} catch (err: any) {
			if (err.message?.includes('expired') || err.message?.includes('invalid')) {
				handleLogout();
				setError('Session expired. Please log in again.');
			} else {
				setError(err.message || 'Failed to load data');
			}
		} finally {
			setLoadingData(false);
		}
	};

	useEffect(() => {
		if (token) {
			loadData();
		}
	}, [token]);

	const handleKick = async (sessionId: number) => {
		if (!token) return;
		try {
			await kickPlayer(token, sessionId);
			loadData();
		} catch (err: any) {
			setError(err.message || 'Kick failed');
		}
	};

	const handleStopLobby = async (lobbyId: number) => {
		if (!token) return;
		try {
			await stopLobby(token, lobbyId);
			loadData();
		} catch (err: any) {
			setError(err.message || 'Stop lobby failed');
		}
	};

	const handleBan = async () => {
		if (!token || !banName) return;
		try {
			await banPlayer(token, { playerName: banName, reason: banReason });
			setBanName('');
			setBanReason('');
			loadData();
		} catch (err: any) {
			setError(err.message || 'Ban failed');
		}
	};

	const handleUnban = async (banId: number) => {
		if (!token) return;
		try {
			await unbanPlayer(token, banId);
			loadData();
		} catch (err: any) {
			setError(err.message || 'Unban failed');
		}
	};

	const handleCreateLobby = async () => {
		if (!token || !lobbyName) return;
		try {
			await createLobby(token, { name: lobbyName, maxPlayers: lobbyMaxPlayers });
			setLobbyName('');
			setLobbyMaxPlayers(4);
			loadData();
		} catch (err: any) {
			setError(err.message || 'Create lobby failed');
		}
	};

	if (!token) {
		return (
			<main className="min-h-screen bg-slate-950 text-slate-50">
				<div className="mx-auto flex max-w-md flex-col gap-6 px-4 py-14">
					<h1 className="text-2xl font-semibold">Admin Login</h1>
					<label className="space-y-2 text-sm">
						<span>Username</span>
						<input
							className="w-full rounded-md border border-slate-800 bg-slate-900 px-3 py-2 text-slate-50 focus:border-indigo-400"
							value={username}
							onChange={(e) => setUsername(e.target.value)}
							placeholder="admin"
						/>
					</label>
					<label className="space-y-2 text-sm">
						<span>Password</span>
						<input
							className="w-full rounded-md border border-slate-800 bg-slate-900 px-3 py-2 text-slate-50 focus:border-indigo-400"
							type="password"
							value={password}
							onChange={(e) => setPassword(e.target.value)}
							placeholder="••••••••"
						/>
					</label>
					<button
						className="rounded-md bg-indigo-500 px-4 py-2 font-medium text-white hover:bg-indigo-400"
						onClick={handleLogin}
						disabled={loadingAuth}
					>
						{loadingAuth ? 'Signing in…' : 'Sign in'}
					</button>
					{message && <p className="text-sm text-indigo-200">{message}</p>}
				</div>
			</main>
		);
	}

	return (
		<main className="min-h-screen bg-slate-950 text-slate-50">
			<div className="mx-auto max-w-6xl px-4 py-10 space-y-8">
				<div className="flex items-center justify-between">
					<div>
						<h1 className="text-2xl font-semibold">Live Overview</h1>
						<p className="text-sm text-slate-400">Monitor lobbies, sessions, and bans.</p>
					</div>
					<div className="flex gap-2">
						<button
							className="rounded-md border border-slate-800 px-3 py-1 text-sm text-slate-100 hover:border-indigo-400"
							onClick={loadData}
							disabled={loadingData}
						>
							{loadingData ? 'Refreshing…' : 'Refresh'}
						</button>
						<button
							className="rounded-md border border-rose-800 px-3 py-1 text-sm text-rose-300 hover:bg-rose-900/20"
							onClick={handleLogout}
						>
							Logout
						</button>
					</div>
				</div>
				{error && <p className="text-sm text-rose-300">{error}</p>}

				<section className="grid gap-4 sm:grid-cols-2 lg:grid-cols-4">
					{stats && (
						<>
							<StatCard label="Active Lobbies" value={stats.active_lobbies} />
							<StatCard label="Active Players" value={stats.active_players} />
							<StatCard label="Banned Players" value={stats.banned_count} />
							<StatCard label="Total Accounts" value={stats.total_accounts} />
						</>
					)}
				</section>

				<section className="grid gap-6 lg:grid-cols-2">
					<div className="rounded-xl bg-slate-900/60 p-5 shadow-lg ring-1 ring-slate-800">
						<div className="mb-4 flex items-center justify-between">
							<h2 className="text-lg font-semibold">Lobbies</h2>
							<span className="text-xs text-slate-400">Create or stop lobbies.</span>
						</div>

						{/* Create Lobby Form */}
						<div className="mb-4 rounded-lg border border-slate-800 bg-slate-900 p-4 space-y-3">
							<p className="text-sm font-semibold text-indigo-200">Create New Lobby</p>
							<div className="space-y-2">
								<input
									className="w-full rounded-md border border-slate-800 bg-slate-950 px-3 py-2 text-sm text-slate-50"
									placeholder="Lobby name"
									value={lobbyName}
									onChange={(e) => setLobbyName(e.target.value)}
								/>
								<div className="flex items-center gap-2">
									<label className="text-xs text-slate-400">Max players:</label>
									<input
										type="number"
										min="2"
										max="8"
										className="w-20 rounded-md border border-slate-800 bg-slate-950 px-3 py-1 text-sm text-slate-50"
										value={lobbyMaxPlayers}
										onChange={(e) => setLobbyMaxPlayers(parseInt(e.target.value) || 4)}
									/>
								</div>
								<button
									className="w-full rounded-md bg-indigo-500 px-3 py-2 text-sm font-medium text-white hover:bg-indigo-400"
									onClick={handleCreateLobby}
									disabled={!lobbyName}
								>
									Create Lobby
								</button>
							</div>
						</div>

						<div className="space-y-3">
							{lobbies.length === 0 && <p className="text-sm text-slate-300">No active lobbies.</p>}
							{lobbies.map((lobby) => (
								<div key={lobby.id} className="rounded-lg border border-slate-800 bg-slate-900 px-4 py-3">
									<div className="flex items-center justify-between">
										<div>
											<p className="text-sm font-semibold text-indigo-200">{lobby.name}</p>
											<p className="text-xs text-slate-400">Players {lobby.player_count}/{lobby.max_players}</p>
										</div>
										<button
											className="rounded-md border border-rose-400 px-3 py-1 text-xs text-rose-200 hover:bg-rose-500/10"
											onClick={() => handleStopLobby(lobby.id)}
										>
											Stop
										</button>
									</div>
									<p className="mt-1 text-xs text-slate-500">ID: {lobby.id}</p>
									{lobby.ActiveSessions && lobby.ActiveSessions.length > 0 && (
										<p className="mt-1 text-xs text-slate-300">Players: {lobby.ActiveSessions.map((s) => s.player_name).join(', ')}</p>
									)}
								</div>
							))}
						</div>
					</div>

					<div className="rounded-xl bg-slate-900/60 p-5 shadow-lg ring-1 ring-slate-800">
						<div className="mb-4 flex items-center justify-between">
							<h2 className="text-lg font-semibold">Sessions</h2>
							<span className="text-xs text-slate-400">Kick suspicious players.</span>
						</div>
						<div className="space-y-3">
							{players.length === 0 && <p className="text-sm text-slate-300">No active players.</p>}
							{players.map((player) => (
								<div key={player.id} className="flex items-center justify-between rounded-lg border border-slate-800 bg-slate-900 px-4 py-3">
									<div>
										<p className="text-sm font-semibold text-indigo-200">{player.player_name}</p>
										<p className="text-xs text-slate-400">Session #{player.id} · Lobby {player.lobby_id}</p>
										{player.PlayerScores && player.PlayerScores[0] && (
											<p className="text-xs text-slate-500">K {player.PlayerScores[0].kills} / D {player.PlayerScores[0].deaths} / S {player.PlayerScores[0].score}</p>
										)}
									</div>
									<button
										className="rounded-md border border-amber-400 px-3 py-1 text-xs text-amber-100 hover:bg-amber-500/10"
										onClick={() => handleKick(player.id)}
									>
										Kick
									</button>
								</div>
							))}
						</div>
					</div>
				</section>

				<section className="grid gap-6 lg:grid-cols-2">
					<div className="rounded-xl bg-slate-900/60 p-5 shadow-lg ring-1 ring-slate-800">
						<div className="mb-4 flex items-center justify-between">
							<h2 className="text-lg font-semibold">Banned Players</h2>
							<span className="text-xs text-slate-400">Unban if needed.</span>
						</div>
						<div className="space-y-3">
							{banned.length === 0 && <p className="text-sm text-slate-300">No bans recorded.</p>}
							{banned.map((entry) => (
								<div key={entry.id} className="flex items-center justify-between rounded-lg border border-slate-800 bg-slate-900 px-4 py-3">
									<div>
										<p className="text-sm font-semibold text-indigo-200">{entry.player_name}</p>
										<p className="text-xs text-slate-400">{entry.reason || 'No reason'}</p>
										<p className="text-[11px] text-slate-500">Banned at {new Date(entry.banned_at).toLocaleString()}</p>
									</div>
									<button
										className="rounded-md border border-emerald-400 px-3 py-1 text-xs text-emerald-100 hover:bg-emerald-500/10"
										onClick={() => handleUnban(entry.id)}
									>
										Unban
									</button>
								</div>
							))}
						</div>
					</div>

					<div className="rounded-xl bg-slate-900/60 p-5 shadow-lg ring-1 ring-slate-800">
						<div className="mb-4 flex items-center justify-between">
							<h2 className="text-lg font-semibold">Quick Ban</h2>
							<span className="text-xs text-slate-400">Player name is required.</span>
						</div>
						<div className="space-y-3">
							<label className="space-y-1 text-sm">
								<span className="text-slate-200">Player name</span>
								<input
									className="w-full rounded-md border border-slate-800 bg-slate-900 px-3 py-2 text-slate-50 focus:border-indigo-400"
									value={banName}
									onChange={(e) => setBanName(e.target.value)}
									placeholder="GradiusFan"
								/>
							</label>
							<label className="space-y-1 text-sm">
								<span className="text-slate-200">Reason</span>
								<textarea
									className="w-full rounded-md border border-slate-800 bg-slate-900 px-3 py-2 text-sm text-slate-50 focus:border-indigo-400"
									rows={3}
									value={banReason}
									onChange={(e) => setBanReason(e.target.value)}
									placeholder="Griefing, cheating, spam..."
								/>
							</label>
							<button
								className="w-full rounded-md bg-rose-500 px-4 py-2 font-medium text-white hover:bg-rose-400"
								onClick={handleBan}
								disabled={!banName}
							>
								Ban player
							</button>
						</div>
					</div>
				</section>
			</div>
		</main>
	);
};

const StatCard = ({ label, value }: { label: string; value: number }) => (
	<div className="rounded-xl bg-slate-900/60 p-4 shadow ring-1 ring-slate-800">
		<p className="text-xs uppercase tracking-wide text-slate-400">{label}</p>
		<p className="mt-1 text-2xl font-semibold text-indigo-200">{value}</p>
	</div>
);

export default Dashboard;
