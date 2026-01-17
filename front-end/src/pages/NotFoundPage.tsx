import { Link } from "react-router-dom";
import { Home } from "lucide-react";

export default function NotFoundPage() {
    return (
        <main className="flex min-h-screen items-center justify-center bg-black px-4">
            <div className="max-w-lg text-center space-y-6">
                {/* 404 Error Code */}
                <div className="space-y-2">
                    <p className="text-sm font-semibold tracking-widest text-[#32FB00] uppercase">
                        Błąd 404
                    </p>

                    <h1 className="text-4xl md:text-5xl font-bold text-white">
                        Nie znaleziono strony
                    </h1>
                </div>

                {/* Description */}
                <p className="text-gray-400 text-base md:text-lg">
                    Strona, której szukasz, nie istnieje lub została usunięta.
                </p>

                {/* Back Button */}
                <div className="pt-2">
                    <Link
                        to="/"
                        className="inline-flex items-center gap-2 rounded-md border border-[#32FB00]/40 px-5 py-2.5 text-sm font-medium text-[#32FB00] hover:bg-[#32FB00]/10 hover:border-[#32FB00]/60 transition-all duration-200"
                    >
                        <Home className="h-4 w-4" />
                        Wróć na stronę główną
                    </Link>
                </div>
            </div>
        </main>
    );
}