import { createBrowserRouter, RouterProvider } from 'react-router-dom'
import MainLayout from './components/layout/MainLayout';
import HomePage from './pages/HomePage';
import NotFoundPage from './pages/NotFoundPage';
import LoginPage from './pages/LoginPage';
import { RequireAuth } from './context/RequireAuth';
import DatabasePage from './pages/DatabasePage';
import { AuthProvider } from './context/AuthContext';
import DatabaseTablesPage from './pages/DatabaseTablesPage';
import TableDataPage from './pages/TableDataPage';

const router = createBrowserRouter([
  {
    path: '/',
    element: <MainLayout />,
    children: [
      {
        path: '/',
        element: <HomePage />,
      },
      {
        path: '/login',
        element: <LoginPage />
      },
      {
        path: '/database',
        element: <RequireAuth />,
        children: [
          {
            path: '',
            element: <DatabasePage />,
          },
          {
            path: ':dbName',
            element: <DatabaseTablesPage />,
          },
          {
            path: ':dbName/:tableName',
            element: <TableDataPage />,
          },
        ]
      }
    ],
  },
  {
    path: '*',
    element: <NotFoundPage />,
  },
]);

export default function AppRouter() {
  return <AuthProvider>
    <RouterProvider router={router} />
  </AuthProvider>
}