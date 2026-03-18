import { useEffect, useState } from 'react';
import {
	getLobbies,
	getLobbyLeaderboard,
	getGlobalLeaderboard,
	type Lobby,
	type LeaderboardEntry,
	type GlobalLeaderboardEntry,
} from '../services/auth';

const Home = () => {
	const [lobbies, setLobbies] = useState<Lobby[]>([]);
	const [loadingLobbies, setLoadingLobbies] = useState(false);
	const [selectedLobby, setSelectedLobby] = useState<number | null>(null);
	const [leaderboard, setLeaderboard] = useState<LeaderboardEntry[]>([]);
	const [loadingLeaderboard, setLoadingLeaderboard] = useState(false);
	const [globalLeaderboard, setGlobalLeaderboard] = useState<GlobalLeaderboardEntry[]>([]);
	const [loadingGlobalLeaderboard, setLoadingGlobalLeaderboard] = useState(false);
	const [error, setError] = useState<string | null>(null);

	const refreshLobbies = async () => {
		try {
			setLoadingLobbies(true);
			setError(null);
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

	const refreshGlobalLeaderboard = async () => {
		try {
			setLoadingGlobalLeaderboard(true);
			setError(null);
			const data = await getGlobalLeaderboard(20);
			setGlobalLeaderboard(data);
		} catch (err: any) {
			setError(err.message || 'Failed to load global leaderboard');
		} finally {
			setLoadingGlobalLeaderboard(false);
		}
	};

	const loadLeaderboard = async (lobbyId: number) => {
		try {
			setLoadingLeaderboard(true);
			setError(null);
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
		refreshGlobalLeaderboard();
	}, []);

	useEffect(() => {
		if (selectedLobby) {
			loadLeaderboard(selectedLobby);
		}
	}, [selectedLobby]);

	return (
		<main className="min-h-screen bg-slate-950 text-slate-50">
			<div className="mx-auto flex max-w-6xl flex-col gap-8 px-4 py-10 lg:flex-row">
				<section className="flex-[1.5] space-y-6">
					<div className="rounded-xl bg-slate-900/60 p-6 shadow-lg ring-1 ring-slate-800">
						<h1 className="text-2xl font-semibold text-indigo-100">R-Type Live Dashboard</h1>
						<p className="mt-2 text-sm text-slate-300">
							Track active lobbies and current scores in real time.
						</p>
					</div>

					<div className="rounded-xl bg-slate-900/60 p-6 shadow-lg ring-1 ring-slate-800">
						<div className="mb-4 flex items-center justify-between">
							<h2 className="text-xl font-semibold">Active Lobbies</h2>
							<button
								className="rounded-md border border-slate-700 px-3 py-1 text-sm text-slate-100 hover:border-indigo-400"
								onClick={() => {
									refreshLobbies();
									refreshGlobalLeaderboard();
								}}
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

				<section className="flex-1 rounded-xl bg-slate-900/60 p-6 shadow-lg ring-1 ring-slate-800">
					<div className="mb-4 flex items-center justify-between">
						<h2 className="text-xl font-semibold">Global Leaderboard</h2>
						<button
							className="rounded-md border border-slate-700 px-3 py-1 text-sm text-slate-100 hover:border-indigo-400"
							onClick={refreshGlobalLeaderboard}
							disabled={loadingGlobalLeaderboard}
						>
							{loadingGlobalLeaderboard ? 'Refreshing…' : 'Refresh'}
						</button>
					</div>
					{loadingGlobalLeaderboard && <p className="text-sm text-slate-300">Loading global leaderboard…</p>}
					{!loadingGlobalLeaderboard && globalLeaderboard.length === 0 && (
						<p className="text-sm text-slate-300">No completed matches yet.</p>
					)}
					{!loadingGlobalLeaderboard && globalLeaderboard.length > 0 && (
						<div className="space-y-2">
							{globalLeaderboard.map((entry, idx) => (
								<div
									key={entry.playerName + idx}
									className="rounded-lg border border-slate-800 bg-slate-900 px-4 py-3"
								>
									<div className="flex items-center justify-between">
										<p className="text-sm font-semibold text-indigo-200">{entry.playerName}</p>
										<p className="text-lg font-semibold text-emerald-300">{entry.score}</p>
									</div>
									<p className="mt-1 text-xs text-slate-400">
										Matches {entry.matchesPlayed} · Kills {entry.kills} · Deaths {entry.deaths}
									</p>
								</div>
							))}
						</div>
					)}
				</section>
			</div>
		</main>
	);
};

export default Home;
