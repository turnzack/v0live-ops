# ADR 0007: Rôle du Runtime Solar2D comme Client Mobile Distant

## Statut
Accepté

## Contexte
Le moteur natif C++20 / WebGPU est optimisé pour les plateformes de bureau et le Web haut de gamme. Le portage direct sur l'ensemble de l'écosystème mobile fragmenté (notamment sur des téléphones d'entrée de gamme ou sans pilotes WebGPU stables) allongerait considérablement le cycle initial. À l'inverse, Solar2D offre un runtime 2D Lua ultra-léger et éprouvé sur iOS et Android.

## Décision
- **Solar2D n'est pas le moteur principal** : Il s'agit d'un runtime de distribution mobile 2D, introduit au Jalon 4 (M4).
- **Consommateur d'assets distants** : Il ne compile pas le C++ de notre moteur. Il télécharge de manière différentielle les assets 2D cuits (atlas PNG/WebP, audio OGG, scènes JSON, scripts Lua de gameplay) exposés par notre pipeline de publication CDN/S3.
- **Périmètre strict** : Uniquement pour les jeux 2D. Aucune tentative de rétro-porter le pipeline 3D ou Jolt Physics dans Solar2D.

## Conséquences
- **Positives** : Évite de porter un runtime C++/WebGPU complexe sur mobile avant d'avoir prouvé le concept ; réutilise une solution de déploiement mobile 2D mature ; permet des mises à jour LiveOps sans republier le binaire sur les stores.
- **Négatives / Limites** : Deux environnements d'exécution à maintenir lors du jalon 4 (moteur natif C++ et client Solar2D Lua).
