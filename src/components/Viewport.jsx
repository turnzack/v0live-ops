import React, { useRef, useEffect, useState } from 'react';
import { Play, Pause, RotateCcw, Maximize2, Move, Eye, Grid } from 'lucide-react';

export default function Viewport({
  scene,
  selectedEntityId,
  onSelectEntity,
  onUpdateEntityPosition,
  isPlaying,
  onTogglePlay,
  onResetSimulation
}) {
  const canvasRef = useRef(null);
  const [zoom, setZoom] = useState(48); // pixels per meter
  const [cameraOffset, setCameraOffset] = useState({ x: 0, y: -0.5 });
  const [isPanning, setIsPanning] = useState(false);
  const [panStart, setPanStart] = useState({ x: 0, y: 0 });
  const [draggedEntity, setDraggedEntity] = useState(null);
  const [showColliders, setShowColliders] = useState(true);
  const [showGrid, setShowGrid] = useState(true);

  // Simulation physics state
  const physicsStateRef = useRef({
    playerVelocity: { x: 0, y: 0 },
    isGrounded: false,
    particles: [],
    coinsCollected: 0
  });

  const keysPressedRef = useRef({});

  // Keyboard handlers for Play Mode
  useEffect(() => {
    const handleKeyDown = (e) => {
      keysPressedRef.current[e.key.toLowerCase()] = true;
      keysPressedRef.current[e.code] = true;
    };
    const handleKeyUp = (e) => {
      keysPressedRef.current[e.key.toLowerCase()] = false;
      keysPressedRef.current[e.code] = false;
    };

    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);
    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
    };
  }, []);

  // Main Render & Simulation Loop
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    let animationFrameId;
    let lastTime = performance.now();

    const loop = (currentTime) => {
      const dt = Math.min((currentTime - lastTime) / 1000, 0.05); // cap delta time
      lastTime = currentTime;

      const width = canvas.width;
      const height = canvas.height;
      const originX = width / 2 - cameraOffset.x * zoom;
      const originY = height / 2 + cameraOffset.y * zoom;

      // 1. Simulation Step (if isPlaying)
      if (isPlaying) {
        const player = scene.entities.find((e) => e.name === 'Player');
        if (player && player.components.Transform) {
          const keys = keysPressedRef.current;
          const speed = 6.0; // m/s
          const jumpPower = 11.0;
          const gravity = -24.0; // m/s²

          let vx = 0;
          if (keys['a'] || keys['arrowleft']) vx -= speed;
          if (keys['d'] || keys['arrowright']) vx += speed;

          const pState = physicsStateRef.current;
          pState.playerVelocity.x = vx;

          // Jump
          if ((keys[' '] || keys['space'] || keys['w'] || keys['arrowup']) && pState.isGrounded) {
            pState.playerVelocity.y = jumpPower;
            pState.isGrounded = false;
            // Spawn jump particles
            for (let i = 0; i < 8; i++) {
              pState.particles.push({
                x: player.components.Transform.position[0] + (Math.random() - 0.5) * 0.4,
                y: player.components.Transform.position[1] - 0.7,
                vx: (Math.random() - 0.5) * 4,
                vy: Math.random() * 2 + 1,
                life: 0.4,
                color: '#00f2fe'
              });
            }
          }

          // Apply Gravity
          pState.playerVelocity.y += gravity * dt;

          // Integrate position
          let newX = player.components.Transform.position[0] + pState.playerVelocity.x * dt;
          let newY = player.components.Transform.position[1] + pState.playerVelocity.y * dt;

          // Collision check against Ground and Platforms
          pState.isGrounded = false;
          scene.entities.forEach((entity) => {
            if (entity.id === player.id || !entity.components.Collider2DComponent) return;

            const ePos = entity.components.Transform.position;
            const eCollider = entity.components.Collider2DComponent;
            const halfW = (eCollider.size[0] > 100 ? eCollider.size[0] / 80 : eCollider.size[0]) / 2;
            const halfH = (eCollider.size[1] > 100 ? eCollider.size[1] / 80 : eCollider.size[1]) / 2;

            const playerHalfW = 0.4;
            const playerHalfH = 0.8;

            if (
              newX + playerHalfW > ePos[0] - halfW &&
              newX - playerHalfW < ePos[0] + halfW &&
              newY - playerHalfH < ePos[1] + halfH &&
              newY + playerHalfH > ePos[1] - halfH
            ) {
              if (entity.name.includes('Coin')) {
                // Collect coin
                pState.coinsCollected += 1;
                entity.components.Transform.position[1] = -999; // hide
                for (let i = 0; i < 15; i++) {
                  pState.particles.push({
                    x: ePos[0],
                    y: ePos[1],
                    vx: (Math.random() - 0.5) * 8,
                    vy: (Math.random() - 0.5) * 8,
                    life: 0.6,
                    color: '#ffb300'
                  });
                }
              } else if (pState.playerVelocity.y <= 0 && player.components.Transform.position[1] - playerHalfH >= ePos[1] + halfH - 0.2) {
                // Landing on top
                newY = ePos[1] + halfH + playerHalfH;
                pState.playerVelocity.y = 0;
                pState.isGrounded = true;
              }
            }
          });

          // Floor safety boundary
          if (newY < -1.1) {
            newY = -1.1;
            pState.playerVelocity.y = 0;
            pState.isGrounded = true;
          }

          player.components.Transform.position[0] = newX;
          player.components.Transform.position[1] = newY;

          // Smooth Camera tracking in Play Mode
          setCameraOffset((prev) => ({
            x: prev.x + (newX - prev.x) * 0.1,
            y: prev.y + (newY - prev.y) * 0.1
          }));
        }

        // Update particles
        const pState = physicsStateRef.current;
        pState.particles.forEach((p) => {
          p.x += p.vx * dt;
          p.y += p.vy * dt;
          p.life -= dt;
        });
        pState.particles = pState.particles.filter((p) => p.life > 0);
      }

      // 2. Clear Screen
      ctx.fillStyle = '#0a0d16';
      ctx.fillRect(0, 0, width, height);

      // 3. Draw Grid
      if (showGrid) {
        ctx.strokeStyle = 'rgba(255, 255, 255, 0.04)';
        ctx.lineWidth = 1;
        const gridSize = zoom;
        const startX = originX % gridSize;
        const startY = originY % gridSize;

        for (let x = startX; x < width; x += gridSize) {
          ctx.beginPath();
          ctx.moveTo(x, 0);
          ctx.lineTo(x, height);
          ctx.stroke();
        }
        for (let y = startY; y < height; y += gridSize) {
          ctx.beginPath();
          ctx.moveTo(0, y);
          ctx.lineTo(width, y);
          ctx.stroke();
        }

        // Axes (X in red, Y in green)
        ctx.strokeStyle = 'rgba(255, 75, 75, 0.4)';
        ctx.lineWidth = 1.5;
        ctx.beginPath();
        ctx.moveTo(0, originY);
        ctx.lineTo(width, originY);
        ctx.stroke();

        ctx.strokeStyle = 'rgba(75, 255, 120, 0.4)';
        ctx.beginPath();
        ctx.moveTo(originX, 0);
        ctx.lineTo(originX, height);
        ctx.stroke();
      }

      // 4. Render Entities
      scene.entities.forEach((entity) => {
        if (!entity.active || !entity.components.Transform) return;

        const [ex, ey] = entity.components.Transform.position;
        const screenX = originX + ex * zoom;
        const screenY = originY - ey * zoom; // invert Y for standard screen coordinates

        const isSelected = entity.id === selectedEntityId;
        const sprite = entity.components.SpriteComponent;
        const collider = entity.components.Collider2DComponent;

        // Draw Entity Sprite representation
        if (sprite) {
          let w = (sprite.size[0] > 100 ? sprite.size[0] / 80 : sprite.size[0]) * zoom;
          let h = (sprite.size[1] > 100 ? sprite.size[1] / 80 : sprite.size[1]) * zoom;

          ctx.save();
          ctx.translate(screenX, screenY);

          if (entity.name === 'Player') {
            // Stylized Cyber Player
            ctx.shadowColor = 'rgba(0, 242, 254, 0.5)';
            ctx.shadowBlur = 12;
            const grad = ctx.createLinearGradient(-w / 2, -h / 2, w / 2, h / 2);
            grad.addColorStop(0, '#00f2fe');
            grad.addColorStop(1, '#4facfe');
            ctx.fillStyle = grad;
            ctx.beginPath();
            ctx.roundRect(-w / 2, -h / 2, w, h, 8);
            ctx.fill();

            // Visor
            ctx.fillStyle = '#ffffff';
            ctx.beginPath();
            ctx.roundRect(-w / 4, -h / 3, w / 2, h / 5, 4);
            ctx.fill();

            // Energy core
            ctx.fillStyle = '#00f5a0';
            ctx.beginPath();
            ctx.arc(0, h / 8, 4, 0, Math.PI * 2);
            ctx.fill();
          } else if (entity.name === 'Ground') {
            // Sci-fi Platform / Ground
            ctx.fillStyle = '#1e293b';
            ctx.fillRect(-w / 2, -h / 2, w, h);

            // Neon edge
            ctx.strokeStyle = '#38bdf8';
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(-w / 2, -h / 2);
            ctx.lineTo(w / 2, -h / 2);
            ctx.stroke();
          } else if (entity.name.includes('Coin')) {
            // Floating Gold Coin
            ctx.shadowColor = 'rgba(255, 179, 0, 0.7)';
            ctx.shadowBlur = 14;
            ctx.fillStyle = '#ffb300';
            ctx.beginPath();
            ctx.arc(0, 0, w / 2, 0, Math.PI * 2);
            ctx.fill();
            ctx.fillStyle = '#fff';
            ctx.font = 'bold 12px Inter';
            ctx.textAlign = 'center';
            ctx.textBaseline = 'middle';
            ctx.fillText('$', 0, 0);
          } else {
            // Generic entity
            ctx.fillStyle = sprite.color || '#3b82f6';
            ctx.beginPath();
            ctx.roundRect(-w / 2, -h / 2, w, h, 4);
            ctx.fill();
          }

          ctx.restore();
        }

        // Draw Collider Wireframe
        if (showColliders && collider) {
          ctx.save();
          ctx.translate(screenX, screenY);
          ctx.strokeStyle = '#00f5a0';
          ctx.lineWidth = 1.5;
          ctx.setLineDash([4, 4]);

          let cw = (collider.size[0] > 100 ? collider.size[0] / 80 : collider.size[0]) * zoom;
          let ch = (collider.size[1] > 100 ? collider.size[1] / 80 : collider.size[1]) * zoom;

          if (collider.type === 'Capsule') {
            ctx.beginPath();
            ctx.roundRect(-cw / 2, -ch / 2, cw, ch, cw / 2);
            ctx.stroke();
          } else {
            ctx.strokeRect(-cw / 2, -ch / 2, cw, ch);
          }
          ctx.restore();
        }

        // Selection Box & Gizmo
        if (isSelected) {
          ctx.save();
          ctx.translate(screenX, screenY);
          ctx.strokeStyle = '#00f2fe';
          ctx.lineWidth = 2;
          ctx.setLineDash([]);
          let w = (sprite ? (sprite.size[0] > 100 ? sprite.size[0] / 80 : sprite.size[0]) : 1) * zoom + 12;
          let h = (sprite ? (sprite.size[1] > 100 ? sprite.size[1] / 80 : sprite.size[1]) : 1) * zoom + 12;
          ctx.strokeRect(-w / 2, -h / 2, w, h);

          // Entity Label
          ctx.fillStyle = 'rgba(0, 242, 254, 0.9)';
          ctx.font = '10px JetBrains Mono';
          ctx.fillText(entity.name, -w / 2, -h / 2 - 6);
          ctx.restore();
        }
      });

      // 5. Render Particles
      const pState = physicsStateRef.current;
      pState.particles.forEach((p) => {
        const px = originX + p.x * zoom;
        const py = originY - p.y * zoom;
        ctx.fillStyle = p.color;
        ctx.shadowColor = p.color;
        ctx.shadowBlur = 8;
        ctx.beginPath();
        ctx.arc(px, py, 3 * (p.life / 0.5), 0, Math.PI * 2);
        ctx.fill();
      });

      animationFrameId = requestAnimationFrame(loop);
    };

    animationFrameId = requestAnimationFrame(loop);
    return () => cancelAnimationFrame(animationFrameId);
  }, [scene, zoom, cameraOffset, isPlaying, selectedEntityId, showColliders, showGrid]);

  // Mouse Interactivity
  const handleMouseDown = (e) => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const rect = canvas.getBoundingClientRect();
    const mouseX = e.clientX - rect.left;
    const mouseY = e.clientY - rect.top;

    if (e.button === 1 || e.shiftKey) {
      // Middle click or Shift+drag pan
      setIsPanning(true);
      setPanStart({ x: mouseX, y: mouseY });
      return;
    }

    // Check hit on entities
    const width = canvas.width;
    const height = canvas.height;
    const originX = width / 2 - cameraOffset.x * zoom;
    const originY = height / 2 + cameraOffset.y * zoom;

    let clickedEntity = null;
    for (let i = scene.entities.length - 1; i >= 0; i--) {
      const entity = scene.entities[i];
      if (!entity.active || !entity.components.Transform) continue;

      const [ex, ey] = entity.components.Transform.position;
      const screenX = originX + ex * zoom;
      const screenY = originY - ey * zoom;
      const sprite = entity.components.SpriteComponent;
      const w = ((sprite ? sprite.size[0] : 48) > 100 ? sprite.size[0] / 80 : 1) * zoom;
      const h = ((sprite ? sprite.size[1] : 64) > 100 ? sprite.size[1] / 80 : 1) * zoom;

      if (
        mouseX >= screenX - w / 2 &&
        mouseX <= screenX + w / 2 &&
        mouseY >= screenY - h / 2 &&
        mouseY <= screenY + h / 2
      ) {
        clickedEntity = entity;
        break;
      }
    }

    if (clickedEntity) {
      onSelectEntity(clickedEntity.id);
      setDraggedEntity({
        id: clickedEntity.id,
        startX: mouseX,
        startY: mouseY,
        entityStartX: clickedEntity.components.Transform.position[0],
        entityStartY: clickedEntity.components.Transform.position[1]
      });
    } else {
      setIsPanning(true);
      setPanStart({ x: mouseX, y: mouseY });
    }
  };

  const handleMouseMove = (e) => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const rect = canvas.getBoundingClientRect();
    const mouseX = e.clientX - rect.left;
    const mouseY = e.clientY - rect.top;

    if (isPanning) {
      const dx = (mouseX - panStart.x) / zoom;
      const dy = (mouseY - panStart.y) / zoom;
      setCameraOffset((prev) => ({ x: prev.x - dx, y: prev.y + dy }));
      setPanStart({ x: mouseX, y: mouseY });
    } else if (draggedEntity && !isPlaying) {
      const dx = (mouseX - draggedEntity.startX) / zoom;
      const dy = -(mouseY - draggedEntity.startY) / zoom;
      onUpdateEntityPosition(
        draggedEntity.id,
        parseFloat((draggedEntity.entityStartX + dx).toFixed(2)),
        parseFloat((draggedEntity.entityStartY + dy).toFixed(2))
      );
    }
  };

  const handleMouseUp = () => {
    setIsPanning(false);
    setDraggedEntity(null);
  };

  const handleWheel = (e) => {
    e.preventDefault();
    const zoomFactor = e.deltaY < 0 ? 1.12 : 0.88;
    setZoom((prev) => Math.max(16, Math.min(180, prev * zoomFactor)));
  };

  return (
    <div className="viewport-container" onContextMenu={(e) => e.preventDefault()}>
      <div className="viewport-overlay">
        <div className="stat-pill">
          <span>FPS:</span> <strong>60</strong>
        </div>
        <div className="stat-pill">
          <span>Échelle:</span> <strong>{Math.round(zoom)} px/m</strong>
        </div>
        <div className="stat-pill">
          <span>Pièces:</span> <strong>{physicsStateRef.current.coinsCollected}</strong>
        </div>
      </div>

      {isPlaying && (
        <div className="playmode-banner">
          <Play size={14} fill="#00f5a0" />
          <span>PLAY MODE ACTIF (WASD / Flèches + Espace pour sauter)</span>
        </div>
      )}

      {/* Viewport Control Buttons */}
      <div style={{ position: 'absolute', bottom: 12, right: 12, display: 'flex', gap: 6, zIndex: 10 }}>
        <button
          className="tool-btn"
          style={{ background: showGrid ? 'rgba(0, 242, 254, 0.15)' : 'var(--bg-surface)' }}
          onClick={() => setShowGrid(!showGrid)}
          title="Afficher la grille"
        >
          <Grid size={14} color={showGrid ? '#00f2fe' : 'var(--text-muted)'} />
        </button>
        <button
          className="tool-btn"
          style={{ background: showColliders ? 'rgba(0, 245, 160, 0.15)' : 'var(--bg-surface)' }}
          onClick={() => setShowColliders(!showColliders)}
          title="Afficher les collisionneurs (Box2D)"
        >
          <Eye size={14} color={showColliders ? '#00f5a0' : 'var(--text-muted)'} />
        </button>
        <button
          className="tool-btn"
          style={{ background: 'var(--bg-surface)' }}
          onClick={() => { setZoom(48); setCameraOffset({ x: 0, y: -0.5 }); }}
          title="Recentrer la caméra"
        >
          <RotateCcw size={14} />
        </button>
      </div>

      <canvas
        ref={canvasRef}
        className="game-canvas"
        width={960}
        height={540}
        onMouseDown={handleMouseDown}
        onMouseMove={handleMouseMove}
        onMouseUp={handleMouseUp}
        onWheel={handleWheel}
      />
    </div>
  );
}
