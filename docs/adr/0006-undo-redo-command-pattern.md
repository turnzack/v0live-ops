# ADR 0006: Undo/Redo Précoce via le Pattern Command

## Statut
Accepté

## Contexte
L'ajout a posteriori d'un système d'annulation (Undo/Redo) sur un éditeur mature est l'une des causes majeures de réécriture complète et de régressions dans les outils de jeu. Si les modifications d'entités modifient directement la mémoire sans passer par une pile de commandes, la rétro-ingénierie devient coûteuse et fragile.

## Décision
Adopter le pattern `Command` dès le Sprint 3 de l'éditeur :
- Toute action modifiant une entité, un composant, une hiérarchie ou un paramètre de scène est encapsulée dans une classe dérivée de `ICommand` (`execute()`, `undo()`, `redo()`).
- Une `CommandHistory` gère la pile d'actions, bornée en mémoire, avec raccourcis clavier Ctrl+Z / Ctrl+Y.
- Les modifications déclenchées depuis l'Inspector ou le Viewport passent obligatoirement par cette abstraction.

## Conséquences
- **Positives** : Expérience utilisateur robuste et professionnelle dès le MVP ; zéro dette technique accumulée sur les mutations de scène.
- **Négatives / Limites** : Discipline rigoureuse exigée : interdiction formelle de muter la scène directement depuis les widgets d'interface ImGui.
