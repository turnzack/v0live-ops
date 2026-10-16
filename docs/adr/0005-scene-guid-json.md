# ADR 0005: Format de Scène JSON et Identifiants Uniques par GUID

## Statut
Accepté

## Contexte
Les scènes stockées sous format binaire propriétaire sont impossibles à relire, comparer (diff) ou fusionner (merge) dans Git par des équipes collaboratives. De plus, référencer des fichiers par chemins relatifs ou absolus brise immédiatement l'intégrité lors de renommages ou déplacements d'assets.

## Décision
1. **Format JSON clair et versionné** : Toutes les scènes sont sérialisées dans un format JSON ordonné et stable (`nlohmann-json`), avec un champ explicite `"version": 1`.
2. **Identifiants GUID universels** : Toutes les entités persistantes et les références aux assets utilisent des GUIDs (UUID v4).
3. **Tests de round-trip obligatoires** : Un test unitaire CI (`TST-005`) vérifie qu'une scène désérialisée puis resérialisée produit un document identique au bit près.

## Conséquences
- **Positives** : Résolution aisée des conflits Git ; renommage et réorganisation d'assets totalement transparents sans liens brisés ; débogage visuel direct dans un éditeur de texte.
- **Négatives / Limites** : Fichiers plus volumineux que du binaire compact (sera compressé dans les `game.pak` lors du packaging final du jeu).
