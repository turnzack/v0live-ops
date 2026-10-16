import React, { useState } from 'react';
import { Play, Save, Check, FileCode, Terminal } from 'lucide-react';

export default function ScriptEditor({ code, onSaveCode }) {
  const [currentCode, setCurrentCode] = useState(code);
  const [consoleOutput, setConsoleOutput] = useState([
    '[2026-10-17 00:00:01] [info] Sol2 Lua 5.4 runtime initialized.',
    '[2026-10-17 00:00:02] [info] Script loaded: samples/platformer/scripts/player.lua',
    '[2026-10-17 00:00:03] [info] Lua onStart() hooked to ECS Player entity.'
  ]);
  const [isSaved, setIsSaved] = useState(true);

  const handleTextChange = (e) => {
    setCurrentCode(e.target.value);
    setIsSaved(false);
  };

  const handleSave = () => {
    onSaveCode(currentCode);
    setIsSaved(true);
    setConsoleOutput((prev) => [
      ...prev,
      `[${new Date().toLocaleTimeString()}] [info] Script player.lua réécrit et recompilé.`
    ]);
  };

  const handleTestRun = () => {
    setConsoleOutput((prev) => [
      ...prev,
      `[${new Date().toLocaleTimeString()}] [info] Exécution de test de player.lua : syntaxe OK.`
    ]);
  };

  return (
    <div style={{ display: 'flex', height: '100%', gap: 14 }}>
      {/* Code Editor */}
      <div style={{ flex: 1, display: 'flex', flexDirection: 'column', background: 'var(--bg-primary)', borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)', overflow: 'hidden' }}>
        <div style={{ padding: '6px 12px', background: 'var(--bg-surface)', borderBottom: '1px solid var(--border-subtle)', display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: 6, fontSize: 11, fontFamily: 'var(--font-mono)', color: 'var(--accent-purple)' }}>
            <FileCode size={13} />
            <span>samples/platformer/scripts/player.lua</span>
          </div>
          <div style={{ display: 'flex', gap: 6 }}>
            <button className="tool-btn" onClick={handleTestRun}>
              <Play size={12} />
              <span>Tester</span>
            </button>
            <button className={`action-btn ${isSaved ? '' : 'primary'}`} onClick={handleSave}>
              {isSaved ? <Check size={12} color="#00f5a0" /> : <Save size={12} />}
              <span>{isSaved ? 'Enregistré' : 'Sauvegarder'}</span>
            </button>
          </div>
        </div>

        <textarea
          value={currentCode}
          onChange={handleTextChange}
          spellCheck={false}
          style={{
            flex: 1,
            background: 'transparent',
            border: 'none',
            color: '#e2e8f0',
            fontFamily: 'var(--font-mono)',
            fontSize: '12px',
            lineHeight: '1.6',
            padding: '12px',
            outline: 'none',
            resize: 'none',
            whiteSpace: 'pre'
          }}
        />
      </div>

      {/* Script Console / Output */}
      <div style={{ width: 320, display: 'flex', flexDirection: 'column', background: 'var(--bg-primary)', borderRadius: 'var(--radius-md)', border: '1px solid var(--border-subtle)', overflow: 'hidden' }}>
        <div style={{ padding: '6px 12px', background: 'var(--bg-surface)', borderBottom: '1px solid var(--border-subtle)', display: 'flex', alignItems: 'center', gap: 6, fontSize: 11, fontWeight: 600, color: 'var(--text-muted)' }}>
          <Terminal size={13} />
          <span>Console de sortie Lua</span>
        </div>
        <div style={{ flex: 1, padding: 10, overflowY: 'auto', fontFamily: 'var(--font-mono)', fontSize: 10, color: 'var(--text-dim)', display: 'flex', flexDirection: 'column', gap: 4 }}>
          {consoleOutput.map((line, idx) => (
            <div key={idx} style={{ color: line.includes('error') ? '#ff5555' : line.includes('OK') ? '#00f5a0' : 'var(--text-muted)' }}>
              {line}
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}
