import React from 'react';
import { Play, Pause, RotateCcw, Save, Undo, Redo, Keyboard, Download, Sparkles } from 'lucide-react';

export default function Header({
  isPlaying,
  onTogglePlay,
  onResetSimulation,
  canUndo,
  canRedo,
  onUndo,
  onRedo,
  onSaveScene,
  onOpenInputMap,
  onExportGame
}) {
  return (
    <header className="top-header">
      {/* Left: Brand */}
      <div className="logo-section">
        <div className="logo-badge">V0</div>
        <div>
          <span className="project-title">v0Live Ops Studio</span>
        </div>
        <span className="version-tag">Sprint 4</span>
      </div>

      {/* Center: Play Controls & Undo/Redo */}
      <div className="toolbar-center">
        <button
          className={`tool-btn ${isPlaying ? 'active-play' : ''}`}
          onClick={onTogglePlay}
          title={isPlaying ? "Arrêter la simulation (Échap)" : "Démarrer Play Mode"}
        >
          {isPlaying ? <Pause size={13} fill="#05141b" /> : <Play size={13} fill="#00f2fe" />}
          <span>{isPlaying ? 'Pause' : 'Play'}</span>
        </button>

        <button className="tool-btn" onClick={onResetSimulation} title="Réinitialiser la physique">
          <RotateCcw size={13} />
        </button>

        <div style={{ width: 1, height: 16, background: 'var(--border-subtle)', margin: '0 4px' }} />

        <button className="tool-btn" onClick={onUndo} disabled={!canUndo} style={{ opacity: canUndo ? 1 : 0.4 }} title="Annuler (Ctrl+Z)">
          <Undo size={13} />
        </button>

        <button className="tool-btn" onClick={onRedo} disabled={!canRedo} style={{ opacity: canRedo ? 1 : 0.4 }} title="Rétablir (Ctrl+Y)">
          <Redo size={13} />
        </button>
      </div>

      {/* Right: Actions & Vercel status */}
      <div className="header-actions">
        <button className="action-btn" onClick={onOpenInputMap} title="Mappage des touches (config/input.json)">
          <Keyboard size={13} />
          <span>Action Map</span>
        </button>

        <button className="action-btn" onClick={onSaveScene} title="Sauvegarder level_01.scene.json">
          <Save size={13} />
          <span>Sauvegarder JSON</span>
        </button>

        <button className="action-btn primary" onClick={onExportGame} title="Exporter le build pour Windows (.exe) ou Web (.wasm)">
          <Download size={13} />
          <span>Exporter Build</span>
        </button>

        <div style={{
          display: 'flex',
          alignItems: 'center',
          gap: 6,
          background: 'rgba(0, 245, 160, 0.1)',
          padding: '4px 10px',
          borderRadius: 'var(--radius-sm)',
          border: '1px solid rgba(0, 245, 160, 0.25)',
          fontSize: 11,
          fontWeight: 600,
          color: '#00f5a0'
        }}>
          <span style={{ width: 6, height: 6, borderRadius: '50%', background: '#00f5a0', boxShadow: '0 0 6px #00f5a0' }} />
          <span>Vercel Live</span>
        </div>
      </div>
    </header>
  );
}
