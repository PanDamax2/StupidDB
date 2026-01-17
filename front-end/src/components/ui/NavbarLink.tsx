import { Link, useLocation } from "react-router-dom";
import { cn } from "../../utils/cn";
import type { NavbarLinkProps } from "../../types/components";

export default function NavbarLink({ item, onClick, className }: NavbarLinkProps) {
  const location = useLocation();
  const isActive = location.pathname === item.to;


  return (
    <Link
      to={item.to}
      onClick={onClick}
      className={cn(
        "relative px-4 py-2 rounded-lg font-medium transition-all text-white duration-200",
        "flex items-center gap-2",
        isActive
          ? "bg-gradient-to-r from-cyan-700 via-cyan-800 to-cyan-900  shadow-inner"
          : "hover:text-white hover:bg-cyan-900",
        className
      )}
    >
      <span
        className={cn(
          "absolute left-0 top-1/2 -translate-y-1/2 h-5 w-1 rounded-r transition-opacity text-md",
          isActive ? " opacity-100" : "opacity-0"
        )}
      />
      {item.name}
    </Link>
  );
}