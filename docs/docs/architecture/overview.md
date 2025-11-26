---
title: Overview
id: overview
slug: /architecture/
---


## Entity Component System (ECS)

This project uses an Entity–Component–System (ECS) architecture to implement a multiplayer R-Type-like game. The ECS is designed to run both:

- **Server-side (headless)**:
Contains no rendering or input systems. Only gameplay logic, networking, and simulation.

- **Client-side**:
Adds rendering, animation, and local input prediction.

The authoritative simulation runs on the server. Clients receive replicated states and send player actions.
