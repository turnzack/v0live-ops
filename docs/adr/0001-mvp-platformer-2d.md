# ADR 0001: Périmètre MVP centré sur le Platformer 2D

## Statut
Accepté

## Contexte
Le développement d'un moteur de jeu complet et d'un éditeur sans cible concrète mène fréquemment à la sur-ingénierie et à l'effet "moteur théorique" sans produit jouable. Il est nécessaire de contraindre le premier livrable majeur (Jalon 1 - M1) sur une boucle de gameplay maîtrisée.

## Décision
Le premier cas d'usage complet et le jeu de référence sera un platformer 2D comprenant :
- Un personnage avec contrôle de mouvement et saut (controller 2D).
- Des tuiles statiques et plateformes avec collisions.
- Des items récupérables (pickups) avec gestion de score.
- Un éditeur permettant de composer et de sauvegarder cette scène en JSON.
- Une capacité de build autonome pour Windows et le Web.

## Conséquences
- **Positives** : Réduit considérablement le risque d'abstraction prématurée ; valide la chaîne complète (Éditeur -> Scène -> Simulation -> Build CI) dès le jalon 1.
- **Négatives / Limites** : Les fonctionnalités 3D (mesh, shaders PBR, caméras perspectivistes) sont strictement reportées au jalon 2 (M2).
