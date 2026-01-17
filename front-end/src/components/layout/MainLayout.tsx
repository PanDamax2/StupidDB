import { Outlet } from 'react-router-dom';
import Navbar from '../common/Navbar';
import Footer from '../common/Footer';
import { Toaster } from 'sonner';

export default function Layout() {
  return (
    <div className="min-h-screen flex flex-col bg-[#0A0A0A]">
      <Navbar />

      <main className="flex-1 container mx-auto px-4 py-8">
        <Outlet />
      </main>

      <Toaster 
        position="top-center" 
        richColors 
        closeButton 
        theme="dark"
      />

      <Footer />
    </div>
  );
}