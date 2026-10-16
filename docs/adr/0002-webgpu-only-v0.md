# ADR 0002: WebGPU comme unique backend de rendu graphique pour la v0

## Statut
Accepté

## Contexte
Supporter simultanément Direct3D 11/12, Vulkan et Metal dès le départ exige un effort d'abstraction disproportionné pour une petite équipe. Le moteur vise dès l'origine à la fois les exécutables de bureau (Windows) et le déploiement Web (WebAssembly / navigateurs modernes).

## Décision
Pour la version 0 (Jalons M0 à M3), le moteur utilisera l'API WebGPU standardisée (`webgpu.h`) comme seul et unique backend de rendu graphique :
- Sur Windows / Linux natif : via une implémentation native (Dawn de Google ou wgpu-native).
- Sur Web : via l'API WebGPU native du navigateur par le biais du compilateur Emscripten.

## Conséquences
- **Positives** : Base de code graphique unifiée ; même pipeline de shaders (WGSL) pour le bureau et le web ; aucune surcharge de maintenance de multiples backends bas niveau en phase initiale.
- **Négatives / Limites** : Dépendance envers la maturité des bibliothèques d'implémentation natives et le support des navigateurs (compatibilité restreinte sur très vieux matériels ou OS obsolètes).
