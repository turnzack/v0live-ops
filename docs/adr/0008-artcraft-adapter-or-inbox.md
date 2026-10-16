# ADR 0008: Intégration d'ArtCraft par Adaptateur Découplé ou Boîte de Dépôt (Inbox)

## Statut
Accepté

## Contexte
ArtCraft est envisagé pour la génération assistée d'assets. Cependant, les spécifications de son API externe, CLI ou backend restent incertaines et peuvent évoluer ou présenter des blocages d'intégration directe. Faire dépendre le cœur du moteur d'une API propriétaire non figée mettrait en péril le calendrier de livraison.

## Décision
1. Définir une interface abstraite `IAssetGenerator` dans le Command Center / Pipeline d'assets.
2. Si ArtCraft dispose d'une API/CLI stable et documentée : développer un adaptateur client dédié (`tools/artcraft-adapter`).
3. Si ArtCraft ne fournit pas d'API automatisable propre : repli immédiat sur un système de **boîte de dépôt surveillée ("Inbox Pattern")** :
   - Un dossier `raw/inbox` est monitoré par un observateur de fichiers (`file watcher`).
   - Tout fichier glissé manuellement ou exporté par ArtCraft/Blender y est détecté, validé, attribué d'un GUID et promu dans le pipeline.
4. Aucun blocage de la chaîne de dev moteur si ArtCraft n'est pas disponible : Blender et les assets classiques sont supportés par défaut.

## Conséquences
- **Positives** : Élimine le risque bloquant de dépendance envers un outil externe ; résilience maximale de la chaîne de production d'assets.
- **Négatives / Limites** : Légère duplication possible si l'on supporte à la fois l'inbox et une API ultérieure.
