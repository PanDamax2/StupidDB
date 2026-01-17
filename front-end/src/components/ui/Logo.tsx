import { Link } from "react-router-dom";
import { SITE_NAME } from "../../constants/site";

export default function Logo() {
    return (
        <Link to="/" className="flex items-center gap-2
                 transition-transform duration-300 ease-in-out hover:scale-110 group" aria-label="Strona główna">

            <h1 className="text-2xl font-bold text-white">{SITE_NAME}</h1>

        </Link>
    );
}