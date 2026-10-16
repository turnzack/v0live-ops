# ADR 0003: Choix de flecs comme framework ECS avec réflexion

## Statut
Accepté

## Contexte
La gestion des entités et des composants nécessite des performances élevées, du cache-friendly data-oriented design, mais également une introspection robuste pour l'inspecteur d'éditeur, la sérialisation de scènes et les bindings de scripting (Lua).

## Décision
Adopter `flecs` comme architecture ECS principale :
- Prise en charge native des relations d'entités (hiérarchies parent/enfant).
- Système de réflexion et métadonnées intégré pour inspecter et modifier les composants sans code UI manuel lourd.
- Ordonnancement explicite des phases et systèmes (pipeline d'exécution déterministe).

Un benchmark comparatif de montée en charge (100 000 entités Transform à 60 FPS) sera exécuté dès le Sprint 1 pour valider formellement les métriques de performance.

## Conséquences
- **Positives** : Évite d'écrire un système de réflexion maison ; facilite grandement l'inspecteur générique ImGui et la sérialisation JSON ; excellente intégration C/C++ et bindings multi-langages.
- **Négatives / Limites** : Légère courbe d'apprentissage sur le modèle de requêtes et d'archétypes de flecs.
