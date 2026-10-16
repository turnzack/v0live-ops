# Backlog & Pilotage du Projet

> **Stratégie de lancement** : Seuls le **Sprint 0 (M0)** et le **Sprint 1 (Début M1)** sont engagés fermement. Les jalons M2, M3 et M4 restent organisés en backlog priorisé sans date fixe.

---

## 🏷️ Labels à importer (GitHub Projects / Linear)

| Catégorie | Label | Description |
|---|---|---|
| **Jalon** | `jalon:M0` | Fondations, monorepo, presets et CI |
| **Jalon** | `jalon:M1` | Platformer 2D jouable (Windows + Web) |
| **Jalon** | `jalon:M2` | Démo 3D (WebGPU PBR, Jolt, Animation) |
| **Jalon** | `jalon:M3` | Éditeur de production & Visual Scripting |
| **Jalon** | `jalon:M4` | ArtCraft, S3/CDN & Distribution Solar2D |
| **Priorité** | `priorité:P0` | Bloquant / Chemin critique du jalon |
| **Priorité** | `priorité:P1` | Priorité haute |
| **Priorité** | `priorité:P2` | Priorité moyenne |
| **Priorité** | `priorité:P3` | Amélioration / Confort |
| **Attribution**| `dev:A-core-rendu` | Core C++, WebGPU, Shaders, Runtimes |
| **Attribution**| `dev:B-simulation-gameplay` | ECS, Physique 2D/3D, Gameplay, Audio, Scripting |
| **Attribution**| `dev:C-outils-pipeline` | Éditeur ImGui, Asset Pipeline, CI/CD, Build |
| **Type** | `type:feature` | Nouvelle capacité fonctionnelle |
| **Type** | `type:spike` | Étude technique exploratoire |
| **Type** | `type:test` | Suite de tests automatisés |
| **Type** | `type:ci` | Infrastructure de build et intégration continue |
| **Type** | `type:risque` | Mitigation d'un risque majeur identifié |

---

## 🎯 Sprint 0 — Jalon M0 : Fondations (2 semaines)
**Objectif de sortie** : Un push Git déclenche la CI Windows/Linux et produit des binaires de test téléchargeables.

| ID | Titre | Priorité | Assigné | Est. | Dépendances | Critère d'acceptation |
|---|---|---|---|---|---|---|
| **FND-001** | Rédiger PRODUCT_MVP.md | P0 | Produit | 1j | — | Vision, persona et premier parcours validés. |
| **FND-002** | Figer platformer 2D | P0 | Produit | 0,5j | FND-001 | ADR 0001 rédigé et acté. |
| **FND-003** | Figer Windows + Web | P0 | Produit | 0,5j | FND-001 | ADR 0002 & 0007 rédigés (Solar2D reporté à M4). |
| **FND-004** | Documenter exclusions v0 | P0 | Produit | 0,5j | FND-001 | Section hors-périmètre formalisée. |
| **FND-005** | Écrire ARCHITECTURE.md | P0 | Dev A | 1j | FND-002 | Runtimes et flux d'assets modélisés. |
| **FND-006** | Définir les 3 DoD | P0 | Équipe | 0,5j | — | DoD-Proto, Feature et Prod dans CONTRIBUTING.md. |
| **FND-007** | Créer les ADR | P0 | Équipe | 1j | — | 8 ADRs fondateurs validés. |
| **FND-010** | Init Git + LFS | P0 | Dev C | 0,5j | — | Suivi des textures/audio/modèles avec `git lfs`. |
| **FND-011** | Créer arborescence monorepo | P0 | Dev C | 0,5j | FND-010 | Structure `engine/`, `editor/`, `tools/` créée. |
| **FND-012** | CMake presets | P0 | Dev A | 1j | FND-011 | `cmake --preset` fonctionnel sur Windows et Linux. |
| **FND-013** | Manifeste vcpkg | P0 | Dev A | 1j | FND-012 | `vcpkg.json` avec flecs, box2d, imgui, glm, spdlog. |
| **FND-014** | clang-format/tidy/pre-commit| P1 | Dev C | 0,5j | FND-011 | Règles et styles enforceables. |
| **FND-015** | CODEOWNERS + PR template | P1 | Dev C | 0,5j | FND-006 | Assignations automatiques et checklist DoD active. |
| **CI-001** | CI Windows + Linux | P0 | Dev C | 1j | FND-013 | Pipelines GitHub Actions fonctionnels. |
| **CI-002** | Cache vcpkg en CI | P1 | Dev C | 0,5j | CI-001 | Temps de build réduit (x3 visé). |
| **CI-003** | Exécution CTest | P0 | Dev C | 0,5j | CI-001 | Tout test rouge bloque le merge. |
| **CI-004** | Analyse clang-tidy diff | P1 | Dev C | 0,5j | FND-014 | Commentaires de linting automatiques en PR. |
| **CI-005** | Publication des artifacts | P1 | Dev C | 0,5j | CI-001 | Binaires et smoke tests téléchargeables. |
| **CI-006** | Protection main + badge CI | P1 | Dev C | 0,5j | CI-003 | Merge bloqué sans validation CI complète. |

---

## 🚀 Sprint 1 — Début M1 : Noyau & Premier Sprite (2 semaines)
**Objectif de sortie** : Un artifact CI permet d'ouvrir l'éditeur sous Windows, d'afficher un sprite déplaçable au clavier, de modifier sa vitesse dans l'Inspector, et de sauvegarder/recharger la scène en JSON.

| ID | Titre | Priorité | Assigné | Est. | Dépendances | Critère d'acceptation |
|---|---|---|---|---|---|---|
| **CORE-001** | Logging spdlog | P0 | Dev A | 1j | FND-013 | Fichier `engine.log` et sortie console synchronisée. |
| **CORE-002** | Assertions moteur | P0 | Dev A | 0,5j | CORE-001 | Macros `ENGINE_ASSERT` avec fichier/ligne et trap debug. |
| **CORE-003** | Maths (Vec2/3/4, Mat4, AABB) | P0 | Dev B | 2j | — | 50+ tests unitaires validant l'algèbre (`TST-001`). |
| **CORE-004** | Fenêtre native + événements OS| P0 | Dev A | 1,5j | FND-013 | Fenêtre Win32/GLFW stable avec boucle d'événements. |
| **CORE-005** | Fixed timestep + interpolation | P0 | Dev B | 1,5j | CORE-004 | Simulation à pas fixe stable même sous charge. |
| **CORE-006** | Configuration JSON moteur | P1 | Dev C | 1j | — | Résolution d'écran et vsync chargeables depuis JSON. |
| **ECS-001** | Intégration flecs + réflexion | P0 | Dev B | 2j | FND-013 | Composants Transform & Tag déclarés avec introspection. |
| **ECS-002** | Phases de systèmes flecs | P0 | Dev B | 1j | ECS-001, CORE-005 | Ordre Update -> Simulate -> Render déterministe. |
| **GFX-001** | WebGPU device + swapchain | P0 | Dev A | 2j | CORE-004 | Initialisation device WebGPU et clear color à l'écran. |
| **GFX-002** | Sprite batcher + chargeur PNG | P0 | Dev A | 2j | GFX-001 | Rendu d'une texture 2D via quad batché. |
| **INP-001** | Clavier / Souris + Actions | P0 | Dev B | 1,5j | CORE-004 | Événements d'entrée mappés sur le déplacement du sprite. |
| **ECS-003** | Scène JSON + GUID | P0 | Dev C | 2j | ECS-001 | Round-trip JSON identique (`TST-005`). |
| **EDT-001** | ImGui docking + Inspector | P0 | Dev C | 2j | ECS-001, GFX-001 | Panneaux Hierarchy + Inspector modifiant la position en direct. |
| **SMP-001** | Sample platformer minimal | P0 | Équipe | 0,5j | Tout | Scène d'exemple fonctionnelle dans `samples/platformer`. |
| **TST-001** | Tests unitaires Maths | P0 | Dev B | inclus | CORE-003 | Suite complète validée en CI. |
| **TST-002** | Tests unitaires ECS | P0 | Dev B | inclus | ECS-001 | Vérification des queries et hiérarchies flecs. |
| **TST-005** | Test de round-trip scène JSON| P0 | Dev C | inclus | ECS-003 | Validation automatique en CI de la persistance. |

### Ordre d'exécution recommandé (Sprint 1)
```
CORE-004 (Fenêtre) ──► CORE-005 (Fixed Timestep) ──► GFX-001 (WebGPU Device)
       │                                                    │
       ▼                                                    ▼
CORE-001 / CORE-002 ──► CORE-003 / TST-001 (Maths)   GFX-002 (Sprite Batcher)
       │                        │                           │
       ▼                        ▼                           │
ECS-001 (flecs) ──────► ECS-002 (Phases)                    │
       │                        │                           │
       ├────────────────────────┼───────────────────────────┤
       ▼                        ▼                           ▼
ECS-003 (Scène JSON)     INP-001 (Inputs) ────────► EDT-001 (ImGui Inspector)
                                                            │
                                                            ▼
                                                   SMP-001 (Sample Validé)
```

---

## 📋 Backlog Priorisé (Sprints 2 à 5 — Clôture Jalon M1)

### Sprint 2 — Rendu & Physique 2D
- `GFX-010` Caméra orthographique lissée + suivi joueur (1j - P0)
- `GFX-011` Sprite sheets + animation par trames (1,5j - P0)
- `GFX-012` Tri des couches de rendu (Z-order / Y-sort) (0,5j - P0)
- `GFX-013` Tilemap 2D + importateur de maps Tiled (2j - P0)
- `GFX-014` Parallaxe arrière-plan 3 plans (0,5j - P1)
- `PHY-001` Wrapper Box2D v3 intégré aux phases flecs (2j - P0)
- `PHY-002` Colliders 2D (boîtes, cercles, chaînes) (1,5j - P0)
- `PHY-003` Character controller 2D (saut, gravité, inertie) (2j - P0)
- `PHY-004` Layers de collision, masques et triggers (1j - P0)
- `DBG-001` Débogage visuel des colliders avec touche F1 (0,5j - P1)

### Sprint 3 — Audio, Play Mode, Lua & Undo/Redo
- `AUD-001` Intégration miniaudio (WAV/OGG streaming) (1,5j - P0)
- `AUD-002` Mixeur audio avec bus Master, Musique et SFX (1j - P1)
- `EDT-010` Play Mode dans l'éditeur avec clonage du monde (2j - P0)
- `EDT-011` Viewport 2D avec pan, zoom et grille magnétique (1,5j - P0)
- `EDT-012` Gizmo 2D et sélection d'entités à la souris (1,5j - P0)
- `EDT-013` Undo/Redo complet via Pattern Command (Ctrl+Z/Ctrl+Y) (2j - P0)
- `SCR-001` Intégration scripting Lua 5.4 + sol2 (2j - P0)
- `SCR-002` Bindings Lua (Transform, Input, Audio, Scene) (2j - P0)

### Sprint 3bis — Physique 2D & Caméra (Box2D v3)
- [x] `PHY-001` Wrapper Box2D v3 (b2WorldId, b2BodyId, Soft Step) (2j - P0)
- [x] `PHY-002` Colliders 2D (Box, Circle, Capsule) (1,5j - P0)
- [x] `PHY-003` Character Controller 2D (Coyote time, Jump buffer, Ground detection) (2j - P0)
- [x] `PHY-004` Layers, filtering, sensor/trigger events (1j - P0)
- [x] `GFX-010` Caméra orthographique lissée avec suivi et zoom (1j - P0)
- [x] `GFX-013` Scène platformer complète avec rigidbodies et sol (1j - P1)
- [x] `DBG-001` Visualisation debug des colliders (F1 / ImGui overlay) (1j - P1)

### Sprint 4 — Éditeur Avancé & Gestion des Assets
- `EDT-020` Hierarchy d'entités complète avec drag & drop (2j - P0)
- `EDT-021` Inspector dynamique généré par la réflexion flecs (2j - P0)
- `PIPE-001` Asset Database avec indexation GUID et SHA-256 (2j - P0)
- `EDT-022` Asset Browser avec drag & drop vers la scène (2j - P0)
- `PIPE-002` File watcher d'assets avec réimportation à chaud (1j - P1)
- `SCR-003` Hot-reload des scripts Lua en cours d'exécution (1,5j - P1)
- `SCR-004` Sandbox et capture des erreurs Lua sans crash éditeur (1j - P0)

### Sprint 5 — Packaging, Build & UI
- `BLD-001` Asset Cooker et génération d'archive `game.pak` (2j - P0)
- `BLD-002` Exporteur Windows 1-clic (.exe autonome) (1,5j - P0)
- `BLD-003` Exporteur Web (Emscripten WebGPU) (3j - P0)
- `UI-001` Système UI de jeu (Canvas 2D, HUD, Score, Pause) (2j - P0)
- `ECS-010` API de persistance et sauvegarde de partie en JSON (1j - P1)
- `CI-010` Déploiement automatique de la preview Web sur Vercel (1j - P1)
- `TST-010` Test smoke automatisé sur 300 frames en CI (1j - P0)

---

## 🔮 Thèmes Backlog Long Terme (M2 → M4)
*(Organisés par capacité sans dates d'engagement contractuelles)*

### M2 — Démo 3D (Rendu PBR, Jolt Physics, Squelette 3D)
- Abstraction `GraphicsDevice` et Render Graph modulaire.
- Chargement glTF 2.0 / GLB avec matériaux PBR métalliques/rugueux.
- Éclairage directionnel avec cascades d'ombres (CSM) et clustered point lights.
- Moteur physique 3D Jolt Physics (CharacterVirtual, raycasts, formes composées).
- Animation de squelette GPU et Blend Trees pour personnage 3D.
- Viewport 3D et gizmos de manipulation 3D (Translate/Rotate/Scale).

### M3 — Éditeur de Production
- Système de Prefabs avec variantes et propagation en cascade.
- Éditeur de Tuiles (Tilemap & Auto-tiling) et Éditeur de Sprite Sheets.
- Visual Scripting basé sur graphe de nœuds.
- Timeline cinématique et animation d'événements.
- Système d'intelligence artificielle (NavMesh 3D / A* 2D / Blackboard).
- Profilage temps réel Tracy (CPU/GPU) et console runtime intégrée.

### M4 — Suite Complète & Distribution
- Pipeline ArtCraft (adaptateur API ou inbox surveillée).
- Serveur MCP pour pilotage IA de l'éditeur et du pipeline d'assets.
- Publication d'assets sur stockage S3 Contabo avec CDN mondial.
- Exportateur et runtime distant Solar2D pour distribution mobile 2D sans re-certification store.
- Télémétrie, LiveOps et configuration distante.
