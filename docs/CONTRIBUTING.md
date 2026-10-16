# Guide de Contribution & Définition de Terminé (DoD)

## Conventions de Code & Qualité
- **C++ Standard** : C++20 minimum (ISO/IEC 14882:2020), avec adoption progressive de C++23.
- **Formatage** : Obligatoire via `clang-format` (fichier de configuration `.clang-format` à la racine).
- **Analyse Statique** : `clang-tidy` exécuté en CI sur les PR et via hooks de pre-commit.
- **Nommage** :
  - Classes et Structures : `PascalCase` (`RenderDevice`, `SceneSerializer`)
  - Fonctions et Méthodes : `camelCase` (`loadScene()`, `createBuffer()`)
  - Variables membres : `m_camelCase` (`m_windowHandle`, `m_entityCount`)
  - Constantes et Énumérations : `kPascalCase` ou `UPPER_SNAKE_CASE`

## Les 3 Niveaux de Definition of Done (DoD)

### 1. DoD-Proto (Jalon 1 — M1)
- [ ] Code compilable sans warning bloquant sur les presets CMake Windows et Linux.
- [ ] Tests unitaires validant la logique pure (maths, sérialisation JSON, ECS).
- [ ] Fonctionnalité vérifiée et démontrable dans le sample `samples/platformer`.
- [ ] Binaire généré avec succès par le workflow GitHub Actions CI.

### 2. DoD-Feature (Jalons 2 & 3 — M2 & M3)
- [ ] Tous les critères du **DoD-Proto**.
- [ ] Intégration complète dans l'éditeur (Hierarchy, Inspector, Viewport).
- [ ] Cycle Play Mode vérifié (Play -> Stop restaure fidèlement la scène sans fuite de mémoire).
- [ ] Gestion réversible : toute modification de scène supporte `Undo` (Ctrl+Z) et `Redo` (Ctrl+Y) via le pattern Command.
- [ ] Messages d'erreur explicites et tolérants aux pannes (pas de crash silencieux ou de panique non attrapée).
- [ ] Documentation technique mise à jour dans `docs/`.

### 3. DoD-Prod (Jalon 4 — M4 et versions finales)
- [ ] Tous les critères du **DoD-Feature**.
- [ ] Budget de frame respecté : 60 FPS constants sur la machine de référence (16.6 ms budget GPU/CPU instrumenté sous Tracy).
- [ ] Tests de non-régression automatisés (tests unitaires, d'intégration et golden images).
- [ ] Stratégie de migration des scènes et assets validée (compatibilité descendante ou convertisseur automatique).
- [ ] Intégrité des hashes vérifiée (SHA-256) pour la chaîne de distribution CDN/S3.

## Workflow Git & Pull Requests
- Branches de travail : `feat/<ticket-id>-description`, `fix/<ticket-id>-description`, `spike/<ticket-id>`.
- Les commits doivent respecter la convention Conventional Commits : `feat:`, `fix:`, `docs:`, `chore:`, `refactor:`, `test:`, `ci:`.
- Protection de la branche `main` : les PR doivent avoir une CI verte avant tout merge.
