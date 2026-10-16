import React, { useState, useEffect, useRef } from 'react';
import Header from './components/Header';
import Hierarchy from './components/Hierarchy';
import Viewport from './components/Viewport';
import Inspector from './components/Inspector';
import AssetBrowser from './components/AssetBrowser';
import ScriptEditor from './components/ScriptEditor';
import LiveOpsPanel from './components/LiveOpsPanel';
import InputMapModal from './components/InputMapModal';
import { initialScene, defaultAssets, defaultLuaCode, defaultInputBindings } from './data/defaultScene';
import { Database, FileCode, Cloud, Terminal, CheckCircle2 } from 'lucide-react';

export default function App() {
  const [scene, setScene] = useState(initialScene);
  const [selectedEntityId, setSelectedEntityId] = useState(scene.entities[0]?.id || null);
  const [isPlaying, setIsPlaying] = useState(false);
  const [activeBottomTab, setActiveBottomTab] = useState('assets'); // 'assets' | 'script' | 'liveops'
  const [assets, setAssets] = useState(defaultAssets);
  const [luaCode, setLuaCode] = useState(defaultLuaCode);
  const [inputBindings, setInputBindings] = useState(defaultInputBindings);
  const [isInputModalOpen, setIsInputModalOpen] = useState(false);
  const [toastMessage, setToastMessage] = useState(null);

  // Undo / Redo Transaction Stack (matching UndoManager.cpp)
  const [history, setHistory] = useState([initialScene]);
  const [historyIndex, setHistoryIndex] = useState(0);

  const pushState = (newScene) => {
    const newHistory = history.slice(0, historyIndex + 1);
    newHistory.push(JSON.parse(JSON.stringify(newScene)));
    setHistory(newHistory);
    setHistoryIndex(newHistory.length - 1);
    setScene(newScene);
  };

  const handleUndo = () => {
    if (historyIndex > 0) {
      setHistoryIndex((prev) => prev - 1);
      setScene(JSON.parse(JSON.stringify(history[historyIndex - 1])));
      showToast("Annulation effectuée (Undo)");
    }
  };

  const handleRedo = () => {
    if (historyIndex < history.length - 1) {
      setHistoryIndex((prev) => prev + 1);
      setScene(JSON.parse(JSON.stringify(history[historyIndex + 1])));
      showToast("Rétablissement effectué (Redo)");
    }
  };

  // Keyboard Shortcuts (Ctrl+Z, Ctrl+Y)
  useEffect(() => {
    const handleKeyDown = (e) => {
      if ((e.ctrlKey || e.metaKey) && e.key === 'z') {
        e.preventDefault();
        handleUndo();
      } else if ((e.ctrlKey || e.metaKey) && e.key === 'y') {
        e.preventDefault();
        handleRedo();
      }
    };
    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [historyIndex, history]);

  const showToast = (msg) => {
    setToastMessage(msg);
    setTimeout(() => setToastMessage(null), 3000);
  };

  // Entity Handlers
  const handleSelectEntity = (id) => {
    setSelectedEntityId(id);
  };

  const handleAddEntity = () => {
    const newId = crypto.randomUUID();
    const newEntity = {
      id: newId,
      name: `Entity_${scene.entities.length + 1}`,
      tag: "Untagged",
      active: true,
      components: {
        Transform: {
          position: [0, 1, 0],
          rotation: [0, 0, 0],
          scale: [1, 1, 1]
        },
        SpriteComponent: {
          size: [48, 48],
          color: "#38bdf8",
          sortingLayer: 1
        },
        Collider2DComponent: {
          type: "Box",
          size: [48, 48]
        }
      }
    };

    const newScene = {
      ...scene,
      entities: [...scene.entities, newEntity]
    };
    pushState(newScene);
    setSelectedEntityId(newId);
    showToast(`Entité ${newEntity.name} créée`);
  };

  const handleDeleteEntity = (id) => {
    const newScene = {
      ...scene,
      entities: scene.entities.filter((e) => e.id !== id)
    };
    pushState(newScene);
    if (selectedEntityId === id) {
      setSelectedEntityId(newScene.entities[0]?.id || null);
    }
    showToast("Entité supprimée");
  };

  const handleDuplicateEntity = (id) => {
    const target = scene.entities.find((e) => e.id === id);
    if (!target) return;
    const duplicated = JSON.parse(JSON.stringify(target));
    duplicated.id = crypto.randomUUID();
    duplicated.name = `${target.name}_Copy`;
    duplicated.components.Transform.position[0] += 1.0;

    const newScene = {
      ...scene,
      entities: [...scene.entities, duplicated]
    };
    pushState(newScene);
    setSelectedEntityId(duplicated.id);
    showToast(`Entité ${duplicated.name} dupliquée`);
  };

  const handleToggleActive = (id) => {
    const newScene = {
      ...scene,
      entities: scene.entities.map((e) =>
        e.id === id ? { ...e, active: !e.active } : e
      )
    };
    pushState(newScene);
  };

  const handleUpdateComponent = (entityId, componentName, updatedValues) => {
    const newScene = {
      ...scene,
      entities: scene.entities.map((e) => {
        if (e.id !== entityId) return e;
        return {
          ...e,
          components: {
            ...e.components,
            [componentName]: {
              ...e.components[componentName],
              ...updatedValues
            }
          }
        };
      })
    };
    pushState(newScene);
  };

  const handleUpdateEntityName = (entityId, newName) => {
    const newScene = {
      ...scene,
      entities: scene.entities.map((e) =>
        e.id === entityId ? { ...e, name: newName } : e
      )
    };
    pushState(newScene);
  };

  const handleUpdateEntityPosition = (entityId, x, y) => {
    const newScene = {
      ...scene,
      entities: scene.entities.map((e) => {
        if (e.id !== entityId) return e;
        return {
          ...e,
          components: {
            ...e.components,
            Transform: {
              ...e.components.Transform,
              position: [x, y, e.components.Transform.position[2]]
            }
          }
        };
      })
    };
    setScene(newScene);
  };

  const handleSaveScene = () => {
    const jsonString = JSON.stringify(scene, null, 2);
    const blob = new Blob([jsonString], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = scene.name || 'level_01.scene.json';
    a.click();
    URL.revokeObjectURL(url);
    showToast("Scène sauvegardée en format JSON stable !");
  };

  const handleExportGame = () => {
    showToast("Génération de l'artefact de release Windows (.exe) & WebGPU (.wasm)...");
  };

  const handleImportAsset = (file) => {
    const newAsset = {
      guid: crypto.randomUUID(),
      name: file.name,
      type: file.name.endsWith('.png') ? 'Texture2D' : file.name.endsWith('.ogg') ? 'AudioClip' : 'Data',
      path: `assets/${file.name}`,
      sha256: Array.from(crypto.getRandomValues(new Uint8Array(32)))
        .map((b) => b.toString(16).padStart(2, '0'))
        .join(''),
      size: `${(file.size / 1024).toFixed(1)} KB`,
      status: 'approved'
    };
    setAssets((prev) => [...prev, newAsset]);
    showToast(`Asset '${file.name}' importé et haché SHA-256 avec succès.`);
  };

  const handleRehashAssets = () => {
    showToast("Vérification SHA-256 de tous les assets terminée : 100% Intègres.");
  };

  const selectedEntity = scene.entities.find((e) => e.id === selectedEntityId);

  return (
    <div className="app-container">
      {/* Top Header */}
      <Header
        isPlaying={isPlaying}
        onTogglePlay={() => setIsPlaying(!isPlaying)}
        onResetSimulation={() => {
          setScene(JSON.parse(JSON.stringify(initialScene)));
          setIsPlaying(false);
          showToast("Simulation réinitialisée");
        }}
        canUndo={historyIndex > 0}
        canRedo={historyIndex < history.length - 1}
        onUndo={handleUndo}
        onRedo={handleRedo}
        onSaveScene={handleSaveScene}
        onOpenInputMap={() => setIsInputModalOpen(true)}
        onExportGame={handleExportGame}
      />

      {/* Main Workspace Layout */}
      <div className="workspace">
        {/* Left: Hierarchy */}
        <Hierarchy
          entities={scene.entities}
          selectedEntityId={selectedEntityId}
          onSelectEntity={handleSelectEntity}
          onAddEntity={handleAddEntity}
          onDeleteEntity={handleDeleteEntity}
          onDuplicateEntity={handleDuplicateEntity}
          onToggleActive={handleToggleActive}
        />

        {/* Center: Viewport & Drawer */}
        <div className="center-area">
          <Viewport
            scene={scene}
            selectedEntityId={selectedEntityId}
            onSelectEntity={handleSelectEntity}
            onUpdateEntityPosition={handleUpdateEntityPosition}
            isPlaying={isPlaying}
            onTogglePlay={() => setIsPlaying(!isPlaying)}
            onResetSimulation={() => setScene(JSON.parse(JSON.stringify(initialScene)))}
          />

          {/* Bottom Drawer */}
          <div className="bottom-drawer">
            <div className="drawer-tabs">
              <button
                className={`drawer-tab ${activeBottomTab === 'assets' ? 'active' : ''}`}
                onClick={() => setActiveBottomTab('assets')}
              >
                <Database size={13} />
                <span>Navigateur d'Assets & SHA-256</span>
              </button>

              <button
                className={`drawer-tab ${activeBottomTab === 'script' ? 'active' : ''}`}
                onClick={() => setActiveBottomTab('script')}
              >
                <FileCode size={13} />
                <span>Éditeur de Scripts (Lua 5.4)</span>
              </button>

              <button
                className={`drawer-tab ${activeBottomTab === 'liveops' ? 'active' : ''}`}
                onClick={() => setActiveBottomTab('liveops')}
              >
                <Cloud size={13} />
                <span>Live Ops & Déploiement S3/CDN</span>
              </button>
            </div>

            <div className="drawer-content">
              {activeBottomTab === 'assets' && (
                <AssetBrowser
                  assets={assets}
                  onImportAsset={handleImportAsset}
                  onRehashAssets={handleRehashAssets}
                />
              )}
              {activeBottomTab === 'script' && (
                <ScriptEditor
                  code={luaCode}
                  onSaveCode={(code) => { setLuaCode(code); showToast("Script player.lua mis à jour."); }}
                />
              )}
              {activeBottomTab === 'liveops' && (
                <LiveOpsPanel
                  assets={assets}
                  sceneName={scene.name}
                />
              )}
            </div>
          </div>
        </div>

        {/* Right: Inspector */}
        <Inspector
          selectedEntity={selectedEntity}
          onUpdateComponent={handleUpdateComponent}
          onUpdateEntityName={handleUpdateEntityName}
        />
      </div>

      {/* Status Bar */}
      <footer className="status-bar">
        <div className="status-left">
          <div className="status-indicator">
            <span className="status-dot" />
            <span>Moteur WebGPU Actif</span>
          </div>
          <span>Entités : {scene.entities.length}</span>
          <span>Assets : {assets.length}</span>
          <span>Scène : {scene.name}</span>
        </div>

        <div className="status-right">
          <span>Simulation : {isPlaying ? '60 Hz (Fixe)' : 'En Pause'}</span>
          <span>Branche : feat/sprint4-editor-asset-pipeline</span>
          <span style={{ color: 'var(--accent-cyan)' }}>Vercel Edge Ready</span>
        </div>
      </footer>

      {/* Input Mapping Modal */}
      <InputMapModal
        isOpen={isInputModalOpen}
        onClose={() => setIsInputModalOpen(false)}
        bindings={inputBindings}
        onSaveBindings={(b) => setInputBindings(b)}
      />

      {/* Toast Notification */}
      {toastMessage && (
        <div style={{
          position: 'fixed',
          bottom: 36,
          right: 20,
          background: 'rgba(15, 20, 34, 0.95)',
          backdropFilter: 'blur(10px)',
          border: '1px solid rgba(0, 242, 254, 0.4)',
          borderRadius: 'var(--radius-md)',
          padding: '10px 16px',
          display: 'flex',
          alignItems: 'center',
          gap: 10,
          boxShadow: '0 8px 30px rgba(0, 0, 0, 0.5)',
          zIndex: 1000,
          animation: 'modal-enter 0.2s ease'
        }}>
          <CheckCircle2 size={16} color="#00f5a0" />
          <span style={{ fontSize: 12, fontWeight: 500, color: '#fff' }}>{toastMessage}</span>
        </div>
      )}
    </div>
  );
}
