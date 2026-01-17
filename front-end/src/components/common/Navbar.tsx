import { useContext, useState } from "react";
import { Menu, X, Database } from "lucide-react";
import { navigation } from "../../constants/navigation";
import Logo from "../ui/Logo";
import NavbarLink from "../ui/NavbarLink";
import { AuthContext } from "../../context/AuthContext";


export default function Navbar() {
  const [isOpen, setIsOpen] = useState(false);
  const { isAuthenticated, logout } = useContext(AuthContext);

  return (
    <nav className="sticky top-0 z-50 bg-[#0D0F12]/80 backdrop-blur border-b border-white/10">
      <div className="container mx-auto px-4 h-14 flex items-center justify-between">
        <div className="flex items-center gap-3">
          <Database className="text-green-400" />
          <Logo />
        </div>


        <div className="hidden md:flex items-center gap-1">
          {navigation.map((item) => (
            <NavbarLink key={item.name} item={item} onClick={() => setIsOpen(false)} />
          ))}

          {isAuthenticated && (
            <button
              onClick={logout}
              className="ml-4 px-4 py-1.5 rounded-lg text-sm font-medium
               bg-red-600/20 text-red-400 hover:bg-red-600/30
               border border-red-500/30 transition"
            >
              Wyloguj
            </button>
          )}
        </div>

        <button
          onClick={() => setIsOpen(!isOpen)}
          className="md:hidden rounded-lg p-2 text-gray-300 hover:bg-white/5"
        >
          {isOpen ? <X size={20} /> : <Menu size={20} />}
        </button>
      </div>


      {isOpen && (
        <div className="md:hidden px-4 pb-4 space-y-1 bg-[#0D0F12] border-t border-white/10">
          {navigation.map((item) => (
            <NavbarLink key={item.name} item={item} onClick={() => setIsOpen(false)} />
          ))}

          {isAuthenticated && (
            <button
              onClick={() => {
                logout();
                setIsOpen(false);
              }}
              className="w-full mt-3 px-4 py-2 rounded-lg text-sm font-medium
               bg-red-600/20 text-red-400 hover:bg-red-600/30
               border border-red-500/30 transition"
            >
              Wyloguj
            </button>
          )}
        </div>
      )}
    </nav>
  );
}