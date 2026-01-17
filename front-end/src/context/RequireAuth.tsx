import { useContext } from 'react';
import { Navigate, Outlet } from 'react-router-dom';
import { AuthContext } from './AuthContext';

export function RequireAuth() {
  const context = useContext(AuthContext);
  if (!context) {
    throw new Error('AuthContext must be used within AuthProvider');
  }
  const { isAuthenticated } = context;

  return isAuthenticated ? <Outlet /> : <Navigate to="/login" replace />;
}