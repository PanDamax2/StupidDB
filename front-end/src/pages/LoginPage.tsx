import { useContext, useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { login as apiLogin } from "../api/auth";
import axios from "axios";
import { AuthContext } from "../context/AuthContext";

export default function LoginPage() {
  const { isAuthenticated, login } = useContext(AuthContext)!;
  const navigate = useNavigate();
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    if (isAuthenticated) {
      navigate("/database", { replace: true });
    }
  }, [isAuthenticated, navigate]);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError("");
    setLoading(true);

    if (password === "") {
      setError("Wpisz hasło");
      setLoading(false);
      return;
    }

    try {
      const res = await apiLogin(password);

      if (res.resType === "TOKEN") {
        login(res.token);
      } else {
        setError(res.message);
      }
    } catch (err) {
      if (axios.isAxiosError(err)) {
        setError(err.response?.data?.message || "Błąd serwera");
      } else if (err instanceof Error) {
        setError(err.message);
      } else {
        setError("Nieznany błąd");
      }
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="flex items-center justify-center min-h-[70vh]">
      <div className="w-full max-w-sm bg-[#111] rounded-2xl shadow-xl p-8 border border-white/10">
        <h2 className="text-2xl font-bold text-white mb-6 text-center">Logowanie</h2>

        <span className="block text-gray-400 text-center text-md mb-3">
          Domyślne hasło to: <b>admin</b>
        </span>

        <form className="space-y-4" onSubmit={handleSubmit}>
          <input
            type="password"
            placeholder="Wpisz hasło"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            className="w-full rounded-lg bg-black border border-white/10 px-4 py-2 text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
          />

          {error && <p className="text-red-500 text-sm">{error}</p>}

          <button
            type="submit"
            disabled={loading}
            className="w-full py-2 rounded-lg bg-blue-600 hover:bg-blue-700 transition text-white font-semibold disabled:opacity-50"
          >
            {loading ? "Logowanie..." : "Zaloguj się"}
          </button>
        </form>
      </div>
    </div>
  );
}