import { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { getDatabases, createDatabase, dropDatabase } from "../api/database";
import type { QueryResponse } from "../api/database";
import { toast } from 'sonner';   // ← import

export default function DatabasePage() {
  const navigate = useNavigate();

  const [dbs, setDbs] = useState<string[]>([]);
  const [newDbName, setNewDbName] = useState("");
  const [loading, setLoading] = useState(false);

  const fetchDatabases = async () => {
    setLoading(true);
    try {
      const res: QueryResponse = await getDatabases();

      if (res.resType === "TABLE" && res.rows && res.cols) {
        const dbNames = res.rows.map((row) => String(row[0])).filter(Boolean);
        setDbs(dbNames);
      } else if (res.resType === "ERROR") {
        toast.error(res.message || "Błąd pobierania listy baz danych");
      } else {
        toast.error("Nieoczekiwany format odpowiedzi serwera");
      }
    } catch (err: any) {
      toast.error(err.response?.data?.message || "Błąd połączenia z serwerem");
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchDatabases();
  }, []);

  const handleCreate = async (e: React.FormEvent) => {
    e.preventDefault();
    const name = newDbName.trim();
    if (!name) {
      toast.error("Wpisz nazwę bazy danych");
      return;
    }

    const lowerName = name.toLowerCase();
    const alreadyExists = dbs.some(db => db.toLowerCase() === lowerName);

    if (alreadyExists) {
      toast.error(`Baza o nazwie "${name}" już istnieje`);
      return;
    }

    setLoading(true);
    try {
      await createDatabase(name);
    } catch (err: any) {
      toast.error(err.response?.data?.message || "Błąd podczas tworzenia bazy");
    } finally {
      toast.success(`Baza "${name}" została utworzona`);
      fetchDatabases();
      setNewDbName('');
      setLoading(false);
    }
  };

  const handleDelete = async (name: string) => {
    if (!window.confirm(`Na pewno usunąć bazę "${name}"?`)) return;

    setLoading(true);
    try {
      await dropDatabase(name);
    } catch (err: any) {
      toast.error(err.response?.data?.message || "Błąd podczas usuwania bazy");
    } finally {
      setLoading(false);
      fetchDatabases();
      toast.success(`Baza "${name}" została usunięta`);
    }
  };

  return (
    <div className="p-6 max-w-5xl mx-auto">
      <h1 className="text-3xl font-bold text-white mb-8">Bazy danych</h1>

      <div className="bg-[#1a1a1a] p-6 rounded-xl border border-white/10 mb-10">
        <h2 className="text-xl text-white font-semibold mb-4">Dodaj nową bazę danych</h2>

        <form onSubmit={handleCreate} className="flex gap-4">
          <input
            type="text"
            value={newDbName}
            onChange={(e) => setNewDbName(e.target.value)}
            placeholder="Nazwa nowej bazy..."
            className="flex-1 bg-black border border-white/20 rounded-lg px-4 py-2 text-white focus:outline-none focus:border-blue-500"
            disabled={loading}
          />
          <button
            type="submit"
            disabled={loading}
            className="px-6 py-2 bg-green-600 hover:bg-green-700 rounded-lg text-white font-medium disabled:opacity-50 transition"
          >
            {loading ? "Tworzenie..." : "Utwórz"}
          </button>
        </form>
      </div>

      <div className="bg-[#111] rounded-xl border border-white/10 overflow-hidden">
        {loading && dbs.length === 0 ? (
          <div className="p-8 text-center text-gray-400">Ładowanie...</div>
        ) : dbs.length === 0 ? (
          <div className="p-8 text-center text-gray-500">Brak baz danych</div>
        ) : (
          <ul className="divide-y divide-white/5">
            {dbs.map((name) => (
              <li
                key={name}
                className="flex items-center justify-between px-6 py-4 hover:bg-white/5 transition"
              >
                <button
                  onClick={() => navigate(`/database/${name}`)}
                  className="text-blue-400 hover:text-blue-300 font-medium text-left flex-1"
                >
                  {name}
                </button>

                <button
                  onClick={() => handleDelete(name)}
                  className="text-red-400 hover:text-red-300 p-2 rounded hover:bg-red-950/30 transition"
                  title="Usuń bazę"
                  disabled={loading}
                >
                  🗑️
                </button>
              </li>
            ))}
          </ul>
        )}
      </div>
    </div>
  );
}