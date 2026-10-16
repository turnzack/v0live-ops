# ADR 0009: Choix de SDL3 pour la Gestion de la Fenêtre Native et des Événements d'Entrée

## Statut
Accepté

## Contexte
Le moteur nécessite une couche d'abstraction de fenêtre et de gestion d'événements OS (clavier, souris, redimensionnement, focus, manettes ultérieures) qui soit :
- Multiplateforme (Windows, Linux, Web / Emscripten).
- Compatible nativement avec WebGPU (obtention de `HWND`/`Display` ou surface WebGPU).
- Moderne et pérenne pour les évolutions futures (audio, gamepad, DPI scaling).

Les options envisagées étaient :
1. **Win32 natif** : Contrôle total sous Windows mais verbeux et non portable (bloque la cible Linux et Web).
2. **GLFW** : Léger et populaire, mais centré historiquement sur OpenGL/Vulkan, avec une gestion audio/manettes moins intégrée.
3. **SDL3** : Version moderne de Simple DirectMedia Layer, architecture refondue, gestion d'événements performante, support natif de surfaces GPU, audio et WebAssembly.

## Décision
Adopter **SDL3** comme bibliothèque de fenêtrage et d'entrée pour la suite moteur :
- Gestion du cycle de vie de la fenêtre principale et du contexte d'affichage.
- Capture unifiée des événements clavier et souris pour le système d'input (`INP-001`).
- Fourniture des handles natifs nécessaires à l'initialisation de l'instance WebGPU (`GFX-001`).

## Conséquences
- **Positives** : API C moderne et propre ; excellente portabilité Windows/Linux/Web ; préparation native du support gamepad et audio sans bibliothèque hétérogène supplémentaire.
- **Négatives / Limites** : Bibliothèque dynamique/statique supplémentaire gérée via `vcpkg` (`sdl3`).
