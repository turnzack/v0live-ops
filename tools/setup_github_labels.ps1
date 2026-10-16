# Script de création automatique des labels dans GitHub via gh CLI

$labels = @(
    @{ Name = "jalon:M0"; Color = "0E8A16"; Description = "Jalon 0: Fondations monorepo et CI" },
    @{ Name = "jalon:M1"; Color = "1D76DB"; Description = "Jalon 1: Platformer 2D jouable" },
    @{ Name = "jalon:M2"; Color = "5319E7"; Description = "Jalon 2: Démo 3D PBR et Jolt" },
    @{ Name = "jalon:M3"; Color = "FBCA04"; Description = "Jalon 3: Éditeur de production" },
    @{ Name = "jalon:M4"; Color = "D93F0B"; Description = "Jalon 4: Suite complète & Solar2D" },
    @{ Name = "priorité:P0"; Color = "B60205"; Description = "Priorité P0: Bloquant / Immédiat" },
    @{ Name = "priorité:P1"; Color = "D93F0B"; Description = "Priorité P1: Élevée" },
    @{ Name = "priorité:P2"; Color = "FBCA04"; Description = "Priorité P2: Moyenne" },
    @{ Name = "priorité:P3"; Color = "0E8A16"; Description = "Priorité P3: Basse" },
    @{ Name = "dev:A-core-rendu"; Color = "0052CC"; Description = "Développeur A: Core C++, WebGPU, Runtimes" },
    @{ Name = "dev:B-simulation-gameplay"; Color = "006B75"; Description = "Développeur B: ECS, Physique, Gameplay, Audio" },
    @{ Name = "dev:C-outils-pipeline"; Color = "6F42C1"; Description = "Développeur C: Éditeur, Pipeline d'assets, CI/CD" },
    @{ Name = "type:feature"; Color = "A2EEEF"; Description = "Fonctionnalité" },
    @{ Name = "type:spike"; Color = "D4C5F9"; Description = "Recherche / Spike technique" },
    @{ Name = "type:test"; Color = "C2E0C6"; Description = "Tests automatisés" },
    @{ Name = "type:ci"; Color = "E99695"; Description = "Intégration continue / Build" },
    @{ Name = "type:risque"; Color = "EE0701"; Description = "Mitigation de risque majeur" }
)

Write-Host "Création des labels sur le dépôt GitHub..." -ForegroundColor Cyan

foreach ($lbl in $labels) {
    Write-Host "Création du label: $($lbl.Name)..."
    gh label create $lbl.Name --repo turnzack/v0live-ops --color $lbl.Color --description $lbl.Description --force
}

Write-Host "Tous les labels ont été créés avec succès !" -ForegroundColor Green
