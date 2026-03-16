---
title: Network Model
id: game-snapshot
---

# Network Model

The Network Model used in the R-Type game is heavely inspired from [Quake 3's](https://fabiensanglard.net/quake3/network.php).

It uses a system of snapshots: each tick, the server will compile all game states modifications and send them to the clients.

Then, based on what latest game state a client has received, addapt the content of the next one to limit the amount of data sent.

If the short explanation is not clear enough, you can read about the original ([https://fabiensanglard.net/quake3/network.phpere](https://fabiensanglard.net/quake3/network.php)) or continue this article which uses resources of said article.

## Client side

The client side of the network model is fairly simple: Clients sends commands to the server each frame and receive updates for the gamestate.

## Server side

The server side is a bit more complex, it has to propagate the Master gamestate to each clients while accounting for lost UDP packets.

This features three key elements:

- [The master gamestate](#the-master-gamestate)
- [Per client snapshot buffer](#per-client-snapshot-buffer)
- [Delta Compression](#delta-compression)


### The master gamestate

The master gamestate designs the content of the ECS at the current frame after having run all systems & handled all clients inputs.

### Per client snapshot buffer

Each frame, once the master gamestate is ready, the server will create a snapshot of all entities in the ECS and store it in a round buffer of 96 slots for each clients which represents roughly 3 secs of runtime at 32 tps.

This buffer will keep track of which latest snapshot the related client is has acknowledged.

### Delta compression

After having generated the server tick's snapshot, for each clients, the server will compare what has changed since the last acknowledged snapshot in it's ring buffer.

It will then compile all of these changes and send them to the appropriate client.
