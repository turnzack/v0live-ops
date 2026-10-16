import React from 'react';
import { X, Keyboard, Check } from 'lucide-react';

export default function InputMapModal({ isOpen, onClose, bindings, onSaveBindings }) {
  if (!isOpen) return null;

  return (
    <div className="modal-backdrop" onClick={onClose}>
      <div className="modal-card" onClick={(e) => e.stopPropagation()}>
        <div className="modal-header">
          <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
            <Keyboard size={16} color="#00f2fe" />
            <span style={{ fontSize: 13, fontWeight: 700, color: '#fff' }}>Configuration des Entrées (config/input.json)</span>
          </div>
          <button className="tool-btn" onClick={onClose}>
            <X size={15} />
          </button>
        </div>

        <div className="modal-body">
          <div style={{ fontSize: 12, color: 'var(--text-muted)', marginBottom: 14 }}>
            Mappage officiel des actions pour la boucle de jeu C++ / WebGPU et l'InputManager :
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(2, 1fr)', gap: 10 }}>
            {bindings.map((b, idx) => (
              <div
                key={idx}
                style={{
                  background: 'var(--bg-surface)',
                  padding: '8px 12px',
                  borderRadius: 'var(--radius-sm)',
                  border: '1px solid var(--border-subtle)',
                  display: 'flex',
                  justifyContent: 'space-between',
                  alignItems: 'center'
                }}
              >
                <span style={{ fontSize: 12, fontWeight: 600, color: 'var(--text-main)' }}>
                  {b.action}
                </span>
                <span
                  style={{
                    background: 'rgba(0, 242, 254, 0.12)',
                    color: 'var(--accent-cyan)',
                    padding: '3px 8px',
                    borderRadius: 4,
                    fontSize: 11,
                    fontFamily: 'var(--font-mono)',
                    border: '1px solid rgba(0, 242, 254, 0.25)'
                  }}
                >
                  Touche: {b.key}
                </span>
              </div>
            ))}
          </div>

          <div style={{ marginTop: 20, display: 'flex', justifyContent: 'flex-end', gap: 8 }}>
            <button className="action-btn primary" onClick={onClose}>
              <Check size={13} />
              <span>Valider les Assignations</span>
            </button>
          </div>
        </div>
      </div>
    </div>
  );
}
