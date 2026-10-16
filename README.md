# Engine Suite

[![CI](https://github.com/turnzack/v0live-ops/actions/workflows/ci.yml/badge.svg)](https://github.com/turnzack/v0live-ops/actions/workflows/ci.yml)
[![Standard C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Graphics WebGPU](https://img.shields.io/badge/Graphics-WebGPU-brightgreen.svg)](https://webgpu.io)
[![Status](https://img.shields.io/badge/Status-M0%20Fondations-orange.svg)](#)

Suite de création de jeux vidéo unifiée : moteur 2D/3D natif C++20/WebGPU, éditeur graphique intégré et pipeline de publication multi-plateforme.

---

## 🎯 Objectif Immédiat (Jalon 1 — Platformer Jouable)
Un platformer 2D minimal :
- Créé et composé directement dans l’éditeur.
- Jouable depuis un binaire produit automatiquement par la CI Windows.
- Scène sauvegardée en format JSON stable avec identifiants GUID.
- Sprite déplaçable au clavier avec caméra et contrôles fluides.

---

## 📁 Structure du Monorepo

```
engine-suite/
├── docs/                 # Documentation d'architecture, MVP, DoD et ADRs
│   ├── adr/              # Architecture Decision Records (0001 à 0008)
│   ├── PRODUCT_MVP.md    # Cadrage produit, persona et périmètre v0
│   ├── ARCHITECTURE.md   # Modèle bi-runtime et pipeline d'assets
│   ├── BACKLOG.md        # Backlog complet et découpage des sprints
│   └── CONTRIBUTING.md   # Conventions et 3 niveaux de DoD
├── engine/               # Moteur natif (C++20)
│   ├── core/             # Types de base, mathématiques, logging, assertions
│   ├── ecs/              # Gestion d'entités flecs et système de réflexion
│   ├── renderer/         # Device WebGPU et sprite batcher
│   ├── physics2d/        # Intégration Box2D v3
│   ├── physics3d/        # Intégration Jolt Physics (M2)
│   ├── audio/            # Backend audio miniaudio
│   ├── scripting/        # Moteur de script Lua 5.4 / sol2
│   └── ui/               # Moteur d'interface runtime
├── editor/               # Éditeur de jeu (ImGui Docking)
├── runtime/              # Point d'entrée exécutable autonome du jeu
├── tools/                # Outils de cuisson, adaptateur ArtCraft et publication
├── exporters/            # Exporteurs de runtimes distants (Solar2D)
├── samples/              # Exemples complets (platformer 2D, démo 3D)
├── tests/                # Suites de tests (unit, integration, perf, golden)
└── .github/              # Actions CI Windows/Linux, CODEOWNERS et PR template
```

---

## 🛠️ Démarrage Rapide

### Prérequis
- Compilateur C++20 (MSVC v143+ sur Windows, GCC 11+ ou Clang 13+ sur Linux)
- CMake 3.21+
- Ninja Build
- Git avec extension Git LFS (`git lfs install`)

### Compilation & Tests (CMake Presets)
```bash
# Configuration et compilation Debug
cmake --preset windows-msvc-debug
cmake --build --preset windows-debug

# Exécution des tests unitaires
ctest --preset windows-debug-test
```
