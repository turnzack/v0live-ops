# MVP — Suite de création de jeux

## Objectif
Permettre à un utilisateur de créer, tester et publier un petit jeu sans assembler plusieurs outils tiers.

## Premier parcours utilisateur
1. Créer un projet.
2. Importer un sprite (PNG).
3. Le placer dans une scène.
4. Ajouter un mouvement (Input / Velocity).
5. Lancer le Play Mode dans l'éditeur.
6. Sauvegarder la scène en JSON stable.
7. Exporter pour Windows (.exe autonome).
8. Exporter pour le Web (Emscripten / WebGPU).

## Jalon 1 (M1) — Cible immédiate
Un platformer 2D jouable, créé dans l’éditeur, exporté pour Windows et Web, avec scène sauvegardée en JSON et sprite déplaçable.

## Hors périmètre v0
- Consoles (PlayStation, Xbox, Switch)
- Multijoueur réseau et réplication
- Illumination globale dynamique (GI temps réel complexe)
- iOS / macOS (priorité Windows + Web en natif, Solar2D pour mobile jalon 4)
- Backend graphique Vulkan natif direct (WebGPU est le backend unique v0)
- Génération ArtCraft automatisée obligatoire (mode optionnel ou repli via inbox)
