import React, { useState } from 'react';
import { Image, Music, FileCode, Upload, RefreshCw, CheckCircle, ShieldCheck } from 'lucide-react';

export default function AssetBrowser({ assets, onImportAsset, onRehashAssets }) {
  const [selectedAsset, setSelectedAsset] = useState(assets[0] || null);

  const getIcon = (type) => {
    switch (type) {
      case 'Texture2D': return <Image size={24} color="#00f2fe" />;
      case 'AudioClip': return <Music size={24} color="#00f5a0" />;
      case 'LuaScript': return <FileCode size={24} color="#a855f7" />;
      default: return <FileCode size={24} color="#64748b" />;
    }
  };

  return (
    <div style={{ display: 'flex', height: '100%', gap: 16 }}>
      {/* Asset Grid */}
      <div style={{ flex: 1, overflowY: 'auto' }}>
        <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: 12, alignItems: 'center' }}>
          <span style={{ fontSize: 12, fontWeight: 700, color: 'var(--text-muted)' }}>
            DATABASE D'ASSETS ({assets.length} fichiers ingérés)
          </span>
          <div style={{ display: 'flex', gap: 6 }}>
            <button className="action-btn" onClick={onRehashAssets}>
              <RefreshCw size={12} />
              <span>Recalculer SHA-256</span>
            </button>
            <label className="action-btn primary" style={{ cursor: 'pointer' }}>
              <Upload size={12} />
              <span>Importer Asset</span>
              <input
                type="file"
                style={{ display: 'none' }}
                onChange={(e) => {
                  if (e.target.files && e.target.files[0]) {
                    onImportAsset(e.target.files[0]);
                  }
                }}
              />
            </label>
          </div>
        </div>

        <div className="asset-grid">
          {assets.map((asset) => {
            const isSelected = selectedAsset && selectedAsset.guid === asset.guid;
            return (
              <div
                key={asset.guid}
                className="asset-card"
                style={{ borderColor: isSelected ? 'var(--accent-cyan)' : 'var(--border-subtle)' }}
                onClick={() => setSelectedAsset(asset)}
              >
                <div className="asset-icon-box">
                  {getIcon(asset.type)}
                </div>
                <div className="asset-name">{asset.name}</div>
                <div className="asset-meta">{asset.size}</div>
                <div style={{ marginTop: 4, display: 'flex', alignItems: 'center', gap: 4 }}>
                  <ShieldCheck size={11} color="#00f5a0" />
                  <span style={{ fontSize: 9, color: 'var(--accent-emerald)', fontFamily: 'var(--font-mono)' }}>
                    SHA-256 OK
                  </span>
                </div>
              </div>
            );
          })}
        </div>
      </div>

      {/* Asset Details Preview */}
      {selectedAsset && (
        <div style={{
          width: 280,
          background: 'var(--bg-surface)',
          borderRadius: 'var(--radius-md)',
          padding: 14,
          border: '1px solid var(--border-subtle)',
          display: 'flex',
          flexDirection: 'column',
          gap: 10
        }}>
          <div style={{ fontSize: 12, fontWeight: 700, color: 'var(--text-main)', borderBottom: '1px solid var(--border-subtle)', paddingBottom: 6 }}>
            Détails de l'Artefact
          </div>
          <div>
            <span style={{ fontSize: 11, color: 'var(--text-muted)' }}>Nom de fichier :</span>
            <div style={{ fontSize: 12, fontWeight: 600, color: 'var(--accent-cyan)' }}>{selectedAsset.name}</div>
          </div>
          <div>
            <span style={{ fontSize: 11, color: 'var(--text-muted)' }}>GUID Immuable :</span>
            <div style={{ fontSize: 10, fontFamily: 'var(--font-mono)', color: 'var(--text-dim)', wordBreak: 'break-all' }}>
              {selectedAsset.guid}
            </div>
          </div>
          <div>
            <span style={{ fontSize: 11, color: 'var(--text-muted)' }}>Empreinte SHA-256 :</span>
            <div style={{ fontSize: 10, fontFamily: 'var(--font-mono)', color: '#00f5a0', wordBreak: 'break-all' }}>
              {selectedAsset.sha256}
            </div>
          </div>
          <div>
            <span style={{ fontSize: 11, color: 'var(--text-muted)' }}>Chemin relatif :</span>
            <div style={{ fontSize: 11, fontFamily: 'var(--font-mono)', color: 'var(--text-muted)' }}>
              {selectedAsset.path}
            </div>
          </div>
          <div style={{ marginTop: 'auto', display: 'flex', alignItems: 'center', gap: 6, background: 'rgba(0, 245, 160, 0.1)', padding: '6px 10px', borderRadius: 'var(--radius-sm)' }}>
            <CheckCircle size={14} color="#00f5a0" />
            <span style={{ fontSize: 11, color: '#00f5a0', fontWeight: 600 }}>Statut : {selectedAsset.status}</span>
          </div>
        </div>
      )}
    </div>
  );
}
