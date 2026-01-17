import { useState, useEffect } from "react";
import { useParams, useNavigate } from "react-router-dom";
import { toast } from "sonner";
import { sendQuery, useDatabase } from "../api/database";

export default function TableDataPage() {
  const { dbName, tableName } = useParams<{ dbName: string; tableName: string }>();
  const navigate = useNavigate();

  const [data, setData] = useState<any[]>([]);
  const [columns, setColumns] = useState<string[]>([]);
  const [loading, setLoading] = useState(false);

  const [newRowValues, setNewRowValues] = useState<string[]>([]);

  const [editingCell, setEditingCell] = useState<{ rowIdx: number; col: string } | null>(null);
  const [editValue, setEditValue] = useState("");

  const [newColName, setNewColName] = useState("");
  const [newColType, setNewColType] = useState("TEXT");

  const [newTableNameInput, setNewTableNameInput] = useState(tableName || "");

  const loadData = async () => {
    if (!dbName || !tableName) return;

    setLoading(true);
    try {
      await useDatabase(dbName);

      const desc = await sendQuery(`DESCRIBE ${tableName}`);
      let cols: string[] = [];
      if (desc.resType === "TABLE" && desc.rows) {
        cols = desc.rows.map((row: any) => String(row[0] || ""));
      }
      setColumns(cols);

      const select = await sendQuery(`SELECTALL ${tableName}`);
      if (select.resType === "TABLE" && select.rows) {
        const formatted = select.rows.map((row: any[]) =>
          cols.reduce((obj: any, col, i) => {
            obj[col] = row[i] ?? null;
            return obj;
          }, {})
        );
        setData(formatted);
      } else {
        setData([]);
      }

      // przygotuj pusty stan dla nowego wiersza
      setNewRowValues(cols.map(() => ""));
    } catch (err: any) {
      toast.error("Błąd ładowania danych");
      setData([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadData();
  }, []);

  const startEdit = (rowIdx: number, col: string, currentValue: any) => {
    setEditingCell({ rowIdx, col });
    setEditValue(currentValue ?? "");
  };

  const saveEdit = async () => {
    if (!editingCell) return;

    const { rowIdx, col } = editingCell;
    const row = data[rowIdx];
    const oldValue = row[col];

    if (String(editValue) === String(oldValue)) {
      setEditingCell(null);
      return;
    }

    try {
      const pkCol = columns[0];
      const pkValue = row[pkCol];

      const query = `UPDATE ${tableName} SET ${col}=${editValue} WHERE ${pkCol}=${pkValue}`;
      await sendQuery(query);

      toast.success("Zapisano zmianę");
      a
      setData((prev) =>
        prev.map((r, i) =>
          i === rowIdx ? { ...r, [col]: editValue } : r
        )
      );
    } catch (err) {
      toast.error("Błąd zapisu zmiany");
    } finally {
      setEditingCell(null);
      setEditValue("");
    }
  };

  const addRow = async () => {
    if (newRowValues.some((v) => v.trim() === "")) {
      toast.error("Wypełnij wszystkie pola");
      return;
    }

    const valuesStr = newRowValues.map((v) => `'${v}'`).join(", ");

    try {
      await sendQuery(`INSERTINTO ${tableName} VALUES (${valuesStr})`);
      toast.success("Wiersz dodany");
      setNewRowValues(columns.map(() => ""));
      loadData();
    } catch (err) {
      toast.error("Błąd dodawania wiersza");
    }
  };

  const deleteRow = async (row: any) => {
    if (!window.confirm("Usunąć wiersz?")) return;

    const pkCol = columns[0];
    const pkValue = row[pkCol];

    try {
      await sendQuery(`DELETE ${tableName} WHERE ${pkCol.split(" ")[0]}=${pkValue}`);
      toast.success("Wiersz usunięty");
      loadData();
    } catch (err) {
      toast.error("Błąd usuwania wiersza");
    }
  };

  const addColumn = async () => {
    if (!newColName.trim()) {
      toast.error("Podaj nazwę kolumny");
      return;
    }

    try {
      await sendQuery(`ADDCOLUMN ${tableName} ${newColName.trim()} ${newColType}`);
      toast.success("Kolumna dodana");
      setNewColName("");
      loadData();
    } catch (err) {
      toast.error("Błąd dodawania kolumny");
    }
  };

  const deleteColumn = async (colName: string) => {
    if (!window.confirm(`Usunąć kolumnę ${colName}?`)) return;

    try {
      await sendQuery(`DROPCOLUMN ${tableName} ${colName}`);
      toast.success("Kolumna usunięta");
      loadData();
    } catch (err) {
      toast.error("Błąd usuwania kolumny");
    }
  };

  const clearTable = async () => {
    if (!window.confirm("Wyczyścić całą tabelę?")) return;

    try {
      await sendQuery(`CLEARTABLE ${tableName}`);
      toast.success("Tabela wyczyszczona");
      loadData();
    } catch (err) {
      toast.error("Błąd czyszczenia tabeli");
    }
  };

  if (loading) return <div className="p-10 text-center">Ładowanie...</div>;

  return (
    <div className="p-6 max-w-6xl mx-auto text-white">
      <button
        onClick={() => navigate(`/database/${dbName}`)}
        className="mb-6 text-blue-400 hover:underline"
      >
        ← Powrót do listy tabel
      </button>

      <div className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold">
          {tableName} <span className="text-gray-500">({dbName})</span>
        </h1>

        <div className="flex gap-4">
          <div className="flex gap-2">
            <input
              value={newTableNameInput}
              onChange={(e) => setNewTableNameInput(e.target.value)}
              className="bg-black border border-gray-700 px-3 py-1 rounded"
              placeholder="Nowa nazwa"
            />
            <button
              onClick={async () => {
                if (newTableNameInput === tableName) return;
                try {
                  await sendQuery(`MODIFYTABLE ${tableName} ${newTableNameInput}`);
                  toast.success("Nazwa zmieniona");
                  navigate(`/database/${dbName}/${newTableNameInput}`);
                } catch {
                  toast.error("Błąd zmiany nazwy");
                }
              }}
              className="bg-purple-700 px-4 py-1 rounded hover:bg-purple-800"
            >
              Zmień nazwę
            </button>
          </div>

          <button
            onClick={clearTable}
            className="bg-red-700 px-4 py-2 rounded hover:bg-red-800"
          >
            Wyczyść tabelę
          </button>
        </div>
      </div>

      <div className="bg-gray-900 p-4 rounded mb-6">
        <h3 className="mb-2 font-semibold">Dodaj kolumnę</h3>
        <div className="flex gap-3">
          <input
            value={newColName}
            onChange={(e) => setNewColName(e.target.value)}
            placeholder="Nazwa kolumny"
            className="flex-1 bg-black border border-gray-700 px-4 py-2 rounded"
          />
          <select
            value={newColType}
            onChange={(e) => setNewColType(e.target.value)}
            className="bg-black border border-gray-700 px-3 py-2 rounded"
          >
            <option>TEXT</option>
            <option>INT</option>
            <option>REAL</option>
          </select>
          <button onClick={addColumn} className="bg-green-700 px-5 py-2 rounded hover:bg-green-800">
            Dodaj
          </button>
        </div>
      </div>

      <div className="overflow-x-auto mb-10">
        <table className="w-full border-collapse bg-gray-900">
          <thead>
            <tr className="bg-gray-800 flex justify-between">
              <th className="p-3 text-left">Akcje</th>
              {columns.map((col) => (
                <th key={col} className="p-3 text-left flex items-center gap-2">
                  {col}
                  <button
                    onClick={() => deleteColumn(col)}
                    className="text-red-400 hover:text-red-300 text-sm"
                  >
                    ✕
                  </button>
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {data.length === 0 ? (
              <tr>
                <td colSpan={columns.length + 1} className="p-6 text-center text-gray-500">
                  Brak wierszy
                </td>
              </tr>
            ) : (
              data.map((row, rowIdx) => (
                <tr key={rowIdx} className="flex justify-between border-t border-gray-800 hover:bg-gray-800/50">
                  <td className="p-3">
                    <button
                      onClick={() => deleteRow(row)}
                      className="text-red-400 hover:text-red-300"
                    >
                      Usuń
                    </button>
                  </td>
                  {columns.map((col) => (
                    <td key={col} className="p-3">
                      {editingCell?.rowIdx === rowIdx && editingCell.col === col ? (
                        <input
                          value={editValue}
                          onChange={(e) => setEditValue(e.target.value)}
                          onBlur={saveEdit}
                          onKeyDown={(e) => e.key === "Enter" && saveEdit()}
                          autoFocus
                          className="bg-gray-800 border border-blue-500 px-2 py-1 w-full rounded"
                        />
                      ) : (
                        <div
                          onClick={() => startEdit(rowIdx, col, row[col])}
                          className="cursor-pointer min-h-[1.5rem]"
                        >
                          {row[col] ?? <span className="text-gray-600">NULL</span>}
                        </div>
                      )}
                    </td>
                  ))}
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      <div className="bg-gray-900 p-6 rounded">
        <h3 className="mb-4 font-semibold">Dodaj nowy wiersz</h3>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4 mb-4">
          {columns.map((col, idx) => (
            <input
              key={col}
              value={newRowValues[idx]}
              onChange={(e) => {
                const copy = [...newRowValues];
                copy[idx] = e.target.value;
                setNewRowValues(copy);
              }}
              placeholder={col}
              className="bg-black border border-gray-700 px-4 py-2 rounded"
            />
          ))}
        </div>
        <button
          onClick={addRow}
          className="bg-blue-700 px-6 py-2 rounded hover:bg-blue-800"
        >
          Dodaj wiersz
        </button>
      </div>
    </div>
  );
}