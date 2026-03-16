---
title: R-Type Network Stack
id: r-type-net
---

# R-Type Network Stack

When launched, the R-Type server main process will provide clients the abbility to create, see & connect to game servers.

When a client creates a lobby server, it will be instanced as a child process of the main server which will manage it's lifetime.

Furthermore, the childs lobbies will regularly send informations about their states to the main server so he can provide it via a HTTP API.

## Client & Lobby Manager (Main server)

When the client connects to a server, it first connects to the main server.
Most of the communications with it will mostly be FTP like: the traffic is not high and we do not want to lose data (ex: lobby creation/destruction/list).

When a client connects to a lobby, the server will redirect the connection to said lobby, severing it on it's side.
It is then the client's job to connect to the lobby via the informations provided by the main server.

## HTTP API (Main server)

The main server process keeps an IPC pipe with all of the lobbies it created, this allows him to collect runtime data and send commands to the lobbies.

Thanks to these informations, the main server maintains a public HTTP API on a separate thread.

It provides infos on like players or running games count to those who have created an account.
This API also allows admins to manage players & lobbies (banning players, creating lobbies, ...).

## Client & Game (Lobby server)

The lobby server is quite the opposite to the main server, it needs to handle a lot of incoming & outgoing data.

In order to reduce network load, all of the clients inputs are sent without awaiting any responses.

The server on his side, sends snapshots to the clients but waits for an answer: it needs to know what the clients game state is to reduce the amount of data it needs to send. (see [Game Snapshot System](game-snapshot.md))
