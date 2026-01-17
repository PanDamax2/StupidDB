import { SITE_NAME } from "../../constants/site";

export default function Footer() {
  const currentYear = new Date().getFullYear();

  return (
    <footer className="  py-2 text-center text-gray-300 mt-auto">
      <div className="container mx-auto px-4 space-y-4 max-w-3xl">
        {/* Copyright */}
        <p className="text-sm md:text-base">
          Copyright © {currentYear} {' '}
          <span className=" font-semibold">{SITE_NAME}</span>.
          Wszystkie prawa zastrzeżone.
        </p>
      </div>
    </footer>
  );
}
