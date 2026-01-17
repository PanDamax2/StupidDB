import { useState, useEffect } from "react";
import { useParams, useNavigate } from "react-router-dom";
import { sendQuery } from "../api/database";
import { toast } from 'sonner';

export default function DatabaseTablesPage() {
  const { dbName } = useParams<{ dbName: string }>();
  const navigate = useNavigate();

  const [tables, setTables] = useState<string[]>([]);
  const [loading, setLoading] = useState(false);
  const [newTableName, setNewTableName] = useState("");

  const fetchTables = async () => {
    if (!dbName) return;

    setLoading(true);
    try {
      await sendQuery(`USE "${dbName}"`);

      const res = await sendQuery("SHOWTABLES");

      if (!res.rows) {
        toast.warning(`Brak tabel w bazie ${dbName}`);
      } else {


        if (res.resType === "TABLE" && res.rows) {
          const tableNames = res.rows
            .flat()
            .filter(Boolean)
            .map(String);
          setTables(tableNames);
        } else if (res.resType === "ERROR") {
          toast.error(res.message || `Błąd pobierania tabel z bazy ${dbName}`);
        } else {
          toast.error("Nieoczekiwany format odpowiedzi");
        }
      }
    } catch (err: any) {
      toast.error(err.response?.data?.message || "Błąd połączenia");
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchTables();
  }, []);

  const handleCreateTable = async (e: React.FormEvent) => {
    e.preventDefault();
    const name = newTableName.trim();
    if (!name) {
      toast.error("Wpisz nazwę tabeli");
      return;
    }

    if (tables.some(t => t.toLowerCase() === name.toLowerCase())) {
      toast.error(`Tabela "${name}" już istnieje`);
      return;
    }

    setLoading(true);
    try {
      const createQuery = `CREATETABLE ${name}`;
      await sendQuery(createQuery);

    } catch (err: any) {
      toast.error(err.response?.data?.message || "Błąd serwera");
    } finally {
      toast.success(`Tabela "${name}" została utworzona`);
      fetchTables();
      setNewTableName("");
      setLoading(false);
    }
  };

  const handleDeleteTable = async (table: string) => {
    if (!window.confirm(`Usunąć tabelę "${table}" z bazy ${dbName}?`)) return;

    setLoading(true);
    try {
      await sendQuery(`DROPTABLE ${table}`);
    } catch (err: any) {
      toast.error(err.response?.data?.message || "Błąd serwera");
    } finally {
      fetchTables();
      toast.success(`Tabela "${table}" usunięta`);
      setLoading(false);
    }
  };

  return (
    <div className="p-6 max-w-5xl mx-auto text-white">
      <button
        onClick={() => navigate("/database")}
        className="mb-6 text-blue-400 hover:text-blue-300 flex items-center gap-2"
      >
        ← Powrót do listy baz
      </button>

      <h1 className="text-3xl font-bold mb-2">Baza: {dbName}</h1>
      <h2 className="text-xl text-gray-400 mb-8">Tabele</h2>

      <div className="bg-[#1a1a1a] p-6 rounded-xl border border-white/10 mb-10">
        <h3 className="text-lg font-semibold mb-4">Dodaj nową tabelę</h3>
        <form onSubmit={handleCreateTable} className="flex gap-4">
          <input
            type="text"
            value={newTableName}
            onChange={(e) => setNewTableName(e.target.value)}
            placeholder="Nazwa nowej tabeli..."
            className="flex-1 bg-black border border-white/20 rounded-lg px-4 py-2 text-white focus:outline-none focus:border-blue-500"
            disabled={loading}
          />
          <button
            type="submit"
            disabled={loading}
            className="px-6 py-2 bg-green-600 hover:bg-green-700 rounded-lg text-white font-medium disabled:opacity-50"
          >
            {loading ? "Tworzenie..." : "Utwórz"}
          </button>
        </form>
      </div>

      {/* Lista tabel */}
      <div className="bg-[#111] rounded-xl border border-white/10 overflow-hidden">
        {loading && tables.length === 0 ? (
          <div className="p-8 text-center text-gray-400">Ładowanie tabel...</div>
        ) : tables.length === 0 ? (
          <div className="p-8 text-center text-gray-500">Brak tabel w tej bazie</div>
        ) : (
          <ul className="divide-y divide-white/5">
            {tables.map((table) => (
              <li
                key={table}
                className="flex items-center justify-between px-6 py-4 hover:bg-white/5"
              >
                <button
                  onClick={() => navigate(`/database/${dbName}/${table}`)}
                  className="text-blue-400 hover:text-blue-300 font-medium flex-1 text-left"
                >
                  {table}
                </button>

                <button
                  onClick={() => handleDeleteTable(table)}
                  className="text-red-400 hover:text-red-300 p-2 rounded hover:bg-red-950/30"
                  title="Usuń tabelę"
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