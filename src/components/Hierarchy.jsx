import React, { useState } from 'react';
import { Plus, Trash2, Copy, Search, Eye, EyeOff, Box, User, Camera, Sparkles } from 'lucide-react';

export default function Hierarchy({
  entities,
  selectedEntityId,
  onSelectEntity,
  onAddEntity,
  onDeleteEntity,
  onDuplicateEntity,
  onToggleActive
}) {
  const [filter, setFilter] = useState('');

  const filteredEntities = entities.filter((e) =>
    e.name.toLowerCase().includes(filter.toLowerCase())
  );

  const getEntityIcon = (entity) => {
    if (entity.name === 'Player') return <User size={14} color="#00f2fe" />;
    if (entity.name.includes('Camera')) return <Camera size={14} color="#a855f7" />;
    if (entity.name.includes('Coin')) return <Sparkles size={14} color="#ffb300" />;
    return <Box size={14} color="#64748b" />;
  };

  return (
    <div className="left-sidebar">
      <div className="panel-header">
        <span>Hiérarchie ({entities.length})</span>
        <button
          className="tool-btn"
          style={{ padding: '3px 8px', background: 'rgba(0, 242, 254, 0.12)', color: '#00f2fe' }}
          onClick={onAddEntity}
          title="Ajouter une entité"
        >
          <Plus size={14} />
          <span>Ajouter</span>
        </button>
      </div>

      {/* Search Input */}
      <div style={{ padding: '8px 10px', borderBottom: '1px solid var(--border-subtle)' }}>
        <div style={{
          display: 'flex',
          alignItems: 'center',
          background: 'var(--bg-primary)',
          borderRadius: 'var(--radius-sm)',
          padding: '4px 8px',
          gap: '6px',
          border: '1px solid var(--border-subtle)'
        }}>
          <Search size={12} color="var(--text-dim)" />
          <input
            type="text"
            placeholder="Filtrer entités..."
            value={filter}
            onChange={(e) => setFilter(e.target.value)}
            style={{
              background: 'transparent',
              border: 'none',
              outline: 'none',
              color: 'var(--text-main)',
              fontSize: '11px',
              width: '100%',
              fontFamily: 'var(--font-sans)'
            }}
          />
        </div>
      </div>

      {/* Entity Tree */}
      <div className="hierarchy-tree">
        {filteredEntities.map((entity) => {
          const isSelected = entity.id === selectedEntityId;
          return (
            <div
              key={entity.id}
              className={`hierarchy-item ${isSelected ? 'selected' : ''}`}
              onClick={() => onSelectEntity(entity.id)}
            >
              <span
                onClick={(e) => { e.stopPropagation(); onToggleActive(entity.id); }}
                style={{ cursor: 'pointer', opacity: entity.active ? 0.9 : 0.3 }}
              >
                {entity.active ? <Eye size={12} /> : <EyeOff size={12} />}
              </span>

              {getEntityIcon(entity)}

              <span style={{ flex: 1, overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                {entity.name}
              </span>

              {isSelected && (
                <div style={{ display: 'flex', gap: 4 }} onClick={(e) => e.stopPropagation()}>
                  <button
                    className="tool-btn"
                    style={{ padding: '2px 4px' }}
                    onClick={() => onDuplicateEntity(entity.id)}
                    title="Dupliquer"
                  >
                    <Copy size={11} />
                  </button>
                  <button
                    className="tool-btn danger"
                    style={{ padding: '2px 4px' }}
                    onClick={() => onDeleteEntity(entity.id)}
                    title="Supprimer"
                  >
                    <Trash2 size={11} />
                  </button>
                </div>
              )}
            </div>
          );
        })}
      </div>
    </div>
  );
}
