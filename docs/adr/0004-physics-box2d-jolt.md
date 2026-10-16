# ADR 0004: Stratégie Physique — Box2D v3 (2D) et Jolt Physics (3D)

## Statut
Accepté

## Contexte
Les besoins physiques diffèrent fondamentalement entre les simulations 2D (précision des plateformes, friction des coins, rapidité) et les simulations 3D (corps rigides, collision de maillages, ragdolls, multithreading massif). Une bibliothèque 3D est souvent inefficace ou surdimensionnée pour du pur gameplay 2D.

## Décision
Adopter une stratégie duale ciblée :
- **Physique 2D (Jalon 1+)** : `Box2D` version 3 pour le platformer 2D, les colliders polygonaux/cercles et le character controller 2D.
- **Physique 3D (Jalon 2+)** : `Jolt Physics` pour les simulations 3D avancées (CharacterVirtual, raycasts 3D, joints et performances multithread).

## Conséquences
- **Positives** : Légèreté et robustesse éprouvée de Box2D pour le platformer 2D du MVP ; performances de pointe et modernité de Jolt pour la 3D sans dette technique.
- **Négatives / Limites** : Deux bibliothèques physiques distinctes dans le monorepo, nécessitant chacune son wrapper ECS (`engine/physics2d` et `engine/physics3d`).
