## Summary
Cette PR regroupe l'ensemble des travaux de la branche `fix-solo-parity` pour améliorer la parité solo/multijoueur et la robustesse du pipeline réseau/snapshots:
- amélioration de la synchronisation client/serveur (deltas, timing d'application, prédiction locale),
- ajout et stabilisation de contenu gameplay (Boss/Shooter et composants associés),
- corrections de logique de spawn/despawn et nettoyage de logs bruités,
- corrections build/plateforme.

## Related Issue(s)
Closes #TBD

## Included Commits
- `a41a6cd` 🐛 fix(server): improve enemy respawn logic and boss spawn conditions
- `4c41a4d` ✨ Add boss entity and related systems for enhanced gameplay dynamics
- `7df06b4` ♻️ Refactor and enhance game mechanics
- `0cf08bf` 🐛 Fix default state of build of MacOS on Release
- `603d901` 🐛 fix(client/network): enhance player prediction and ID assignment logic in multiplayer system
- `e465b7a` ✨ Enhance multiplayer scene loading by adding new components and textures for Boss and Shooter entities
- `7fadad9` 🐛 Remove unnecessary logging for bullet entities and clean up related code
- `3045c59` 🐛 fix(client/network): apply server snapshot deltas on client tick and add snapshot logs
- `bda9c6d` ✨ Implement deferred entity destruction and enhance bullet logging
- `d175db5` 🚧 on fix for bullet
- `c6c7e31` ✨ Enhance README
- `be20e3f` ✨ Add MIT License file

## Changes
- Ajout de l'entité Boss et des systèmes/composants/ressources associés côté gameplay et chargement de scène.
- Enrichissement du chargement de scène multijoueur (textures et composants supplémentaires pour Boss/Shooter).
- Amélioration de l'application des snapshot deltas côté client (application au tick client + logs réseau utiles).
- Amélioration de la logique de prédiction client et de l'assignation des IDs joueurs.
- Mise en place de la destruction différée d'entités pour sécuriser les suppressions pendant les itérations système.
- Ajustements de mécaniques gameplay (bullets/enemies) et nettoyage des logs inutiles liés aux projectiles.
- Correctifs récents sur la logique de respawn ennemis et les conditions d'apparition du boss.
- Correctif build Release macOS.
- Refactorings transverses sur les mécaniques et la synchro pour réduire les écarts solo/multi.
- Correctif sur la gestion de l'ordre d'arrivée des composants dans les deltas (suppression des faux warnings "Missing Transform/Velocity" quand les composants arrivent plus tard dans le même delta).

## Risk & Rollback
Risques potentiels:
- Régression de synchro en multijoueur (entités non initialisées visuellement selon l'ordre des composants réseau).
- Effets de bord sur spawn/despawn (notamment boss, bullets, destructions différées).

Rollback:
1. Revert complet de la branche/PR si régression large constatée.
2. Sinon revert ciblé des commits réseau/snapshot ou gameplay concernés.
3. Vérifier en priorité les systèmes `handle_snapshots_deltas_system`, `server_*_system`, et les loaders de scène.

## Testing
1. Build local: `cmake --build build -j4`.
2. Lancer serveur + client en multijoueur et valider:
   - apparition/synchronisation player, shooters, bullets, boss,
   - absence de warnings parasites "Missing Transform/Velocity" lors des créations réseau normales,
   - cohérence des destructions d'entités (pas de crash, pas d'entités fantômes).
3. Vérifier que les mécaniques restent cohérentes en solo vs multi (spawn, déplacements, tirs, collisions, boss).
4. Vérifier un build Release sur macOS (ou CI équivalente) pour confirmer le correctif plateforme.

## Checklist
- [ ] Source branch is based on `dev`
- [ ] Linked to the correct GitHub issues
- [ ] Added at least 2 people as reviewers, 4 if you are merging into main
- [ ] No conflicts
- [ ] Documentation deployment CI must pass
