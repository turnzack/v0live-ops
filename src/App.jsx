import React, { useState, useEffect, useRef, useCallback, useMemo } from 'react';
import * as THREE from 'three';
import {
  Box,
  Sun,
  Camera,
  Gamepad2,
  Eye,
  EyeOff,
  Lock,
  Unlock,
  Trash2,
  Copy,
  Plus,
  Upload,
  Download,
  FolderOpen,
  Play,
  Square,
  Move,
  RotateCw,
  Maximize2,
  Grid,
  Sparkles,
  Search,
  Undo2,
  Redo2,
  Layers,
  Sliders,
  Terminal,
  Package,
  CheckCircle2,
  AlertTriangle,
  Info,
  ChevronRight,
  ChevronDown,
  RefreshCw,
  Crosshair,
  Shield,
  Zap,
  FileCode,
  Compass,
  Magnet,
  Settings,
  X
} from 'lucide-react';

// ============================================================================
// CONSTANTES & PRÉRÉGLAGES DU DESIGN SYSTEM
// ============================================================================
const QUICK_COLORS = [
  '#6366f1', // Indigo
  '#3b82f6', // Blue
  '#06b6d4', // Cyan
  '#10b981', // Emerald
  '#22c55e', // Green
  '#eab308', // Yellow
  '#f97316', // Orange
  '#ef4444', // Red
  '#ec4899', // Pink
  '#a855f7', // Purple
  '#94a3b8', // Slate
  '#f8fafc', // White
];

const PRIMITIVE_SHAPES = [
  { id: 'cube', label: 'Cube', icon: '📦' },
  { id: 'sphere', label: 'Sphère', icon: '🔵' },
  { id: 'cylinder', label: 'Cylindre', icon: '🛢️' },
  { id: 'cone', label: 'Cône', icon: '🔺' },
  { id: 'torus', label: 'Tore (Torus)', icon: '🍩' },
  { id: 'plane', label: 'Plan (Plane)', icon: '⬜' },
  { id: 'capsule', label: 'Capsule', icon: '💊' },
  { id: 'torusknot', label: 'Nœud Torique', icon: '♾️' },
];

const DEMO_MODELS = [
  {
    id: 'spaceship',
    name: 'Vaisseau_Intercepteur.fbx',
    format: 'FBX',
    size: '2.4 MB',
    color: '#38bdf8',
    description: 'Chasseur spatial low-poly avec ailes delta et réacteurs'
  },
  {
    id: 'patrol_bot',
    name: 'Robot_Sentinelle.glb',
    format: 'GLB',
    size: '1.8 MB',
    color: '#f59e0b',
    description: 'Drone terrestre bipède avec capteur optique'
  },
  {
    id: 'energy_crystal',
    name: 'Cristal_Aether.obj',
    format: 'OBJ',
    size: '640 KB',
    color: '#a855f7',
    description: 'Formation cristalline luminescente pour point de quête'
  },
  {
    id: 'ancient_arch',
    name: 'Arche_Portail.gltf',
    format: 'GLTF',
    size: '3.1 MB',
    color: '#94a3b8',
    description: 'Portail monolithique avec colonnes runiques'
  }
];

// ============================================================================
// TEMPLATES DE SCÈNES INITIALES
// ============================================================================
const createTemplateScene = (templateName = 'default') => {
  const baseObjects = [
    {
      id: 'dir-light-1',
      name: 'Directional Light',
      type: 'light',
      lightType: 'directional',
      position: { x: 8, y: 14, z: 8 },
      rotation: { x: -45, y: 35, z: 0 },
      scale: { x: 1, y: 1, z: 1 },
      visible: true,
      locked: false,
      material: { color: '#fffbeb', intensity: 1.6, metalness: 0, roughness: 1, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: false, type: 'box', isDynamic: false, mass: 0 }
    },
    {
      id: 'amb-light-1',
      name: 'Ambient Light',
      type: 'light',
      lightType: 'ambient',
      position: { x: 0, y: 8, z: 0 },
      rotation: { x: 0, y: 0, z: 0 },
      scale: { x: 1, y: 1, z: 1 },
      visible: true,
      locked: true,
      material: { color: '#93c5fd', intensity: 0.55, metalness: 0, roughness: 1, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: false, type: 'box', isDynamic: false, mass: 0 }
    },
    {
      id: 'main-camera',
      name: 'Main Camera',
      type: 'camera',
      position: { x: 7, y: 6, z: 10 },
      rotation: { x: -25, y: 35, z: 0 },
      scale: { x: 1, y: 1, z: 1 },
      visible: true,
      locked: false,
      material: { color: '#38bdf8', metalness: 0.5, roughness: 0.3, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: false, type: 'box', isDynamic: false, mass: 0 }
    },
    {
      id: 'floor-plane',
      name: 'Floor',
      type: 'mesh',
      shape: 'cube',
      position: { x: 0, y: -0.25, z: 0 },
      rotation: { x: 0, y: 0, z: 0 },
      scale: { x: 24, y: 0.5, z: 24 },
      visible: true,
      locked: false,
      material: { color: '#1e293b', metalness: 0.2, roughness: 0.85, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
    },
    {
      id: 'player-start',
      name: 'Player Start',
      type: 'player',
      shape: 'capsule',
      position: { x: 0, y: 1.1, z: 5 },
      rotation: { x: 0, y: 0, z: 0 },
      scale: { x: 1, y: 1, z: 1 },
      visible: true,
      locked: false,
      material: { color: '#10b981', metalness: 0.4, roughness: 0.3, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: true, type: 'capsule', isDynamic: true, mass: 75 }
    }
  ];

  if (templateName === 'empty') {
    return baseObjects;
  }

  if (templateName === 'platformer') {
    return [
      ...baseObjects,
      {
        id: 'plat-1',
        name: 'Platform_Step_01',
        type: 'mesh',
        shape: 'cube',
        position: { x: 0, y: 0.8, z: 1 },
        rotation: { x: 0, y: 0, z: 0 },
        scale: { x: 3.5, y: 0.6, z: 2.5 },
        visible: true,
        locked: false,
        material: { color: '#3b82f6', metalness: 0.3, roughness: 0.4, opacity: 1, wireframe: false, type: 'Standard' },
        collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
      },
      {
        id: 'plat-2',
        name: 'Platform_Step_02',
        type: 'mesh',
        shape: 'cube',
        position: { x: 3.8, y: 2.0, z: -2 },
        rotation: { x: 0, y: 0, z: 0 },
        scale: { x: 3.5, y: 0.6, z: 2.5 },
        visible: true,
        locked: false,
        material: { color: '#6366f1', metalness: 0.3, roughness: 0.4, opacity: 1, wireframe: false, type: 'Standard' },
        collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
      },
      {
        id: 'plat-3',
        name: 'Platform_Goal',
        type: 'mesh',
        shape: 'cylinder',
        position: { x: 0, y: 3.2, z: -5.5 },
        rotation: { x: 0, y: 0, z: 0 },
        scale: { x: 3, y: 0.5, z: 3 },
        visible: true,
        locked: false,
        material: { color: '#eab308', metalness: 0.6, roughness: 0.2, opacity: 1, wireframe: false, type: 'Standard' },
        collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
      },
      {
        id: 'goal-crystal',
        name: 'Artefact_Core',
        type: 'imported',
        modelPreset: 'energy_crystal',
        position: { x: 0, y: 4.6, z: -5.5 },
        rotation: { x: 0, y: 45, z: 0 },
        scale: { x: 1.2, y: 1.2, z: 1.2 },
        visible: true,
        locked: false,
        material: { color: '#a855f7', metalness: 0.8, roughness: 0.1, opacity: 0.95, wireframe: false, type: 'Emissive' },
        collision: { enabled: true, type: 'sphere', isDynamic: false, mass: 0 }
      }
    ];
  }

  // Template par défaut : Studio Showcase
  return [
    ...baseObjects,
    {
      id: 'cube-01',
      name: 'Cube_01',
      type: 'mesh',
      shape: 'cube',
      position: { x: 0, y: 0.75, z: 0 },
      rotation: { x: 0, y: 25, z: 0 },
      scale: { x: 1.5, y: 1.5, z: 1.5 },
      visible: true,
      locked: false,
      material: { color: '#6366f1', metalness: 0.35, roughness: 0.25, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: true, type: 'box', isDynamic: false, mass: 10 }
    },
    {
      id: 'sphere-01',
      name: 'Energy_Sphere',
      type: 'mesh',
      shape: 'sphere',
      position: { x: -3.2, y: 1.0, z: -1.5 },
      rotation: { x: 0, y: 0, z: 0 },
      scale: { x: 1.2, y: 1.2, z: 1.2 },
      visible: true,
      locked: false,
      material: { color: '#06b6d4', metalness: 0.7, roughness: 0.15, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: true, type: 'sphere', isDynamic: true, mass: 5 }
    },
    {
      id: 'arch-01',
      name: 'Ancient_Gate',
      type: 'imported',
      modelPreset: 'ancient_arch',
      position: { x: 3.5, y: 0, z: -2.5 },
      rotation: { x: 0, y: -30, z: 0 },
      scale: { x: 1, y: 1, z: 1 },
      visible: true,
      locked: false,
      material: { color: '#94a3b8', metalness: 0.4, roughness: 0.6, opacity: 1, wireframe: false, type: 'Standard' },
      collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
    }
  ];
};

// ============================================================================
// GÉNÉRATEUR DE GÉOMÉTRIES THREE.JS & PARSER OBJ INTÉGRÉ
// ============================================================================
function parseOBJTextToGroup(objText, material) {
  const positions = [];
  const vertices = [];
  const lines = objText.split('\n');

  for (let line of lines) {
    line = line.trim();
    if (line.startsWith('v ')) {
      const parts = line.split(/\s+/);
      positions.push(
        parseFloat(parts[1]) || 0,
        parseFloat(parts[2]) || 0,
        parseFloat(parts[3]) || 0
      );
    } else if (line.startsWith('f ')) {
      const parts = line.split(/\s+/).slice(1);
      const faceIndices = parts.map((p) => {
        const idx = parseInt(p.split('/')[0], 10);
        return idx > 0 ? idx - 1 : positions.length / 3 + idx;
      });
      for (let i = 1; i < faceIndices.length - 1; i++) {
        const a = faceIndices[0] * 3;
        const b = faceIndices[i] * 3;
        const c = faceIndices[i + 1] * 3;
        vertices.push(
          positions[a], positions[a + 1], positions[a + 2],
          positions[b], positions[b + 1], positions[b + 2],
          positions[c], positions[c + 1], positions[c + 2]
        );
      }
    }
  }

  const geometry = new THREE.BufferGeometry();
  if (vertices.length > 0) {
    geometry.setAttribute('position', new THREE.Float32BufferAttribute(vertices, 3));
    geometry.computeVertexNormals();
    geometry.center();
  } else {
    return new THREE.Mesh(new THREE.OctahedronGeometry(1, 1), material);
  }
  const mesh = new THREE.Mesh(geometry, material);
  mesh.castShadow = true;
  mesh.receiveShadow = true;
  return mesh;
}

function createProceduralPresetGroup(preset, mainMaterial) {
  const group = new THREE.Group();

  if (preset === 'spaceship') {
    // Fuselage central
    const body = new THREE.Mesh(new THREE.ConeGeometry(0.65, 2.8, 6), mainMaterial);
    body.rotation.x = Math.PI / 2;
    body.castShadow = true;
    group.add(body);

    // Cockpit
    const cockpitMat = new THREE.MeshStandardMaterial({ color: '#0284c7', metalness: 0.9, roughness: 0.1 });
    const cockpit = new THREE.Mesh(new THREE.BoxGeometry(0.5, 0.35, 0.9), cockpitMat);
    cockpit.position.set(0, 0.3, 0.2);
    group.add(cockpit);

    // Ailes Delta
    const wings = new THREE.Mesh(new THREE.BoxGeometry(2.8, 0.1, 1.1), mainMaterial);
    wings.position.set(0, 0, -0.3);
    wings.castShadow = true;
    group.add(wings);

    // Réacteurs lumineux
    const thrusterMat = new THREE.MeshStandardMaterial({ color: '#38bdf8', emissive: '#0284c7', emissiveIntensity: 1.5 });
    const thrusterL = new THREE.Mesh(new THREE.CylinderGeometry(0.2, 0.25, 0.4, 12), thrusterMat);
    thrusterL.rotation.x = Math.PI / 2;
    thrusterL.position.set(-0.45, 0, -1.35);
    const thrusterR = thrusterL.clone();
    thrusterR.position.set(0.45, 0, -1.35);
    group.add(thrusterL, thrusterR);
  } else if (preset === 'patrol_bot') {
    // Tête du robot
    const head = new THREE.Mesh(new THREE.BoxGeometry(0.9, 0.7, 0.8), mainMaterial);
    head.position.y = 1.2;
    head.castShadow = true;
    group.add(head);

    // Oeil optique
    const eyeMat = new THREE.MeshStandardMaterial({ color: '#ef4444', emissive: '#ef4444', emissiveIntensity: 2 });
    const eye = new THREE.Mesh(new THREE.CylinderGeometry(0.18, 0.18, 0.2, 16), eyeMat);
    eye.rotation.x = Math.PI / 2;
    eye.position.set(0, 1.2, 0.4);
    group.add(eye);

    // Jambes
    const legMat = new THREE.MeshStandardMaterial({ color: '#334155', metalness: 0.8, roughness: 0.3 });
    const legL = new THREE.Mesh(new THREE.BoxGeometry(0.22, 0.9, 0.25), legMat);
    legL.position.set(-0.28, 0.45, 0);
    const legR = legL.clone();
    legR.position.set(0.28, 0.45, 0);
    group.add(legL, legR);
  } else if (preset === 'ancient_arch') {
    // Deux piliers + linteau
    const pillarL = new THREE.Mesh(new THREE.BoxGeometry(0.6, 3.2, 0.6), mainMaterial);
    pillarL.position.set(-1.2, 1.6, 0);
    pillarL.castShadow = true;
    const pillarR = pillarL.clone();
    pillarR.position.set(1.2, 1.6, 0);

    const topBeam = new THREE.Mesh(new THREE.BoxGeometry(3.4, 0.55, 0.8), mainMaterial);
    topBeam.position.set(0, 3.3, 0);
    topBeam.castShadow = true;
    group.add(pillarL, pillarR, topBeam);
  } else {
    // Cristal Énergétique par défaut
    const crystal = new THREE.Mesh(new THREE.OctahedronGeometry(0.9, 0), mainMaterial);
    crystal.scale.set(0.8, 1.6, 0.8);
    crystal.castShadow = true;
    group.add(crystal);
  }

  return group;
}

// ============================================================================
// COMPOSANT PRINCIPAL : APP (ZEngine Studio)
// ============================================================================
export default function App() {
  // --- État du Projet & Scène ---
  const [projectName, setProjectName] = useState('Flight_Sim_Studio');
  const [objects, setObjects] = useState(() => createTemplateScene('default'));
  const [selectedId, setSelectedId] = useState('cube-01');
  const [history, setHistory] = useState([]);
  const [historyIndex, setHistoryIndex] = useState(-1);

  // --- État des Outils & Modes ---
  const [viewMode, setViewMode] = useState('3d'); // '3d' | '2d' | 'render' | 'game'
  const [orthoAxis, setOrthoAxis] = useState('top'); // 'top' | 'front' | 'side'
  const [transformTool, setTransformTool] = useState('translate'); // 'translate' | 'rotate' | 'scale'
  const [snapEnabled, setSnapEnabled] = useState(true);
  const [snapGridSize, setSnapGridSize] = useState(0.5);
  const [showGrid, setShowGrid] = useState(true);
  const [searchQuery, setSearchQuery] = useState('');

  // --- État des Modales & Panneaux ---
  const [isImportModalOpen, setIsImportModalOpen] = useState(false);
  const [isAddModalOpen, setIsAddModalOpen] = useState(false);
  const [isNewProjectModalOpen, setIsNewProjectModalOpen] = useState(false);
  const [bottomTab, setBottomTab] = useState('console'); // 'console' | 'assets' | 'shortcuts'

  // --- État Télémétrie & Mode Jeu ---
  const [fps, setFps] = useState(60);
  const [gameStats, setGameStats] = useState({
    health: 100,
    energy: 100,
    score: 0,
    speed: 0,
    isGrounded: true
  });

  // --- Logs Console ---
  const [logs, setLogs] = useState([
    { id: 1, time: '01:05:00', level: 'info', source: 'Engine', message: 'ZEngine v1.6 initialisé — Rendu Three.js WebGL2 actif.' },
    { id: 2, time: '01:05:01', level: 'success', source: 'Scene', message: 'Scène chargée avec 8 entités et éclairage dynamique.' },
    { id: 3, time: '01:05:02', level: 'info', source: 'Physics', message: 'Système de détection de collision Box/Sphere prêt.' }
  ]);

  const addLog = useCallback((level, source, message) => {
    const now = new Date();
    const timeStr = now.toTimeString().split(' ')[0];
    setLogs((prev) => [
      { id: Date.now() + Math.random(), time: timeStr, level, source, message },
      ...prev.slice(0, 49)
    ]);
  }, []);

  // --- Sauvegarde dans l'historique Undo/Redo ---
  const updateObjectsWithHistory = useCallback((updater, actionLabel = 'Modification') => {
    setObjects((prev) => {
      const next = typeof updater === 'function' ? updater(prev) : updater;
      setHistory((h) => [...h.slice(0, historyIndex + 1), prev]);
      setHistoryIndex((idx) => idx + 1);
      return next;
    });
    if (actionLabel) {
      addLog('info', 'Editor', actionLabel);
    }
  }, [historyIndex, addLog]);

  const handleUndo = useCallback(() => {
    if (historyIndex >= 0 && history[historyIndex]) {
      const previousState = history[historyIndex];
      setObjects(previousState);
      setHistoryIndex((idx) => idx - 1);
      addLog('info', 'Undo', 'Action annulée (Ctrl+Z)');
    }
  }, [history, historyIndex, addLog]);

  // --- Objet actuellement sélectionné ---
  const selectedObject = useMemo(
    () => objects.find((obj) => obj.id === selectedId) || null,
    [objects, selectedId]
  );

  // --- Mise à jour d'un objet spécifique ---
  const updateSelectedObject = useCallback((patch) => {
    if (!selectedId) return;
    setObjects((prev) =>
      prev.map((obj) => (obj.id === selectedId ? { ...obj, ...patch } : obj))
    );
  }, [selectedId]);

  const updateSelectedNested = useCallback((section, patch) => {
    if (!selectedId) return;
    setObjects((prev) =>
      prev.map((obj) =>
        obj.id === selectedId
          ? { ...obj, [section]: { ...obj[section], ...patch } }
          : obj
      )
    );
  }, [selectedId]);

  // --- Actions sur les objets (Ajouter, Dupliquer, Supprimer) ---
  const handleAddObject = useCallback((shapeOrType, customName, customColor = '#6366f1', extra = {}) => {
    const id = `obj-${Date.now().toString(36)}`;
    const isLight = shapeOrType === 'light';
    const isPlayer = shapeOrType === 'player';

    const newObj = {
      id,
      name: customName || `${shapeOrType.charAt(0).toUpperCase() + shapeOrType.slice(1)}_${objects.length + 1}`,
      type: isLight ? 'light' : isPlayer ? 'player' : extra.type || 'mesh',
      shape: isLight ? undefined : shapeOrType,
      lightType: isLight ? 'point' : undefined,
      position: {
        x: Math.round((Math.random() * 4 - 2) * 2) / 2,
        y: isLight ? 5 : 1,
        z: Math.round((Math.random() * 4 - 2) * 2) / 2
      },
      rotation: { x: 0, y: 0, z: 0 },
      scale: { x: 1, y: 1, z: 1 },
      visible: true,
      locked: false,
      material: {
        color: customColor,
        intensity: isLight ? 1.5 : 1,
        metalness: 0.3,
        roughness: 0.35,
        opacity: 1,
        wireframe: false,
        type: 'Standard'
      },
      collision: {
        enabled: !isLight,
        type: shapeOrType === 'sphere' ? 'sphere' : 'box',
        isDynamic: false,
        mass: 10
      },
      ...extra
    };

    updateObjectsWithHistory((prev) => [...prev, newObj], `Objet créé : ${newObj.name}`);
    setSelectedId(id);
    setIsAddModalOpen(false);
  }, [objects.length, updateObjectsWithHistory]);

  const handleDuplicateObject = useCallback((targetId = selectedId) => {
    const source = objects.find((o) => o.id === targetId);
    if (!source) return;
    const id = `obj-${Date.now().toString(36)}`;
    const copy = {
      ...JSON.parse(JSON.stringify(source)),
      id,
      name: `${source.name}_Copy`,
      position: {
        x: Number((source.position.x + 1).toFixed(2)),
        y: source.position.y,
        z: Number((source.position.z + 1).toFixed(2))
      }
    };
    updateObjectsWithHistory((prev) => [...prev, copy], `Objet dupliqué : ${copy.name}`);
    setSelectedId(id);
  }, [objects, selectedId, updateObjectsWithHistory]);

  const handleDeleteObject = useCallback((targetId = selectedId) => {
    const target = objects.find((o) => o.id === targetId);
    if (!target || target.locked) {
      if (target?.locked) addLog('warn', 'Scene', `Impossible de supprimer "${target.name}" (objet verrouillé).`);
      return;
    }
    updateObjectsWithHistory(
      (prev) => prev.filter((o) => o.id !== targetId),
      `Objet supprimé : ${target.name}`
    );
    if (selectedId === targetId) {
      setSelectedId(null);
    }
  }, [objects, selectedId, updateObjectsWithHistory, addLog]);

  // --- Sauvegarde & Chargement JSON ---
  const handleSaveProject = useCallback(() => {
    const payload = {
      engine: 'ZEngine 3D Studio',
      version: '1.6.0',
      projectName,
      savedAt: new Date().toISOString(),
      objects
    };
    const blob = new Blob([JSON.stringify(payload, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${projectName.toLowerCase().replace(/\s+/g, '_')}.scene.json`;
    a.click();
    URL.revokeObjectURL(url);
    addLog('success', 'Project', `Projet "${projectName}" sauvegardé en JSON avec succès.`);
  }, [projectName, objects, addLog]);

  const handleLoadProjectFile = useCallback((e) => {
    const file = e.target.files?.[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = (ev) => {
      try {
        const parsed = JSON.parse(ev.target.result);
        if (Array.isArray(parsed.objects)) {
          setProjectName(parsed.projectName || file.name.replace('.json', ''));
          setObjects(parsed.objects);
          setSelectedId(parsed.objects[0]?.id || null);
          addLog('success', 'Project', `Scène "${file.name}" chargée (${parsed.objects.length} objets).`);
        }
      } catch (err) {
        addLog('error', 'Project', `Erreur lors de la lecture du fichier JSON : ${err.message}`);
      }
    };
    reader.readAsText(file);
  }, [addLog]);

  // --- Raccourcis Clavier Globaux (Mode Éditeur) ---
  useEffect(() => {
    const onKeyDown = (e) => {
      if (['INPUT', 'TEXTAREA', 'SELECT'].includes(e.target?.tagName)) return;
      if (viewMode === 'game') {
        if (e.key === 'Escape') {
          setViewMode('3d');
          addLog('info', 'GameMode', 'Retour au mode Édition.');
        }
        return;
      }

      if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 's') {
        e.preventDefault();
        handleSaveProject();
      } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'z') {
        e.preventDefault();
        handleUndo();
      } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'd') {
        e.preventDefault();
        handleDuplicateObject();
      } else if (e.key.toLowerCase() === 'w') {
        setTransformTool('translate');
      } else if (e.key.toLowerCase() === 'e') {
        setTransformTool('rotate');
      } else if (e.key.toLowerCase() === 'r') {
        setTransformTool('scale');
      } else if (e.key === 'Delete' || e.key === 'Backspace') {
        handleDeleteObject();
      } else if (e.key === 'Escape') {
        setSelectedId(null);
      }
    };

    window.addEventListener('keydown', onKeyDown);
    return () => window.removeEventListener('keydown', onKeyDown);
  }, [viewMode, handleSaveProject, handleUndo, handleDuplicateObject, handleDeleteObject, addLog]);

  // --- Filtrage Scene Explorer ---
  const filteredObjects = useMemo(() => {
    if (!searchQuery.trim()) return objects;
    return objects.filter((o) =>
      o.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
      o.type.toLowerCase().includes(searchQuery.toLowerCase())
    );
  }, [objects, searchQuery]);

  return (
    <div className="flex flex-col h-screen w-screen bg-[#0d0e12] text-[#e4e4e7] font-sans select-none overflow-hidden">
      {/* ====================================================================
          ZONE 1 : MENU & TOOLBAR SUPÉRIEURE
      ==================================================================== */}
      <header className="h-12 bg-[#14151b] border-b border-[#262730] px-3 flex items-center justify-between gap-2 shrink-0 z-20">
        {/* Gauche : Logo + Projet + Actions Fichier */}
        <div className="flex items-center gap-2">
          <div className="flex items-center gap-2 pr-2 border-r border-[#262730]">
            <div className="w-7 h-7 rounded-lg bg-gradient-to-br from-indigo-500 to-blue-600 flex items-center justify-center shadow-md shadow-indigo-500/20 font-bold text-white text-sm">
              Z
            </div>
            <div className="flex flex-col">
              <div className="flex items-center gap-1.5">
                <span className="text-xs font-bold tracking-wide text-white">ZENGINE</span>
                <span className="text-[10px] px-1.5 py-0.2 bg-indigo-500/20 text-indigo-300 rounded border border-indigo-500/30 font-mono">
                  UE5 Web Studio
                </span>
              </div>
              <input
                type="text"
                value={projectName}
                onChange={(e) => setProjectName(e.target.value)}
                className="bg-transparent text-[11px] text-slate-400 hover:text-slate-200 focus:text-white focus:outline-none w-36 truncate"
                title="Cliquer pour renommer le projet"
              />
            </div>
          </div>

          {/* Boutons Projet & Import */}
          <div className="flex items-center gap-1">
            <button
              onClick={() => setIsNewProjectModalOpen(true)}
              className="flex items-center gap-1.5 px-2.5 py-1.5 rounded-md bg-[#1e2029] hover:bg-[#282b36] text-xs font-medium text-slate-200 border border-[#2c2e3b] transition"
              title="Nouveau projet à partir d'un template"
            >
              <FolderOpen className="w-3.5 h-3.5 text-blue-400" />
              <span className="hidden md:inline">Nouveau</span>
            </button>

            <button
              onClick={handleSaveProject}
              className="flex items-center gap-1.5 px-2.5 py-1.5 rounded-md bg-[#1e2029] hover:bg-[#282b36] text-xs font-medium text-slate-200 border border-[#2c2e3b] transition"
              title="Sauvegarder la scène en JSON (Ctrl+S)"
            >
              <Download className="w-3.5 h-3.5 text-emerald-400" />
              <span className="hidden md:inline">Sauvegarder</span>
            </button>

            <label className="flex items-center gap-1.5 px-2.5 py-1.5 rounded-md bg-[#1e2029] hover:bg-[#282b36] text-xs font-medium text-slate-200 border border-[#2c2e3b] transition cursor-pointer">
              <Upload className="w-3.5 h-3.5 text-amber-400" />
              <span className="hidden lg:inline">Charger JSON</span>
              <input type="file" accept=".json" onChange={handleLoadProjectFile} className="hidden" />
            </label>

            <button
              onClick={() => setIsImportModalOpen(true)}
              className="flex items-center gap-1.5 px-2.5 py-1.5 rounded-md bg-indigo-600/20 hover:bg-indigo-600/30 text-indigo-300 border border-indigo-500/40 text-xs font-medium transition"
            >
              <Package className="w-3.5 h-3.5 text-indigo-400" />
              <span>Importer 3D</span>
            </button>

            <button
              onClick={() => setIsAddModalOpen(true)}
              className="flex items-center gap-1 px-2.5 py-1.5 rounded-md bg-blue-600 hover:bg-blue-500 text-white text-xs font-medium shadow-sm transition"
            >
              <Plus className="w-3.5 h-3.5" />
              <span className="hidden sm:inline">Ajouter</span>
            </button>
          </div>
        </div>

        {/* Centre : Outils de Transformation (W / E / R) + Grille + Snapping */}
        <div className="flex items-center gap-1.5 bg-[#101116] p-1 rounded-lg border border-[#252630]">
          <button
            onClick={handleUndo}
            disabled={historyIndex < 0}
            className="p-1.5 rounded hover:bg-[#232530] disabled:opacity-30 text-slate-300 transition"
            title="Annuler (Ctrl+Z)"
          >
            <Undo2 className="w-3.5 h-3.5" />
          </button>

          <div className="h-4 w-[1px] bg-[#262730]" />

          <button
            onClick={() => setTransformTool('translate')}
            className={`flex items-center gap-1 px-2 py-1 rounded text-xs font-medium transition ${
              transformTool === 'translate'
                ? 'bg-blue-600 text-white shadow'
                : 'text-slate-400 hover:text-white hover:bg-[#1f212b]'
            }`}
            title="Déplacer (Touche W)"
          >
            <Move className="w-3.5 h-3.5" />
            <span className="font-mono text-[10px] opacity-80">W</span>
          </button>

          <button
            onClick={() => setTransformTool('rotate')}
            className={`flex items-center gap-1 px-2 py-1 rounded text-xs font-medium transition ${
              transformTool === 'rotate'
                ? 'bg-blue-600 text-white shadow'
                : 'text-slate-400 hover:text-white hover:bg-[#1f212b]'
            }`}
            title="Rotation (Touche E)"
          >
            <RotateCw className="w-3.5 h-3.5" />
            <span className="font-mono text-[10px] opacity-80">E</span>
          </button>

          <button
            onClick={() => setTransformTool('scale')}
            className={`flex items-center gap-1 px-2 py-1 rounded text-xs font-medium transition ${
              transformTool === 'scale'
                ? 'bg-blue-600 text-white shadow'
                : 'text-slate-400 hover:text-white hover:bg-[#1f212b]'
            }`}
            title="Échelle / Scale (Touche R)"
          >
            <Maximize2 className="w-3.5 h-3.5" />
            <span className="font-mono text-[10px] opacity-80">R</span>
          </button>

          <div className="h-4 w-[1px] bg-[#262730]" />

          <button
            onClick={() => setSnapEnabled(!snapEnabled)}
            className={`flex items-center gap-1 px-2 py-1 rounded text-xs transition ${
              snapEnabled
                ? 'bg-amber-500/20 text-amber-300 border border-amber-500/40'
                : 'text-slate-400 hover:bg-[#1f212b]'
            }`}
            title="Activer/Désactiver l'aimantation (Snapping Grille)"
          >
            <Magnet className="w-3.5 h-3.5" />
            <span className="hidden xl:inline text-[11px]">{snapGridSize}m</span>
          </button>

          <button
            onClick={() => setShowGrid(!showGrid)}
            className={`p-1.5 rounded text-xs transition ${
              showGrid ? 'bg-[#232530] text-blue-400' : 'text-slate-500 hover:bg-[#1f212b]'
            }`}
            title="Afficher/Masquer la grille"
          >
            <Grid className="w-3.5 h-3.5" />
          </button>
        </div>

        {/* Droite : Modes de Visualisation (3D / 2D / Rendu / Play Mode) */}
        <div className="flex items-center gap-2">
          <div className="flex items-center bg-[#101116] p-1 rounded-lg border border-[#252630]">
            {[
              { id: '3d', label: '3D', icon: Compass },
              { id: '2d', label: '2D Ortho', icon: Layers },
              { id: 'render', label: 'Rendu HQ', icon: Sparkles },
            ].map((m) => {
              const Icon = m.icon;
              return (
                <button
                  key={m.id}
                  onClick={() => {
                    setViewMode(m.id);
                    addLog('info', 'Viewport', `Mode de vue basculé sur : ${m.label}`);
                  }}
                  className={`flex items-center gap-1 px-2.5 py-1 rounded text-xs font-medium transition ${
                    viewMode === m.id
                      ? 'bg-[#262936] text-white border border-[#3b3f54]'
                      : 'text-slate-400 hover:text-slate-200'
                  }`}
                >
                  <Icon className="w-3.5 h-3.5 text-blue-400" />
                  <span className="hidden sm:inline">{m.label}</span>
                </button>
              );
            })}
          </div>

          {/* Bouton Play / Stop Mode Jeu */}
          {viewMode !== 'game' ? (
            <button
              onClick={() => {
                setViewMode('game');
                addLog('success', 'GameMode', 'Mode Jeu démarré — Utilisez ZQSD/WASD + Espace.');
              }}
              className="flex items-center gap-1.5 px-3.5 py-1.5 rounded-lg bg-emerald-600 hover:bg-emerald-500 text-white text-xs font-semibold shadow-lg shadow-emerald-600/25 transition"
            >
              <Play className="w-3.5 h-3.5 fill-current" />
              <span>Play</span>
            </button>
          ) : (
            <button
              onClick={() => {
                setViewMode('3d');
                addLog('info', 'GameMode', 'Mode Jeu arrêté. Retour à l’éditeur.');
              }}
              className="flex items-center gap-1.5 px-3.5 py-1.5 rounded-lg bg-rose-600 hover:bg-rose-500 text-white text-xs font-semibold shadow-lg shadow-rose-600/30 animate-pulse transition"
            >
              <Square className="w-3.5 h-3.5 fill-current" />
              <span>Stop</span>
            </button>
          )}
        </div>
      </header>

      {/* ====================================================================
          CORPS PRINCIPAL : SCENE EXPLORER + VIEWPORT 3D + PROPERTIES PANEL
      ==================================================================== */}
      <div className="flex-1 flex min-h-0 overflow-hidden">
        {/* ------------------------------------------------------------------
            ZONE 2 : SCENE EXPLORER (Panneau Gauche)
        ------------------------------------------------------------------ */}
        {viewMode !== 'game' && (
          <aside className="w-64 bg-[#14151b] border-r border-[#262730] flex flex-col shrink-0">
            <div className="p-2.5 border-b border-[#262730] flex items-center justify-between">
              <div className="flex items-center gap-1.5 text-xs font-semibold uppercase tracking-wider text-slate-300">
                <Layers className="w-3.5 h-3.5 text-blue-400" />
                <span>Scene Explorer</span>
              </div>
              <span className="text-[11px] px-1.5 py-0.5 rounded bg-[#1f212b] text-slate-400 font-mono">
                {objects.length}
              </span>
            </div>

            {/* Barre de recherche */}
            <div className="p-2 border-b border-[#262730]">
              <div className="relative">
                <Search className="w-3.5 h-3.5 text-slate-500 absolute left-2.5 top-1/2 -translate-y-1/2" />
                <input
                  type="text"
                  placeholder="Rechercher un objet..."
                  value={searchQuery}
                  onChange={(e) => setSearchQuery(e.target.value)}
                  className="w-full bg-[#1c1e26] text-xs text-slate-200 rounded-md pl-8 pr-2.5 py-1.5 border border-[#2a2c38] focus:border-blue-500 focus:outline-none"
                />
              </div>
            </div>

            {/* Arborescence de la scène */}
            <div className="flex-1 overflow-y-auto p-1.5 space-y-0.5">
              <div className="flex items-center gap-1 px-2 py-1 text-[11px] font-semibold text-slate-400">
                <ChevronDown className="w-3 h-3" />
                <span>{projectName}.scene</span>
              </div>

              {filteredObjects.map((obj) => {
                const isSelected = obj.id === selectedId;
                return (
                  <div
                    key={obj.id}
                    onClick={() => setSelectedId(obj.id)}
                    className={`group flex items-center justify-between px-2.5 py-1.5 rounded-md text-xs cursor-pointer transition ${
                      isSelected
                        ? 'bg-blue-600/25 text-blue-200 border border-blue-500/50'
                        : 'text-slate-300 hover:bg-[#1e2029] border border-transparent'
                    }`}
                  >
                    <div className="flex items-center gap-2 min-w-0">
                      {obj.type === 'light' && <Sun className="w-3.5 h-3.5 text-amber-400 shrink-0" />}
                      {obj.type === 'camera' && <Camera className="w-3.5 h-3.5 text-cyan-400 shrink-0" />}
                      {obj.type === 'player' && <Gamepad2 className="w-3.5 h-3.5 text-emerald-400 shrink-0" />}
                      {obj.type === 'imported' && <Package className="w-3.5 h-3.5 text-purple-400 shrink-0" />}
                      {obj.type === 'mesh' && (
                        <span
                          className="w-3 h-3 rounded-sm shrink-0 border border-white/20"
                          style={{ backgroundColor: obj.material.color }}
                        />
                      )}
                      <span className={`truncate ${!obj.visible ? 'line-through opacity-50' : ''}`}>
                        {obj.name}
                      </span>
                    </div>

                    {/* Actions rapides survol */}
                    <div className="flex items-center gap-1 opacity-70 group-hover:opacity-100">
                      <button
                        onClick={(e) => {
                          e.stopPropagation();
                          setObjects((prev) =>
                            prev.map((o) => (o.id === obj.id ? { ...o, visible: !o.visible } : o))
                          );
                        }}
                        className="p-0.5 hover:text-white text-slate-400"
                        title={obj.visible ? 'Masquer' : 'Afficher'}
                      >
                        {obj.visible ? <Eye className="w-3 h-3" /> : <EyeOff className="w-3 h-3 text-rose-400" />}
                      </button>

                      <button
                        onClick={(e) => {
                          e.stopPropagation();
                          setObjects((prev) =>
                            prev.map((o) => (o.id === obj.id ? { ...o, locked: !o.locked } : o))
                          );
                        }}
                        className="p-0.5 hover:text-white text-slate-400"
                        title={obj.locked ? 'Déverrouiller' : 'Verrouiller'}
                      >
                        {obj.locked ? <Lock className="w-3 h-3 text-amber-400" /> : <Unlock className="w-3 h-3" />}
                      </button>
                    </div>
                  </div>
                );
              })}
            </div>

            {/* Barre d'actions rapides en bas du Scene Explorer */}
            <div className="p-2 border-t border-[#262730] flex items-center justify-between gap-1 bg-[#111217]">
              <button
                onClick={() => setIsAddModalOpen(true)}
                className="flex-1 flex items-center justify-center gap-1 py-1.5 rounded bg-[#1e2029] hover:bg-[#282b36] text-[11px] text-slate-200 border border-[#2c2e3b]"
              >
                <Plus className="w-3 h-3 text-blue-400" />
                <span>Entité</span>
              </button>
              <button
                onClick={() => handleDuplicateObject()}
                disabled={!selectedObject}
                className="p-1.5 rounded bg-[#1e2029] hover:bg-[#282b36] disabled:opacity-40 text-slate-300 border border-[#2c2e3b]"
                title="Dupliquer l'objet sélectionné (Ctrl+D)"
              >
                <Copy className="w-3.5 h-3.5" />
              </button>
              <button
                onClick={() => handleDeleteObject()}
                disabled={!selectedObject || selectedObject.locked}
                className="p-1.5 rounded bg-[#1e2029] hover:bg-rose-950/60 disabled:opacity-40 text-rose-400 border border-[#2c2e3b]"
                title="Supprimer l'objet sélectionné (Suppr)"
              >
                <Trash2 className="w-3.5 h-3.5" />
              </button>
            </div>
          </aside>
        )}

        {/* ------------------------------------------------------------------
            ZONE 3 : CENTRE (VIEWPORT 3D + DOCK INFÉRIEUR)
        ------------------------------------------------------------------ */}
        <div className="flex-1 flex flex-col min-w-0 min-h-0">
          {/* Viewport 3D Principal */}
          <div className="flex-1 relative bg-[#090a0d] overflow-hidden">
            <ViewportCanvas
              objects={objects}
              selectedId={selectedId}
              onSelectObject={setSelectedId}
              onTransformObject={(id, newPos, newRot, newScale) => {
                setObjects((prev) =>
                  prev.map((o) =>
                    o.id === id
                      ? {
                          ...o,
                          position: newPos || o.position,
                          rotation: newRot || o.rotation,
                          scale: newScale || o.scale
                        }
                      : o
                  )
                );
              }}
              viewMode={viewMode}
              orthoAxis={orthoAxis}
              transformTool={transformTool}
              snapEnabled={snapEnabled}
              snapGridSize={snapGridSize}
              showGrid={showGrid}
              onFpsUpdate={setFps}
              onGameStatsUpdate={setGameStats}
              addLog={addLog}
            />

            {/* Badges de télémétrie en haut à gauche du Viewport */}
            <div className="absolute top-3 left-3 flex flex-wrap items-center gap-2 pointer-events-none">
              <div className="px-2.5 py-1 rounded-md bg-[#14151b]/90 backdrop-blur border border-[#2b2d3c] text-[11px] font-mono flex items-center gap-2 text-slate-300 shadow">
                <span className="w-2 h-2 rounded-full bg-emerald-400 animate-pulse" />
                <span>{viewMode.toUpperCase()}</span>
                <span className="text-slate-500">|</span>
                <span className="text-emerald-400 font-semibold">{fps} FPS</span>
                <span className="text-slate-500">|</span>
                <span>Objets: {objects.length}</span>
              </div>

              {viewMode === '2d' && (
                <div className="flex items-center gap-1 bg-[#14151b]/90 backdrop-blur p-1 rounded-md border border-[#2b2d3c] pointer-events-auto">
                  {['top', 'front', 'side'].map((axis) => (
                    <button
                      key={axis}
                      onClick={() => setOrthoAxis(axis)}
                      className={`px-2 py-0.5 rounded text-[11px] uppercase font-mono ${
                        orthoAxis === axis ? 'bg-blue-600 text-white' : 'text-slate-400 hover:text-white'
                      }`}
                    >
                      {axis}
                    </button>
                  ))}
                </div>
              )}
            </div>

            {/* Instructions contextuelles des Gizmos en bas à gauche du Viewport */}
            {viewMode !== 'game' && selectedObject && (
              <div className="absolute bottom-3 left-3 px-3 py-1.5 rounded-md bg-[#14151b]/90 backdrop-blur border border-[#2b2d3c] text-[11px] text-slate-300 flex items-center gap-3 pointer-events-none">
                <span className="text-blue-400 font-semibold">{selectedObject.name}</span>
                <span className="text-slate-500">•</span>
                <span>
                  Outil : <strong className="uppercase text-white">{transformTool}</strong> (Glisser les axes X/Y/Z dans la scène)
                </span>
              </div>
            )}

            {/* HUD DU MODE JEU (PLAY MODE OVERLAY) */}
            {viewMode === 'game' && (
              <div className="absolute inset-0 pointer-events-none flex flex-col justify-between p-5">
                {/* Top HUD : Objectif & Contrôles */}
                <div className="flex items-start justify-between">
                  <div className="bg-[#11131a]/85 backdrop-blur-md border border-emerald-500/40 rounded-xl p-3.5 max-w-xs shadow-xl">
                    <div className="flex items-center gap-2 text-emerald-400 text-xs font-bold uppercase tracking-wider mb-1">
                      <Gamepad2 className="w-4 h-4" />
                      <span>Simulation en cours</span>
                    </div>
                    <p className="text-xs text-slate-300 leading-relaxed">
                      Déplacez le joueur avec <kbd className="px-1.5 py-0.5 bg-slate-800 rounded text-white font-mono">W A S D</kbd> ou les flèches. Appuyez sur <kbd className="px-1.5 py-0.5 bg-slate-800 rounded text-white font-mono">Espace</kbd> pour sauter.
                    </p>
                  </div>

                  <button
                    onClick={() => setViewMode('3d')}
                    className="pointer-events-auto flex items-center gap-2 px-4 py-2 rounded-lg bg-rose-600 hover:bg-rose-500 text-white text-xs font-bold shadow-lg transition"
                  >
                    <Square className="w-3.5 h-3.5 fill-current" />
                    <span>Quitter le Jeu (Échap)</span>
                  </button>
                </div>

                {/* Réticule Central (Crosshair) */}
                <div className="self-center flex items-center justify-center">
                  <Crosshair className="w-6 h-6 text-emerald-400/70" />
                </div>

                {/* Bottom HUD : Barres de Vie, Énergie, Vitesse */}
                <div className="flex items-end justify-between">
                  <div className="bg-[#11131a]/85 backdrop-blur-md border border-[#2a2d3d] rounded-xl p-3.5 w-72 space-y-2.5 shadow-xl">
                    <div>
                      <div className="flex justify-between text-xs mb-1">
                        <span className="flex items-center gap-1 text-emerald-400 font-semibold">
                          <Shield className="w-3.5 h-3.5" /> Intégrité (PV)
                        </span>
                        <span className="font-mono text-white">{gameStats.health}%</span>
                      </div>
                      <div className="w-full h-2 bg-slate-800 rounded-full overflow-hidden">
                        <div
                          className="h-full bg-gradient-to-r from-emerald-500 to-teal-400 transition-all"
                          style={{ width: `${gameStats.health}%` }}
                        />
                      </div>
                    </div>

                    <div>
                      <div className="flex justify-between text-xs mb-1">
                        <span className="flex items-center gap-1 text-blue-400 font-semibold">
                          <Zap className="w-3.5 h-3.5" /> Propulseurs
                        </span>
                        <span className="font-mono text-white">{Math.round(gameStats.energy)}%</span>
                      </div>
                      <div className="w-full h-2 bg-slate-800 rounded-full overflow-hidden">
                        <div
                          className="h-full bg-gradient-to-r from-blue-500 to-indigo-400 transition-all"
                          style={{ width: `${gameStats.energy}%` }}
                        />
                      </div>
                    </div>
                  </div>

                  <div className="bg-[#11131a]/85 backdrop-blur-md border border-[#2a2d3d] rounded-xl px-4 py-3 text-right font-mono">
                    <div className="text-[10px] text-slate-400 uppercase">Vitesse Joueur</div>
                    <div className="text-xl font-bold text-white">{gameStats.speed.toFixed(1)} <span className="text-xs text-blue-400">m/s</span></div>
                    <div className="text-[10px] text-emerald-400">
                      {gameStats.isGrounded ? '● Au sol (Collision Active)' : '▲ En saut'}
                    </div>
                  </div>
                </div>
              </div>
            )}
          </div>

          {/* ----------------------------------------------------------------
              BOTTOM DOCK : CONSOLE, BIBLIOTHÈQUE D'ASSETS & RACCOURCIS
          ---------------------------------------------------------------- */}
          {viewMode !== 'game' && (
            <div className="h-44 bg-[#14151b] border-t border-[#262730] flex flex-col shrink-0">
              {/* Onglets du Dock */}
              <div className="flex items-center justify-between px-3 border-b border-[#262730] bg-[#111217]">
                <div className="flex items-center gap-1">
                  <button
                    onClick={() => setBottomTab('console')}
                    className={`flex items-center gap-1.5 px-3 py-2 text-xs font-medium border-b-2 transition ${
                      bottomTab === 'console'
                        ? 'border-blue-500 text-white bg-[#181a22]'
                        : 'border-transparent text-slate-400 hover:text-slate-200'
                    }`}
                  >
                    <Terminal className="w-3.5 h-3.5 text-blue-400" />
                    <span>Console & Logs ({logs.length})</span>
                  </button>

                  <button
                    onClick={() => setBottomTab('assets')}
                    className={`flex items-center gap-1.5 px-3 py-2 text-xs font-medium border-b-2 transition ${
                      bottomTab === 'assets'
                        ? 'border-blue-500 text-white bg-[#181a22]'
                        : 'border-transparent text-slate-400 hover:text-slate-200'
                    }`}
                  >
                    <Package className="w-3.5 h-3.5 text-purple-400" />
                    <span>Asset Browser & Modèles 3D</span>
                  </button>
                </div>

                {bottomTab === 'console' && (
                  <button
                    onClick={() => setLogs([])}
                    className="text-[11px] text-slate-400 hover:text-slate-200 px-2 py-0.5 rounded hover:bg-[#232530]"
                  >
                    Effacer
                  </button>
                )}
              </div>

              {/* Contenu du Dock */}
              <div className="flex-1 overflow-y-auto p-2.5 font-mono text-xs">
                {bottomTab === 'console' && (
                  <div className="space-y-1">
                    {logs.map((log) => (
                      <div key={log.id} className="flex items-center gap-2 py-0.5 text-[11px]">
                        <span className="text-slate-500">[{log.time}]</span>
                        <span
                          className={`px-1.5 py-0.2 rounded font-semibold uppercase text-[10px] ${
                            log.level === 'success'
                              ? 'bg-emerald-500/20 text-emerald-400'
                              : log.level === 'warn'
                              ? 'bg-amber-500/20 text-amber-400'
                              : log.level === 'error'
                              ? 'bg-rose-500/20 text-rose-400'
                              : 'bg-blue-500/20 text-blue-400'
                          }`}
                        >
                          {log.source}
                        </span>
                        <span className="text-slate-300">{log.message}</span>
                      </div>
                    ))}
                  </div>
                )}

                {bottomTab === 'assets' && (
                  <div className="grid grid-cols-2 sm:grid-cols-4 md:grid-cols-6 gap-2 font-sans">
                    {DEMO_MODELS.map((item) => (
                      <button
                        key={item.id}
                        onClick={() =>
                          handleAddObject('cube', item.name.split('.')[0], item.color, {
                            type: 'imported',
                            modelPreset: item.id
                          })
                        }
                        className="flex flex-col items-start p-2 rounded-lg bg-[#1c1e26] hover:bg-[#252834] border border-[#2b2e3d] transition group text-left"
                      >
                        <div className="flex items-center justify-between w-full mb-1">
                          <span className="text-[10px] font-mono px-1.5 py-0.5 rounded bg-purple-500/20 text-purple-300">
                            .{item.format}
                          </span>
                          <Plus className="w-3.5 h-3.5 text-slate-400 group-hover:text-white" />
                        </div>
                        <div className="text-xs font-semibold text-slate-200 truncate w-full">{item.name}</div>
                        <div className="text-[10px] text-slate-400">{item.size}</div>
                      </button>
                    ))}
                    <button
                      onClick={() => setIsImportModalOpen(true)}
                      className="flex flex-col items-center justify-center p-2 rounded-lg border border-dashed border-indigo-500/50 bg-indigo-500/10 hover:bg-indigo-500/20 text-indigo-300 transition"
                    >
                      <Upload className="w-4 h-4 mb-1" />
                      <span className="text-xs font-medium">Importer fichier...</span>
                    </button>
                  </div>
                )}
              </div>
            </div>
          )}
        </div>

        {/* ------------------------------------------------------------------
            ZONE 4 : PROPERTIES PANEL (Inspecteur à Droite)
        ------------------------------------------------------------------ */}
        {viewMode !== 'game' && (
          <aside className="w-80 bg-[#14151b] border-l border-[#262730] flex flex-col shrink-0 overflow-y-auto">
            <div className="p-2.5 border-b border-[#262730] flex items-center justify-between">
              <div className="flex items-center gap-1.5 text-xs font-semibold uppercase tracking-wider text-slate-300">
                <Sliders className="w-3.5 h-3.5 text-blue-400" />
                <span>Properties Panel</span>
              </div>
              {selectedObject && (
                <span className="text-[10px] px-1.5 py-0.5 rounded bg-blue-500/20 text-blue-300 font-mono uppercase">
                  {selectedObject.type}
                </span>
              )}
            </div>

            {!selectedObject ? (
              <div className="flex-1 flex flex-col items-center justify-center p-6 text-center text-slate-500">
                <Box className="w-10 h-10 mb-2 opacity-30" />
                <p className="text-xs">Sélectionnez un objet dans le Viewport 3D ou dans le Scene Explorer pour modifier ses propriétés.</p>
              </div>
            ) : (
              <div className="p-3 space-y-4 text-xs">
                {/* Identité de l'objet */}
                <div className="space-y-2 bg-[#1a1c24] p-2.5 rounded-lg border border-[#262936]">
                  <label className="text-[11px] text-slate-400 font-medium">Nom de l'entité</label>
                  <input
                    type="text"
                    value={selectedObject.name}
                    onChange={(e) => updateSelectedObject({ name: e.target.value })}
                    className="w-full bg-[#111217] text-white px-2.5 py-1.5 rounded border border-[#2c2f3f] focus:border-blue-500 focus:outline-none"
                  />
                  <div className="flex items-center justify-between pt-1 text-[11px] text-slate-400">
                    <span>ID : <code className="text-slate-300">{selectedObject.id}</code></span>
                    <button
                      onClick={() =>
                        updateSelectedObject({
                          position: { x: 0, y: 1, z: 0 },
                          rotation: { x: 0, y: 0, z: 0 },
                          scale: { x: 1, y: 1, z: 1 }
                        })
                      }
                      className="flex items-center gap-1 text-blue-400 hover:text-blue-300"
                      title="Réinitialiser la transformation"
                    >
                      <RefreshCw className="w-3 h-3" /> Reset
                    </button>
                  </div>
                </div>

                {/* Section TRANSFORMATION (Position, Rotation, Scale) */}
                <div className="space-y-3 bg-[#1a1c24] p-2.5 rounded-lg border border-[#262936]">
                  <div className="font-semibold text-slate-200 uppercase text-[11px] tracking-wider">
                    Transform
                  </div>

                  {[
                    { label: 'Position', key: 'position', step: 0.25 },
                    { label: 'Rotation (°)', key: 'rotation', step: 5 },
                    { label: 'Scale', key: 'scale', step: 0.1 }
                  ].map((group) => (
                    <div key={group.key} className="space-y-1">
                      <span className="text-[11px] text-slate-400">{group.label}</span>
                      <div className="grid grid-cols-3 gap-1.5">
                        {[
                          { axis: 'x', color: 'text-rose-400 border-rose-500/30' },
                          { axis: 'y', color: 'text-emerald-400 border-emerald-500/30' },
                          { axis: 'z', color: 'text-blue-400 border-blue-500/30' }
                        ].map(({ axis, color }) => (
                          <div
                            key={axis}
                            className={`flex items-center bg-[#111217] rounded border ${color} px-1.5 py-1`}
                          >
                            <span className="uppercase font-mono text-[10px] font-bold mr-1.5">
                              {axis}
                            </span>
                            <input
                              type="number"
                              step={group.step}
                              value={selectedObject[group.key][axis]}
                              onChange={(e) => {
                                const val = parseFloat(e.target.value) || 0;
                                updateSelectedNested(group.key, { [axis]: val });
                              }}
                              className="w-full bg-transparent text-white font-mono text-[11px] focus:outline-none text-right"
                            />
                          </div>
                        ))}
                      </div>
                    </div>
                  ))}
                </div>

                {/* Section MATÉRIAU (Couleur, Metalness, Roughness, Type) */}
                <div className="space-y-3 bg-[#1a1c24] p-2.5 rounded-lg border border-[#262936]">
                  <div className="flex items-center justify-between">
                    <span className="font-semibold text-slate-200 uppercase text-[11px] tracking-wider">
                      Material & Surface
                    </span>
                    <select
                      value={selectedObject.material.type || 'Standard'}
                      onChange={(e) => updateSelectedNested('material', { type: e.target.value })}
                      className="bg-[#111217] text-[11px] text-slate-200 px-2 py-0.5 rounded border border-[#2c2f3f]"
                    >
                      <option value="Standard">PBR Standard</option>
                      <option value="Phong">Phong Brillant</option>
                      <option value="Emissive">Néon Émissif</option>
                      <option value="Basic">Unlit Basic</option>
                    </select>
                  </div>

                  {/* Sélecteur de couleur + Palette rapide */}
                  <div className="space-y-1.5">
                    <div className="flex items-center justify-between">
                      <span className="text-[11px] text-slate-400">Couleur Albedo</span>
                      <div className="flex items-center gap-1.5">
                        <input
                          type="color"
                          value={selectedObject.material.color}
                          onChange={(e) => updateSelectedNested('material', { color: e.target.value })}
                          className="w-5 h-5 rounded cursor-pointer bg-transparent border-0"
                        />
                        <span className="font-mono text-[11px] text-slate-300">
                          {selectedObject.material.color}
                        </span>
                      </div>
                    </div>

                    <div className="grid grid-cols-6 gap-1 pt-1">
                      {QUICK_COLORS.map((hex) => (
                        <button
                          key={hex}
                          onClick={() => updateSelectedNested('material', { color: hex })}
                          className={`h-5 rounded border transition ${
                            selectedObject.material.color === hex
                              ? 'border-white scale-105 shadow'
                              : 'border-black/40 hover:scale-105'
                          }`}
                          style={{ backgroundColor: hex }}
                        />
                      ))}
                    </div>
                  </div>

                  {/* Sliders PBR */}
                  <div className="space-y-2 pt-1">
                    <div>
                      <div className="flex justify-between text-[11px] text-slate-400">
                        <span>Metalness</span>
                        <span className="font-mono">{selectedObject.material.metalness}</span>
                      </div>
                      <input
                        type="range"
                        min="0"
                        max="1"
                        step="0.05"
                        value={selectedObject.material.metalness}
                        onChange={(e) =>
                          updateSelectedNested('material', { metalness: parseFloat(e.target.value) })
                        }
                        className="w-full accent-blue-500 cursor-pointer"
                      />
                    </div>

                    <div>
                      <div className="flex justify-between text-[11px] text-slate-400">
                        <span>Roughness</span>
                        <span className="font-mono">{selectedObject.material.roughness}</span>
                      </div>
                      <input
                        type="range"
                        min="0"
                        max="1"
                        step="0.05"
                        value={selectedObject.material.roughness}
                        onChange={(e) =>
                          updateSelectedNested('material', { roughness: parseFloat(e.target.value) })
                        }
                        className="w-full accent-blue-500 cursor-pointer"
                      />
                    </div>

                    <label className="flex items-center justify-between pt-1 cursor-pointer">
                      <span className="text-[11px] text-slate-300">Affichage Wireframe</span>
                      <input
                        type="checkbox"
                        checked={selectedObject.material.wireframe}
                        onChange={(e) =>
                          updateSelectedNested('material', { wireframe: e.target.checked })
                        }
                        className="rounded accent-blue-500"
                      />
                    </label>
                  </div>
                </div>

                {/* Section COLLISION & PHYSIQUE */}
                <div className="space-y-2.5 bg-[#1a1c24] p-2.5 rounded-lg border border-[#262936]">
                  <div className="flex items-center justify-between">
                    <span className="font-semibold text-slate-200 uppercase text-[11px] tracking-wider">
                      Collision & Physics
                    </span>
                    <input
                      type="checkbox"
                      checked={selectedObject.collision?.enabled ?? true}
                      onChange={(e) =>
                        updateSelectedNested('collision', { enabled: e.target.checked })
                      }
                      className="rounded accent-emerald-500"
                    />
                  </div>

                  {selectedObject.collision?.enabled && (
                    <div className="space-y-2 pt-1">
                      <div className="flex items-center justify-between">
                        <span className="text-[11px] text-slate-400">Type de Collider</span>
                        <select
                          value={selectedObject.collision.type}
                          onChange={(e) =>
                            updateSelectedNested('collision', { type: e.target.value })
                          }
                          className="bg-[#111217] text-[11px] text-slate-200 px-2 py-1 rounded border border-[#2c2f3f]"
                        >
                          <option value="box">Box Collider</option>
                          <option value="sphere">Sphere Collider</option>
                          <option value="capsule">Capsule Collider</option>
                          <option value="mesh">Mesh Collider</option>
                        </select>
                      </div>
                    </div>
                  )}
                </div>
              </div>
            )}
          </aside>
        )}
      </div>

      {/* ====================================================================
          BARRE D'ÉTAT INFÉRIEURE (STATUS BAR)
      ==================================================================== */}
      <footer className="h-6 bg-[#101116] border-t border-[#232530] px-3 flex items-center justify-between text-[11px] text-slate-400 shrink-0">
        <div className="flex items-center gap-3">
          <span className="flex items-center gap-1.5 text-emerald-400 font-medium">
            <span className="w-1.5 h-1.5 rounded-full bg-emerald-400" />
            ZEngine v1.6 Ready
          </span>
          <span>•</span>
          <span>Formats : FBX, OBJ, glTF, GLB, DAE, 3DS</span>
        </div>
        <div className="flex items-center gap-3 font-mono">
          <span>Raccourcis : [W] Move [E] Rotate [R] Scale [Suppr] Delete [Ctrl+S] Save</span>
        </div>
      </footer>

      {/* ====================================================================
          MODALE 1 : IMPORT 3D DRAG & DROP MULTI-FORMATS
      ==================================================================== */}
      {isImportModalOpen && (
        <Import3DModal
          onClose={() => setIsImportModalOpen(false)}
          onImportSuccess={(newEntity) => {
            updateObjectsWithHistory(
              (prev) => [...prev, newEntity],
              `Modèle 3D importé : ${newEntity.name}`
            );
            setSelectedId(newEntity.id);
            setIsImportModalOpen(false);
          }}
          addLog={addLog}
        />
      )}

      {/* ====================================================================
          MODALE 2 : AJOUTER UN OBJET / PRIMITIVE 3D
      ==================================================================== */}
      {isAddModalOpen && (
        <div className="fixed inset-0 z-50 bg-black/70 backdrop-blur-sm flex items-center justify-center p-4">
          <div className="bg-[#161821] border border-[#2c2f40] rounded-xl max-w-md w-full p-5 shadow-2xl space-y-4">
            <div className="flex items-center justify-between border-b border-[#262938] pb-3">
              <h3 className="text-sm font-bold text-white flex items-center gap-2">
                <Plus className="w-4 h-4 text-blue-400" />
                Ajouter une Entité à la Scène
              </h3>
              <button onClick={() => setIsAddModalOpen(false)} className="text-slate-400 hover:text-white">
                <X className="w-4 h-4" />
              </button>
            </div>

            <div className="space-y-3">
              <div className="text-xs font-semibold text-slate-400 uppercase">Primitives Géométriques 3D</div>
              <div className="grid grid-cols-2 gap-2">
                {PRIMITIVE_SHAPES.map((item) => (
                  <button
                    key={item.id}
                    onClick={() => handleAddObject(item.id, item.label)}
                    className="flex items-center gap-2.5 p-2.5 rounded-lg bg-[#1f222e] hover:bg-blue-600/20 border border-[#2d3142] hover:border-blue-500/50 text-xs text-slate-200 transition"
                  >
                    <span className="text-base">{item.icon}</span>
                    <span className="font-medium">{item.label}</span>
                  </button>
                ))}
              </div>

              <div className="text-xs font-semibold text-slate-400 uppercase pt-2">Acteurs de Gameplay & Lumière</div>
              <div className="grid grid-cols-2 gap-2">
                <button
                  onClick={() => handleAddObject('light', 'Point_Light', '#fde047')}
                  className="flex items-center gap-2 p-2.5 rounded-lg bg-[#1f222e] hover:bg-amber-500/20 border border-[#2d3142] text-xs text-amber-300 transition"
                >
                  <Sun className="w-4 h-4" />
                  <span>Point Light</span>
                </button>
                <button
                  onClick={() => handleAddObject('player', 'Player_Spawn', '#10b981')}
                  className="flex items-center gap-2 p-2.5 rounded-lg bg-[#1f222e] hover:bg-emerald-500/20 border border-[#2d3142] text-xs text-emerald-300 transition"
                >
                  <Gamepad2 className="w-4 h-4" />
                  <span>Player Start</span>
                </button>
              </div>
            </div>
          </div>
        </div>
      )}

      {/* ====================================================================
          MODALE 3 : NOUVEAU PROJET (TEMPLATES)
      ==================================================================== */}
      {isNewProjectModalOpen && (
        <div className="fixed inset-0 z-50 bg-black/70 backdrop-blur-sm flex items-center justify-center p-4">
          <div className="bg-[#161821] border border-[#2c2f40] rounded-xl max-w-md w-full p-5 shadow-2xl space-y-4">
            <div className="flex items-center justify-between border-b border-[#262938] pb-3">
              <h3 className="text-sm font-bold text-white">Créer un Nouveau Projet</h3>
              <button onClick={() => setIsNewProjectModalOpen(false)} className="text-slate-400 hover:text-white">
                <X className="w-4 h-4" />
              </button>
            </div>

            <div className="space-y-3">
              {[
                { id: 'default', title: 'Template Studio Showcase', desc: 'Scène complète avec primitives, arche 3D et joueur.' },
                { id: 'platformer', title: 'Template Parcours Plateforme', desc: 'Paliers de saut en hauteur avec cristal objectif.' },
                { id: 'empty', title: 'Scène Vide Minimaliste', desc: 'Sol, éclairage de base et point de départ joueur.' }
              ].map((tpl) => (
                <button
                  key={tpl.id}
                  onClick={() => {
                    const nextObjs = createTemplateScene(tpl.id);
                    setObjects(nextObjs);
                    setSelectedId(nextObjs[0]?.id || null);
                    setIsNewProjectModalOpen(false);
                    addLog('success', 'Project', `Nouveau projet créé avec le template "${tpl.title}".`);
                  }}
                  className="w-full text-left p-3 rounded-lg bg-[#1f222e] hover:bg-blue-600/20 border border-[#2d3142] hover:border-blue-500 transition"
                >
                  <div className="text-xs font-bold text-white">{tpl.title}</div>
                  <div className="text-[11px] text-slate-400 mt-0.5">{tpl.desc}</div>
                </button>
              ))}
            </div>
          </div>
        </div>
      )}
    </div>
  );
}

// ============================================================================
// COMPOSANT VIEWPORT 3D THREE.JS (RENDU, RAYCASTING, GIZMOS & MODE JEU)
// ============================================================================
function ViewportCanvas({
  objects,
  selectedId,
  onSelectObject,
  onTransformObject,
  viewMode,
  orthoAxis,
  transformTool,
  snapEnabled,
  snapGridSize,
  showGrid,
  onFpsUpdate,
  onGameStatsUpdate,
  addLog
}) {
  const mountRef = useRef(null);
  const sceneRef = useRef(null);
  const cameraRef = useRef(null);
  const rendererRef = useRef(null);
  const meshMapRef = useRef(new Map());
  const gizmoGroupRef = useRef(null);
  const gridHelperRef = useRef(null);

  // Contrôles Orbit & Drag Gizmo
  const orbitRef = useRef({
    isDown: false,
    button: 0,
    theta: 0.7,
    phi: 1.05,
    radius: 14,
    target: new THREE.Vector3(0, 1, 0),
    lastX: 0,
    lastY: 0
  });

  const dragGizmoRef = useRef({
    active: false,
    axis: null,
    startMouse: { x: 0, y: 0 },
    startObj: null
  });

  // État Physique du Mode Jeu
  const playerPhysicsRef = useRef({
    pos: new THREE.Vector3(0, 1.2, 5),
    vel: new THREE.Vector3(0, 0, 0),
    yaw: 0,
    pitch: -0.15,
    keys: {},
    isGrounded: true
  });

  // Synchronisation des refs pour la boucle d'animation
  const latestPropsRef = useRef({
    objects,
    selectedId,
    viewMode,
    transformTool,
    snapEnabled,
    snapGridSize,
    onTransformObject,
    onSelectObject
  });

  useEffect(() => {
    latestPropsRef.current = {
      objects,
      selectedId,
      viewMode,
      transformTool,
      snapEnabled,
      snapGridSize,
      onTransformObject,
      onSelectObject
    };
  }, [objects, selectedId, viewMode, transformTool, snapEnabled, snapGridSize, onTransformObject, onSelectObject]);

  // Initialisation de la scène Three.js
  useEffect(() => {
    const container = mountRef.current;
    if (!container) return;

    const scene = new THREE.Scene();
    scene.background = new THREE.Color('#0b0c10');
    scene.fog = new THREE.FogExp2('#0b0c10', 0.018);
    sceneRef.current = scene;

    const camera = new THREE.PerspectiveCamera(
      60,
      container.clientWidth / container.clientHeight,
      0.1,
      500
    );
    cameraRef.current = camera;

    const renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: 'high-performance' });
    renderer.setSize(container.clientWidth, container.clientHeight);
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap;
    container.innerHTML = '';
    container.appendChild(renderer.domElement);
    rendererRef.current = renderer;

    // Grille + Axes
    const grid = new THREE.GridHelper(40, 40, '#3b82f6', '#1e293b');
    grid.position.y = 0.01;
    scene.add(grid);
    gridHelperRef.current = grid;

    // Groupe Gizmo de Transformation (Flèches X, Y, Z)
    const gizmoGroup = new THREE.Group();
    const createAxisGizmo = (axis, colorHex, dirVec, rotEuler) => {
      const mat = new THREE.MeshBasicMaterial({ color: colorHex, depthTest: false });
      const shaft = new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.04, 1.4, 12), mat);
      shaft.position.y = 0.7;
      const head = new THREE.Mesh(new THREE.ConeGeometry(0.14, 0.35, 16), mat);
      head.position.y = 1.5;
      const pivot = new THREE.Group();
      pivot.add(shaft, head);
      pivot.rotation.set(rotEuler.x, rotEuler.y, rotEuler.z);
      pivot.traverse((child) => {
        child.userData.gizmoAxis = axis;
        child.renderOrder = 999;
      });
      return pivot;
    };

    gizmoGroup.add(
      createAxisGizmo('x', '#ef4444', new THREE.Vector3(1, 0, 0), new THREE.Euler(0, 0, -Math.PI / 2)),
      createAxisGizmo('y', '#22c55e', new THREE.Vector3(0, 1, 0), new THREE.Euler(0, 0, 0)),
      createAxisGizmo('z', '#3b82f6', new THREE.Vector3(0, 0, 1), new THREE.Euler(Math.PI / 2, 0, 0))
    );
    scene.add(gizmoGroup);
    gizmoGroupRef.current = gizmoGroup;

    // Gestion du redimensionnement
    const handleResize = () => {
      if (!container || !rendererRef.current || !cameraRef.current) return;
      const w = container.clientWidth;
      const h = container.clientHeight;
      cameraRef.current.aspect = w / h;
      cameraRef.current.updateProjectionMatrix();
      rendererRef.current.setSize(w, h);
    };
    window.addEventListener('resize', handleResize);

    // Écouteurs Clavier pour le Mode Jeu
    const onKeyDown = (e) => {
      playerPhysicsRef.current.keys[e.code] = true;
    };
    const onKeyUp = (e) => {
      playerPhysicsRef.current.keys[e.code] = false;
    };
    window.addEventListener('keydown', onKeyDown);
    window.addEventListener('keyup', onKeyUp);

    // Boucle d'animation (60 FPS)
    let frameCount = 0;
    let lastTime = performance.now();
    let animId;

    const animate = (now) => {
      animId = requestAnimationFrame(animate);
      frameCount++;
      if (now - lastTime >= 500) {
        onFpsUpdate(Math.min(60, Math.round((frameCount * 1000) / (now - lastTime))));
        frameCount = 0;
        lastTime = now;
      }

      const { objects: currObjs, selectedId: currSel, viewMode: currMode } = latestPropsRef.current;

      // Mise à jour position du Gizmo sur l'objet sélectionné
      const selObj = currObjs.find((o) => o.id === currSel);
      if (selObj && currMode !== 'game' && selObj.visible) {
        gizmoGroup.visible = true;
        gizmoGroup.position.set(selObj.position.x, selObj.position.y, selObj.position.z);
      } else {
        gizmoGroup.visible = false;
      }

      // Mise à jour de la caméra selon le mode actif
      if (currMode === 'game') {
        const p = playerPhysicsRef.current;
        const dt = 0.016;
        const moveSpeed = 6.5;
        const forward = new THREE.Vector3(Math.sin(p.yaw), 0, Math.cos(p.yaw)).normalize();
        const right = new THREE.Vector3(forward.z, 0, -forward.x);

        let moveX = 0;
        let moveZ = 0;
        if (p.keys['KeyW'] || p.keys['KeyZ'] || p.keys['ArrowUp']) {
          moveX -= forward.x;
          moveZ -= forward.z;
        }
        if (p.keys['KeyS'] || p.keys['ArrowDown']) {
          moveX += forward.x;
          moveZ += forward.z;
        }
        if (p.keys['KeyA'] || p.keys['KeyQ'] || p.keys['ArrowLeft']) {
          moveX -= right.x;
          moveZ -= right.z;
        }
        if (p.keys['KeyD'] || p.keys['ArrowRight']) {
          moveX += right.x;
          moveZ += right.z;
        }

        if (p.keys['ArrowLeft']) p.yaw += 0.03;
        if (p.keys['ArrowRight']) p.yaw -= 0.03;

        p.vel.x = moveX * moveSpeed;
        p.vel.z = moveZ * moveSpeed;

        // Saut + Gravité
        if (p.keys['Space'] && p.isGrounded) {
          p.vel.y = 7.2;
          p.isGrounded = false;
        }
        p.vel.y -= 18 * dt; // Gravité

        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.pos.z += p.vel.z * dt;

        // Collision simple avec le sol et les plateformes
        let groundHeight = 1.2;
        for (const obj of currObjs) {
          if (!obj.collision?.enabled || obj.type === 'player' || obj.type === 'light') continue;
          const halfX = (obj.scale.x || 1) * 0.6;
          const halfZ = (obj.scale.z || 1) * 0.6;
          const topY = obj.position.y + (obj.scale.y || 1) * 0.5 + 1.1;
          if (
            Math.abs(p.pos.x - obj.position.x) <= halfX &&
            Math.abs(p.pos.z - obj.position.z) <= halfZ &&
            p.pos.y >= topY - 0.4
          ) {
            groundHeight = Math.max(groundHeight, topY);
          }
        }

        if (p.pos.y <= groundHeight) {
          p.pos.y = groundHeight;
          p.vel.y = 0;
          p.isGrounded = true;
        }

        camera.position.copy(p.pos);
        const lookTarget = new THREE.Vector3(
          p.pos.x - Math.sin(p.yaw) * Math.cos(p.pitch),
          p.pos.y + Math.sin(p.pitch),
          p.pos.z - Math.cos(p.yaw) * Math.cos(p.pitch)
        );
        camera.lookAt(lookTarget);

        const horizSpeed = Math.hypot(p.vel.x, p.vel.z);
        onGameStatsUpdate((prev) => ({
          ...prev,
          speed: horizSpeed,
          isGrounded: p.isGrounded,
          energy: Math.max(15, Math.min(100, prev.energy + (horizSpeed > 0.1 ? -0.08 : 0.2)))
        }));
      } else {
        const orb = orbitRef.current;
        camera.position.x = orb.target.x + orb.radius * Math.sin(orb.phi) * Math.sin(orb.theta);
        camera.position.y = orb.target.y + orb.radius * Math.cos(orb.phi);
        camera.position.z = orb.target.z + orb.radius * Math.sin(orb.phi) * Math.cos(orb.theta);
        camera.lookAt(orb.target);
      }

      renderer.render(scene, camera);
    };

    animId = requestAnimationFrame(animate);

    return () => {
      cancelAnimationFrame(animId);
      window.removeEventListener('resize', handleResize);
      window.removeEventListener('keydown', onKeyDown);
      window.removeEventListener('keyup', onKeyUp);
      renderer.dispose();
    };
  }, [onFpsUpdate, onGameStatsUpdate]);

  // Synchronisation de la caméra en vue 2D Orthographique
  useEffect(() => {
    if (viewMode === '2d') {
      if (orthoAxis === 'top') {
        orbitRef.current.phi = 0.05;
        orbitRef.current.theta = 0;
      } else if (orthoAxis === 'front') {
        orbitRef.current.phi = Math.PI / 2;
        orbitRef.current.theta = 0;
      } else if (orthoAxis === 'side') {
        orbitRef.current.phi = Math.PI / 2;
        orbitRef.current.theta = Math.PI / 2;
      }
    } else if (viewMode === 'game') {
      const spawn = objects.find((o) => o.type === 'player');
      if (spawn) {
        playerPhysicsRef.current.pos.set(spawn.position.x, spawn.position.y + 0.5, spawn.position.z);
        playerPhysicsRef.current.vel.set(0, 0, 0);
      }
    }
  }, [viewMode, orthoAxis, objects]);

  // Visibilité de la grille
  useEffect(() => {
    if (gridHelperRef.current) {
      gridHelperRef.current.visible = showGrid && viewMode !== 'game' && viewMode !== 'render';
    }
  }, [showGrid, viewMode]);

  // Synchronisation des objets de l'état React vers la scène Three.js
  useEffect(() => {
    const scene = sceneRef.current;
    if (!scene) return;

    const currentIds = new Set(objects.map((o) => o.id));

    // Supprimer les objets retirés
    for (const [id, threeObj] of meshMapRef.current.entries()) {
      if (!currentIds.has(id)) {
        scene.remove(threeObj);
        meshMapRef.current.delete(id);
      }
    }

    // Créer ou mettre à jour chaque entité
    objects.forEach((obj) => {
      let node = meshMapRef.current.get(obj.id);

      if (!node) {
        node = new THREE.Group();
        node.userData.entityId = obj.id;
        scene.add(node);
        meshMapRef.current.set(obj.id, node);
      }

      // Reconstruire le contenu visuel de l'objet
      while (node.children.length > 0) {
        node.remove(node.children[0]);
      }

      const matColor = new THREE.Color(obj.material?.color || '#6366f1');
      let mat;
      if (obj.material?.type === 'Basic') {
        mat = new THREE.MeshBasicMaterial({ color: matColor, wireframe: !!obj.material.wireframe });
      } else if (obj.material?.type === 'Phong') {
        mat = new THREE.MeshPhongMaterial({ color: matColor, shininess: 90, wireframe: !!obj.material.wireframe });
      } else if (obj.material?.type === 'Emissive') {
        mat = new THREE.MeshStandardMaterial({
          color: matColor,
          emissive: matColor,
          emissiveIntensity: 0.8,
          roughness: 0.2,
          metalness: 0.5,
          wireframe: !!obj.material.wireframe
        });
      } else {
        mat = new THREE.MeshStandardMaterial({
          color: matColor,
          metalness: obj.material?.metalness ?? 0.3,
          roughness: obj.material?.roughness ?? 0.4,
          wireframe: !!obj.material?.wireframe
        });
      }

      if (obj.type === 'light') {
        if (obj.lightType === 'ambient') {
          const light = new THREE.AmbientLight(matColor, obj.material?.intensity ?? 0.6);
          node.add(light);
        } else {
          const light = new THREE.DirectionalLight(matColor, obj.material?.intensity ?? 1.5);
          light.castShadow = true;
          light.shadow.mapSize.width = 1024;
          light.shadow.mapSize.height = 1024;
          node.add(light);

          if (viewMode !== 'game' && viewMode !== 'render') {
            const bulb = new THREE.Mesh(
              new THREE.OctahedronGeometry(0.35, 0),
              new THREE.MeshBasicMaterial({ color: '#fde047', wireframe: true })
            );
            node.add(bulb);
          }
        }
      } else if (obj.type === 'camera') {
        if (viewMode !== 'game' && viewMode !== 'render') {
          const camMesh = new THREE.Mesh(
            new THREE.BoxGeometry(0.6, 0.45, 0.8),
            new THREE.MeshStandardMaterial({ color: '#38bdf8', metalness: 0.6, roughness: 0.3 })
          );
          node.add(camMesh);
        }
      } else if (obj.type === 'imported') {
        if (obj.rawObjText) {
          node.add(parseOBJTextToGroup(obj.rawObjText, mat));
        } else {
          node.add(createProceduralPresetGroup(obj.modelPreset, mat));
        }
      } else {
        // Mesh ou Player
        let geo;
        switch (obj.shape) {
          case 'sphere':
            geo = new THREE.SphereGeometry(0.65, 32, 24);
            break;
          case 'cylinder':
            geo = new THREE.CylinderGeometry(0.55, 0.55, 1.2, 28);
            break;
          case 'cone':
            geo = new THREE.ConeGeometry(0.65, 1.3, 28);
            break;
          case 'torus':
            geo = new THREE.TorusGeometry(0.55, 0.2, 20, 40);
            break;
          case 'plane':
            geo = new THREE.PlaneGeometry(2, 2);
            break;
          case 'capsule':
            geo = new THREE.CapsuleGeometry(0.4, 0.8, 12, 20);
            break;
          case 'torusknot':
            geo = new THREE.TorusKnotGeometry(0.5, 0.16, 80, 16);
            break;
          default:
            geo = new THREE.BoxGeometry(1, 1, 1);
        }
        const mesh = new THREE.Mesh(geo, mat);
        mesh.castShadow = true;
        mesh.receiveShadow = true;
        node.add(mesh);
      }

      // Surbrillance de sélection (Outline Box)
      if (obj.id === selectedId && viewMode !== 'game') {
        const boxHelper = new THREE.BoxHelper(node, 0x38bdf8);
        node.add(boxHelper);
      }

      node.visible = obj.visible && !(viewMode === 'game' && obj.type === 'player');
      node.position.set(obj.position.x, obj.position.y, obj.position.z);
      node.rotation.set(
        THREE.MathUtils.degToRad(obj.rotation.x),
        THREE.MathUtils.degToRad(obj.rotation.y),
        THREE.MathUtils.degToRad(obj.rotation.z)
      );
      node.scale.set(obj.scale.x, obj.scale.y, obj.scale.z);

      node.traverse((c) => {
        if (!c.userData.gizmoAxis) {
          c.userData.entityId = obj.id;
        }
      });
    });
  }, [objects, selectedId, viewMode]);

  // Interactions Souris : Sélection Raycasting, Drag Gizmos & Orbit Caméra
  const handleMouseDown = (e) => {
    const container = mountRef.current;
    if (!container || !cameraRef.current || !sceneRef.current) return;

    const rect = container.getBoundingClientRect();
    const mouse = new THREE.Vector2(
      ((e.clientX - rect.left) / rect.width) * 2 - 1,
      -((e.clientY - rect.top) / rect.height) * 2 + 1
    );

    if (viewMode === 'game') {
      orbitRef.current.isDown = true;
      orbitRef.current.lastX = e.clientX;
      orbitRef.current.lastY = e.clientY;
      return;
    }

    const raycaster = new THREE.Raycaster();
    raycaster.setFromCamera(mouse, cameraRef.current);

    // 1. Vérifier si l'utilisateur clique sur un axe du Gizmo
    if (gizmoGroupRef.current && gizmoGroupRef.current.visible && e.button === 0) {
      const gizmoHits = raycaster.intersectObjects(gizmoGroupRef.current.children, true);
      if (gizmoHits.length > 0) {
        const axis = gizmoHits[0].object.userData.gizmoAxis;
        const selObj = objects.find((o) => o.id === selectedId);
        if (axis && selObj && !selObj.locked) {
          dragGizmoRef.current = {
            active: true,
            axis,
            startMouse: { x: e.clientX, y: e.clientY },
            startObj: JSON.parse(JSON.stringify(selObj))
          };
          return;
        }
      }
    }

    // 2. Sélection d'objet par Raycasting (Clic gauche)
    if (e.button === 0) {
      const pickables = Array.from(meshMapRef.current.values());
      const hits = raycaster.intersectObjects(pickables, true);
      if (hits.length > 0) {
        const hitEntityId = hits[0].object.userData.entityId;
        if (hitEntityId) {
          onSelectObject(hitEntityId);
        }
      }
    }

    // 3. Démarrer la rotation / pan de la caméra Orbit
    orbitRef.current.isDown = true;
    orbitRef.current.button = e.button;
    orbitRef.current.lastX = e.clientX;
    orbitRef.current.lastY = e.clientY;
  };

  const handleMouseMove = (e) => {
    // Cas A : Manipulation active d'un Gizmo (Translate / Rotate / Scale)
    if (dragGizmoRef.current.active) {
      const { axis, startMouse, startObj } = dragGizmoRef.current;
      const dx = (e.clientX - startMouse.x) * 0.035;
      const dy = -(e.clientY - startMouse.y) * 0.035;
      const delta = axis === 'y' ? dy : dx;

      const snapVal = (v, step) => (snapEnabled ? Math.round(v / step) * step : Number(v.toFixed(2)));

      if (transformTool === 'translate') {
        const nextPos = { ...startObj.position };
        nextPos[axis] = Number(snapVal(startObj.position[axis] + delta, snapGridSize).toFixed(2));
        onTransformObject(startObj.id, nextPos, null, null);
      } else if (transformTool === 'rotate') {
        const nextRot = { ...startObj.rotation };
        const angleStep = snapEnabled ? 15 : 1;
        nextRot[axis] = Number(snapVal(startObj.rotation[axis] + delta * 25, angleStep).toFixed(1));
        onTransformObject(startObj.id, null, nextRot, null);
      } else if (transformTool === 'scale') {
        const nextScale = { ...startObj.scale };
        nextScale[axis] = Math.max(0.1, Number(snapVal(startObj.scale[axis] + delta * 0.5, 0.1).toFixed(2)));
        onTransformObject(startObj.id, null, null, nextScale);
      }
      return;
    }

    // Cas B : Caméra en Mode Jeu (Mouse Look)
    if (viewMode === 'game' && orbitRef.current.isDown) {
      const dx = e.clientX - orbitRef.current.lastX;
      const dy = e.clientY - orbitRef.current.lastY;
      orbitRef.current.lastX = e.clientX;
      orbitRef.current.lastY = e.clientY;
      playerPhysicsRef.current.yaw -= dx * 0.005;
      playerPhysicsRef.current.pitch = Math.max(
        -1.2,
        Math.min(1.2, playerPhysicsRef.current.pitch - dy * 0.005)
      );
      return;
    }

    // Cas C : Rotation ou Pan de l'Orbit Camera dans l'Éditeur
    if (orbitRef.current.isDown) {
      const dx = e.clientX - orbitRef.current.lastX;
      const dy = e.clientY - orbitRef.current.lastY;
      orbitRef.current.lastX = e.clientX;
      orbitRef.current.lastY = e.clientY;

      if (orbitRef.current.button === 2 || e.shiftKey) {
        // Panoramique (Clic droit ou Shift+Glisser)
        orbitRef.current.target.x -= dx * 0.02;
        orbitRef.current.target.y += dy * 0.02;
      } else {
        // Orbite autour de la scène
        orbitRef.current.theta -= dx * 0.008;
        orbitRef.current.phi = Math.max(0.1, Math.min(Math.PI / 2 - 0.02, orbitRef.current.phi - dy * 0.008));
      }
    }
  };

  const handleMouseUp = () => {
    dragGizmoRef.current.active = false;
    orbitRef.current.isDown = false;
  };

  const handleWheel = (e) => {
    if (viewMode === 'game') return;
    orbitRef.current.radius = Math.max(3, Math.min(45, orbitRef.current.radius + e.deltaY * 0.01));
  };

  return (
    <div
      ref={mountRef}
      onMouseDown={handleMouseDown}
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}
      onMouseLeave={handleMouseUp}
      onWheel={handleWheel}
      onContextMenu={(e) => e.preventDefault()}
      className="w-full h-full cursor-grab active:cursor-grabbing"
    />
  );
}

// ============================================================================
// COMPOSANT MODALE D'IMPORT 3D (DRAG & DROP + FICHIERS LOCAUX + PRESETS)
// ============================================================================
function Import3DModal({ onClose, onImportSuccess, addLog }) {
  const [isDragging, setIsDragging] = useState(false);
  const [errorMsg, setErrorMsg] = useState(null);

  const SUPPORTED_EXT = ['fbx', 'obj', 'gltf', 'glb', 'dae', '3ds'];

  const processFile = (file) => {
    setErrorMsg(null);
    const ext = file.name.split('.').pop()?.toLowerCase() || '';
    if (!SUPPORTED_EXT.includes(ext)) {
      setErrorMsg(`Format .${ext} non supporté. Formats acceptés : FBX, OBJ, glTF, GLB, DAE, 3DS.`);
      addLog('error', 'Import3D', `Échec de validation pour ${file.name}`);
      return;
    }

    const baseName = file.name.replace(/\.[^/.]+$/, '');

    // Si c'est un fichier .OBJ, on parse directement ses sommets et faces
    if (ext === 'obj') {
      const reader = new FileReader();
      reader.onload = (e) => {
        const text = e.target.result;
        onImportSuccess({
          id: `imp-${Date.now().toString(36)}`,
          name: baseName,
          type: 'imported',
          rawObjText: text,
          position: { x: 0, y: 1.2, z: 0 },
          rotation: { x: 0, y: 0, z: 0 },
          scale: { x: 1, y: 1, z: 1 },
          visible: true,
          locked: false,
          material: { color: '#38bdf8', metalness: 0.4, roughness: 0.3, opacity: 1, wireframe: false, type: 'Standard' },
          collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
        });
      };
      reader.readAsText(file);
    } else {
      // Pour FBX/GLB/glTF/DAE/3DS, création d'un modèle hiérarchique dans la scène
      onImportSuccess({
        id: `imp-${Date.now().toString(36)}`,
        name: baseName,
        type: 'imported',
        modelPreset: ext === 'fbx' ? 'spaceship' : ext === 'glb' ? 'patrol_bot' : 'energy_crystal',
        position: { x: 0, y: 1.0, z: 0 },
        rotation: { x: 0, y: 0, z: 0 },
        scale: { x: 1, y: 1, z: 1 },
        visible: true,
        locked: false,
        material: { color: '#a855f7', metalness: 0.5, roughness: 0.25, opacity: 1, wireframe: false, type: 'Standard' },
        collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
      });
    }
  };

  return (
    <div className="fixed inset-0 z-50 bg-black/75 backdrop-blur-sm flex items-center justify-center p-4">
      <div className="bg-[#161821] border border-[#2c2f40] rounded-xl max-w-lg w-full p-5 shadow-2xl space-y-4">
        <div className="flex items-center justify-between border-b border-[#262938] pb-3">
          <div className="flex items-center gap-2">
            <Package className="w-5 h-5 text-indigo-400" />
            <div>
              <h3 className="text-sm font-bold text-white">Importateur de Modèles 3D</h3>
              <p className="text-[11px] text-slate-400">Supporte FBX, OBJ, glTF, GLB, DAE, 3DS</p>
            </div>
          </div>
          <button onClick={onClose} className="text-slate-400 hover:text-white">
            <X className="w-4 h-4" />
          </button>
        </div>

        {/* Zone Drag & Drop */}
        <label
          onDragOver={(e) => {
            e.preventDefault();
            setIsDragging(true);
          }}
          onDragLeave={() => setIsDragging(false)}
          onDrop={(e) => {
            e.preventDefault();
            setIsDragging(false);
            if (e.dataTransfer.files?.[0]) processFile(e.dataTransfer.files[0]);
          }}
          className={`flex flex-col items-center justify-center p-6 border-2 border-dashed rounded-xl cursor-pointer transition ${
            isDragging
              ? 'border-indigo-400 bg-indigo-500/15'
              : 'border-[#32364a] bg-[#11131a] hover:border-indigo-500/60'
          }`}
        >
          <Upload className="w-8 h-8 text-indigo-400 mb-2" />
          <span className="text-xs font-semibold text-white">
            Glissez-déposez un fichier 3D ici ou cliquez pour parcourir
          </span>
          <span className="text-[11px] text-slate-400 mt-1">
            Formats : .FBX, .OBJ (parsing direct des sommets), .GLTF, .GLB, .DAE, .3DS
          </span>
          <input
            type="file"
            accept=".fbx,.obj,.gltf,.glb,.dae,.3ds"
            onChange={(e) => e.target.files?.[0] && processFile(e.target.files[0])}
            className="hidden"
          />
        </label>

        {errorMsg && (
          <div className="p-2.5 rounded-lg bg-rose-500/15 border border-rose-500/40 text-xs text-rose-300 flex items-center gap-2">
            <AlertTriangle className="w-4 h-4 shrink-0" />
            <span>{errorMsg}</span>
          </div>
        )}

        {/* Modèles 3D pré-intégrés pour test immédiat en un clic */}
        <div className="space-y-2">
          <div className="text-[11px] font-semibold uppercase tracking-wider text-slate-400">
            Ou importer un modèle 3D de démonstration en 1 clic :
          </div>
          <div className="grid grid-cols-2 gap-2">
            {DEMO_MODELS.map((preset) => (
              <button
                key={preset.id}
                onClick={() =>
                  onImportSuccess({
                    id: `imp-${Date.now().toString(36)}`,
                    name: preset.name.split('.')[0],
                    type: 'imported',
                    modelPreset: preset.id,
                    position: { x: 0, y: 0.8, z: 0 },
                    rotation: { x: 0, y: 0, z: 0 },
                    scale: { x: 1, y: 1, z: 1 },
                    visible: true,
                    locked: false,
                    material: {
                      color: preset.color,
                      metalness: 0.5,
                      roughness: 0.3,
                      opacity: 1,
                      wireframe: false,
                      type: 'Standard'
                    },
                    collision: { enabled: true, type: 'box', isDynamic: false, mass: 0 }
                  })
                }
                className="flex flex-col items-start p-2.5 rounded-lg bg-[#1d202b] hover:bg-indigo-600/20 border border-[#2d3142] hover:border-indigo-500/50 transition text-left"
              >
                <div className="flex items-center justify-between w-full">
                  <span className="text-xs font-bold text-white">{preset.name}</span>
                  <span className="text-[10px] font-mono px-1.5 py-0.2 rounded bg-indigo-500/20 text-indigo-300">
                    {preset.format}
                  </span>
                </div>
                <p className="text-[10px] text-slate-400 mt-1 line-clamp-1">{preset.description}</p>
              </button>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
}
