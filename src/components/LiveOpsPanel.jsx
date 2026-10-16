import React, { useState } from 'react';
import { Cloud, Globe, ArrowUpRight, CheckCircle2, Shield, Radio, Activity, RefreshCw } from 'lucide-react';

export default function LiveOpsPanel({ assets, sceneName }) {
  const [version, setVersion] = useState('1.0.4');
  const [isDeploying, setIsDeploying] = useState(false);
  const [deploymentLogs, setDeploymentLogs] = useState([
    { id: 1, ver: 'v1.0.3', date: '2026-10-16 19:30', status: 'Active sur CDN', hash: '5d49ac4' },
    { id: 2, ver: 'v1.0.2', date: '2026-10-15 14:15', status: 'Archivé S3', hash: '42efbe7' }
  ]);

  const handlePublishRelease = () => {
    setIsDeploying(true);
    setTimeout(() => {
      setIsDeploying(false);
      setDeploymentLogs((prev) => [
        {
          id: Date.now(),
          ver: `v${version}`,
          date: new Date().toISOString().replace('T', ' ').slice(0, 16),
          status: 'Déployé sur Vercel Edge & S3',
          hash: Math.random().toString(16).slice(2, 9)
        },
        ...prev
      ]);
    }, 1200);
  };

  return (
    <div style={{ display: 'flex', height: '100%', gap: 16 }}>
      {/* Left: Deployment controls & Manifest */}
      <div style={{ flex: 1, display: 'flex', flexDirection: 'column', gap: 12 }}>
        <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
          <div>
            <div style={{ fontSize: 13, fontWeight: 700, color: 'var(--text-main)' }}>
              Gestionnaire de Manifestes & Publication Distante
            </div>
            <div style={{ fontSize: 11, color: 'var(--text-dim)' }}>
              Selon l'architecture v0live-ops : 'latest.json' pointeur dynamique CDN & artefacts immuables SHA-256.
            </div>
          </div>

          <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
            <span style={{ fontSize: 11, color: 'var(--text-muted)' }}>Prochaine version :</span>
            <input
              type="text"
              value={version}
              onChange={(e) => setVersion(e.target.value)}
              style={{
                width: 70,
                background: 'var(--bg-primary)',
                border: '1px solid var(--border-subtle)',
                color: 'var(--accent-cyan)',
                padding: '4px 8px',
                borderRadius: 'var(--radius-sm)',
                fontSize: 12,
                fontFamily: 'var(--font-mono)',
                textAlign: 'center'
              }}
            />
            <button
              className="action-btn primary"
              onClick={handlePublishRelease}
              disabled={isDeploying}
            >
              {isDeploying ? <RefreshCw size={13} className="spin" /> : <Cloud size={13} />}
              <span>{isDeploying ? 'Déploiement en cours...' : 'Publier Version'}</span>
            </button>
          </div>
        </div>

        {/* Live Metrics Grid */}
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(4, 1fr)', gap: 10 }}>
          <div style={{ background: 'var(--bg-surface)', padding: 12, borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: 6, color: 'var(--accent-emerald)', fontSize: 11, fontWeight: 600 }}>
              <Radio size={12} />
              <span>Joueurs CCU</span>
            </div>
            <div style={{ fontSize: 20, fontWeight: 800, marginTop: 4, color: '#fff' }}>1,248</div>
            <div style={{ fontSize: 10, color: 'var(--text-dim)', marginTop: 2 }}>Sessions temps réel</div>
          </div>

          <div style={{ background: 'var(--bg-surface)', padding: 12, borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: 6, color: 'var(--accent-cyan)', fontSize: 11, fontWeight: 600 }}>
              <Globe size={12} />
              <span>CDN Cache Hit</span>
            </div>
            <div style={{ fontSize: 20, fontWeight: 800, marginTop: 4, color: '#fff' }}>99.4%</div>
            <div style={{ fontSize: 10, color: 'var(--text-dim)', marginTop: 2 }}>Contabo S3 / Edge</div>
          </div>

          <div style={{ background: 'var(--bg-surface)', padding: 12, borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: 6, color: 'var(--accent-blue)', fontSize: 11, fontWeight: 600 }}>
              <Activity size={12} />
              <span>Latence Moyenne</span>
            </div>
            <div style={{ fontSize: 20, fontWeight: 800, marginTop: 4, color: '#fff' }}>14 ms</div>
            <div style={{ fontSize: 10, color: 'var(--text-dim)', marginTop: 2 }}>Vercel Edge Network</div>
          </div>

          <div style={{ background: 'var(--bg-surface)', padding: 12, borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: 6, color: 'var(--accent-emerald)', fontSize: 11, fontWeight: 600 }}>
              <Shield size={12} />
              <span>Taux de crash</span>
            </div>
            <div style={{ fontSize: 20, fontWeight: 800, marginTop: 4, color: '#fff' }}>0.00%</div>
            <div style={{ fontSize: 10, color: 'var(--text-dim)', marginTop: 2 }}>DoD-Prod validé</div>
          </div>
        </div>

        {/* Live Manifest Preview */}
        <div style={{ background: 'var(--bg-primary)', padding: 10, borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)', flex: 1, overflowY: 'auto' }}>
          <div style={{ fontSize: 10, color: 'var(--text-dim)', fontFamily: 'var(--font-mono)', marginBottom: 6 }}>
            PREVIEW DU MANIFESTE : latest.json
          </div>
          <pre style={{ fontSize: 11, fontFamily: 'var(--font-mono)', color: 'var(--accent-cyan)', lineHeight: 1.4 }}>
{`{
  "engine_version": "v0live-ops-sprint4",
  "build_target": "windows-x64 / webassembly-webgpu",
  "scene_entrypoint": "${sceneName}",
  "manifest_release": "manifest-v${version}.json",
  "assets_count": ${assets.length},
  "deployment_endpoint": "https://v0live-ops-874w-git-main-v01-e951.vercel.app/cdn/latest.json"
}`}
          </pre>
        </div>
      </div>

      {/* Right: History table */}
      <div style={{ width: 340, background: 'var(--bg-surface)', borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)', padding: 14, display: 'flex', flexDirection: 'column' }}>
        <div style={{ fontSize: 12, fontWeight: 700, color: 'var(--text-main)', marginBottom: 10 }}>
          Historique des Déploiements
        </div>
        <div style={{ flex: 1, overflowY: 'auto', display: 'flex', flexDirection: 'column', gap: 8 }}>
          {deploymentLogs.map((log) => (
            <div key={log.id} style={{ background: 'var(--bg-primary)', padding: '8px 10px', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-subtle)' }}>
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <span style={{ fontSize: 12, fontWeight: 700, color: 'var(--accent-cyan)' }}>{log.ver}</span>
                <span style={{ fontSize: 10, fontFamily: 'var(--font-mono)', color: 'var(--text-dim)' }}>commit {log.hash}</span>
              </div>
              <div style={{ fontSize: 11, color: 'var(--text-muted)', marginTop: 2 }}>{log.date}</div>
              <div style={{ display: 'flex', alignItems: 'center', gap: 4, marginTop: 4 }}>
                <CheckCircle2 size={11} color="#00f5a0" />
                <span style={{ fontSize: 10, color: '#00f5a0', fontWeight: 600 }}>{log.status}</span>
              </div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}
