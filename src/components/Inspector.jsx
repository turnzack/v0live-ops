import React from 'react';
import { Sliders, Box, Layers, Activity, FileCode, Plus, Check } from 'lucide-react';

export default function Inspector({ selectedEntity, onUpdateComponent, onUpdateEntityName }) {
  if (!selectedEntity) {
    return (
      <div className="right-sidebar">
        <div className="panel-header">Inspecteur</div>
        <div style={{ padding: 24, textAlign: 'center', color: 'var(--text-dim)', fontSize: 12 }}>
          Sélectionnez une entité dans la scène ou la hiérarchie pour inspecter ses composants.
        </div>
      </div>
    );
  }

  const { components } = selectedEntity;

  const handleVec3Change = (compName, prop, index, value) => {
    const current = [...(components[compName][prop] || [0, 0, 0])];
    current[index] = parseFloat(value) || 0;
    onUpdateComponent(selectedEntity.id, compName, { [prop]: current });
  };

  const handleVec2Change = (compName, prop, index, value) => {
    const current = [...(components[compName][prop] || [0, 0])];
    current[index] = parseFloat(value) || 0;
    onUpdateComponent(selectedEntity.id, compName, { [prop]: current });
  };

  const handlePropChange = (compName, prop, value) => {
    onUpdateComponent(selectedEntity.id, compName, { [prop]: value });
  };

  return (
    <div className="right-sidebar">
      <div className="panel-header">
        <span>Inspecteur</span>
        <span style={{ fontSize: 10, color: 'var(--accent-cyan)', fontFamily: 'var(--font-mono)' }}>
          GUID: {selectedEntity.id.slice(0, 8)}...
        </span>
      </div>

      {/* Entity Header */}
      <div style={{ padding: '12px 14px', borderBottom: '1px solid var(--border-subtle)', background: 'var(--bg-surface)' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
          <input
            type="text"
            value={selectedEntity.name}
            onChange={(e) => onUpdateEntityName(selectedEntity.id, e.target.value)}
            style={{
              flex: 1,
              background: 'var(--bg-primary)',
              border: '1px solid var(--border-subtle)',
              borderRadius: 'var(--radius-sm)',
              padding: '6px 10px',
              fontSize: '13px',
              fontWeight: '600',
              color: 'var(--text-main)',
              outline: 'none'
            }}
          />
        </div>
      </div>

      {/* 1. Transform Component */}
      {components.Transform && (
        <div className="component-card">
          <div className="component-title">
            <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
              <Sliders size={13} color="#00f2fe" />
              <span>Transform</span>
            </div>
          </div>
          <div className="component-body">
            <div className="prop-row">
              <span className="prop-label">Position</span>
              <div className="vec3-inputs">
                <div className="vec-item">
                  <span className="vec-tag x">X</span>
                  <input
                    type="number"
                    step="0.1"
                    value={components.Transform.position[0]}
                    onChange={(e) => handleVec3Change('Transform', 'position', 0, e.target.value)}
                  />
                </div>
                <div className="vec-item">
                  <span className="vec-tag y">Y</span>
                  <input
                    type="number"
                    step="0.1"
                    value={components.Transform.position[1]}
                    onChange={(e) => handleVec3Change('Transform', 'position', 1, e.target.value)}
                  />
                </div>
                <div className="vec-item">
                  <span className="vec-tag z">Z</span>
                  <input
                    type="number"
                    step="0.1"
                    value={components.Transform.position[2]}
                    onChange={(e) => handleVec3Change('Transform', 'position', 2, e.target.value)}
                  />
                </div>
              </div>
            </div>

            <div className="prop-row">
              <span className="prop-label">Échelle</span>
              <div className="vec3-inputs">
                <div className="vec-item">
                  <span className="vec-tag x">X</span>
                  <input
                    type="number"
                    step="0.1"
                    value={components.Transform.scale[0]}
                    onChange={(e) => handleVec3Change('Transform', 'scale', 0, e.target.value)}
                  />
                </div>
                <div className="vec-item">
                  <span className="vec-tag y">Y</span>
                  <input
                    type="number"
                    step="0.1"
                    value={components.Transform.scale[1]}
                    onChange={(e) => handleVec3Change('Transform', 'scale', 1, e.target.value)}
                  />
                </div>
                <div className="vec-item">
                  <span className="vec-tag z">Z</span>
                  <input
                    type="number"
                    step="0.1"
                    value={components.Transform.scale[2]}
                    onChange={(e) => handleVec3Change('Transform', 'scale', 2, e.target.value)}
                  />
                </div>
              </div>
            </div>
          </div>
        </div>
      )}

      {/* 2. Sprite Component */}
      {components.SpriteComponent && (
        <div className="component-card">
          <div className="component-title">
            <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
              <Layers size={13} color="#4facfe" />
              <span>Sprite Component</span>
            </div>
          </div>
          <div className="component-body">
            <div className="prop-row">
              <span className="prop-label">Dimensions</span>
              <div className="vec3-inputs">
                <div className="vec-item">
                  <span className="vec-tag x">L</span>
                  <input
                    type="number"
                    value={components.SpriteComponent.size[0]}
                    onChange={(e) => handleVec2Change('SpriteComponent', 'size', 0, e.target.value)}
                  />
                </div>
                <div className="vec-item">
                  <span className="vec-tag y">H</span>
                  <input
                    type="number"
                    value={components.SpriteComponent.size[1]}
                    onChange={(e) => handleVec2Change('SpriteComponent', 'size', 1, e.target.value)}
                  />
                </div>
              </div>
            </div>

            <div className="prop-row">
              <span className="prop-label">Couleur</span>
              <input
                type="color"
                value={components.SpriteComponent.color || '#00f2fe'}
                onChange={(e) => handlePropChange('SpriteComponent', 'color', e.target.value)}
                style={{ width: 36, height: 26, border: 'none', background: 'transparent', cursor: 'pointer' }}
              />
              <span style={{ fontSize: 11, fontFamily: 'var(--font-mono)', color: 'var(--text-muted)' }}>
                {components.SpriteComponent.color || '#00f2fe'}
              </span>
            </div>

            <div className="prop-row">
              <span className="prop-label">Calque</span>
              <input
                type="number"
                className="prop-input"
                value={components.SpriteComponent.sortingLayer || 0}
                onChange={(e) => handlePropChange('SpriteComponent', 'sortingLayer', parseInt(e.target.value) || 0)}
              />
            </div>
          </div>
        </div>
      )}

      {/* 3. RigidBody2D Component */}
      {components.RigidBody2DComponent && (
        <div className="component-card">
          <div className="component-title">
            <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
              <Box size={13} color="#00f5a0" />
              <span>RigidBody 2D (Box2D v3)</span>
            </div>
          </div>
          <div className="component-body">
            <div className="prop-row">
              <span className="prop-label">Type</span>
              <select
                className="prop-input"
                value={components.RigidBody2DComponent.type}
                onChange={(e) => handlePropChange('RigidBody2DComponent', 'type', e.target.value)}
              >
                <option value="Dynamic">Dynamic (Physique)</option>
                <option value="Static">Static (Décor fixe)</option>
                <option value="Kinematic">Kinematic (Plateforme)</option>
              </select>
            </div>

            <div className="prop-row">
              <span className="prop-label">Gravité</span>
              <input
                type="number"
                step="0.1"
                className="prop-input"
                value={components.RigidBody2DComponent.gravityScale ?? 1.0}
                onChange={(e) => handlePropChange('RigidBody2DComponent', 'gravityScale', parseFloat(e.target.value) || 0)}
              />
            </div>
          </div>
        </div>
      )}

      {/* 4. Collider2D Component */}
      {components.Collider2DComponent && (
        <div className="component-card">
          <div className="component-title">
            <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
              <Activity size={13} color="#ffb300" />
              <span>Collider 2D</span>
            </div>
          </div>
          <div className="component-body">
            <div className="prop-row">
              <span className="prop-label">Forme</span>
              <span className="prop-input">{components.Collider2DComponent.type}</span>
            </div>
            <div className="prop-row">
              <span className="prop-label">Friction</span>
              <input
                type="number"
                step="0.1"
                className="prop-input"
                value={components.Collider2DComponent.friction ?? 0.2}
                onChange={(e) => handlePropChange('Collider2DComponent', 'friction', parseFloat(e.target.value) || 0)}
              />
            </div>
          </div>
        </div>
      )}

      {/* 5. Script Component */}
      {components.ScriptComponent && (
        <div className="component-card">
          <div className="component-title">
            <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
              <FileCode size={13} color="#a855f7" />
              <span>Script Lua</span>
            </div>
          </div>
          <div className="component-body">
            <div className="prop-row">
              <span className="prop-label">Script</span>
              <span className="prop-input" style={{ color: 'var(--accent-purple)' }}>
                {components.ScriptComponent.scriptName}
              </span>
            </div>
            <div className="prop-row">
              <span className="prop-label">Actif</span>
              <input
                type="checkbox"
                checked={components.ScriptComponent.enabled}
                onChange={(e) => handlePropChange('ScriptComponent', 'enabled', e.target.checked)}
              />
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
