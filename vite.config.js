import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import tailwindcss from '@tailwindcss/vite';

import { resolve } from 'path';

export default defineConfig({
  plugins: [
    react(),
    tailwindcss()
  ],
  optimizeDeps: {
    entries: ['index.html'],
    include: ['react', 'react-dom', 'three', 'lucide-react']
  },
  server: {
    port: 3000,
    open: true,
    watch: {
      ignored: [
        '**/third_party/**',
        '**/build/**',
        '**/dist/**',
        '**/engine/**',
        '**/editor/**',
        '**/tests/**',
        '**/tools/**',
        '**/exporters/**'
      ]
    }
  },
  build: {
    outDir: 'dist',
    sourcemap: false,
    rollupOptions: {
      input: resolve(__dirname, 'index.html')
    }
  }
});
