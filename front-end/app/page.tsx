'use client';

import { Terminal } from 'lucide-react';

export default function Home() {
  return (
    <div className="flex min-h-screen items-center justify-center bg-zinc-50 font-sans dark:bg-black">
      <main className=" bg-white dark:bg-black">



      {/* <div className="min-h-screen bg-black text-green-400 font-mono p-8">
        <div className="max-w-4xl mx-auto">
          <h1 className="text-2xl mb-6 flex items-center gap-3">
            <Terminal className="w-6 h-6" /> Terminal Bazy Danych
          </h1>

          <div className="bg-gray-900 rounded-lg p-4 h-96 overflow-y-auto mb-4 font-mono text-sm">
             {terminalOutput.map((line, i) => (
              <div key={i}>{line}</div>
            ))} 
          </div>

          <div className="flex gap-2">
            <input
              value={terminalInput}
              onChange={e => setTerminalInput(e.target.value)}
              onKeyPress={e => e.key === 'Enter' && runCommand()}
              placeholder="Wpisz komendę..."
              className="flex-1 bg-gray-800 text-green-400 px-4 py-2 rounded border border-green-700 focus:outline-none focus:ring-2 focus:ring-green-500"
            />
            <button onClick={runCommand} className="bg-green-600 hover:bg-green-700 px-6 py-2 rounded">
              Wyślij
            </button>
          </div>

          <button onClick={() => setView(currentDB ? 'db' : 'home')} className="mt-6 text-gray-500 hover:text-white">
            ← Wróć
          </button>
        </div>
      </div> */}
        
      </main>
    </div>
  );
}
