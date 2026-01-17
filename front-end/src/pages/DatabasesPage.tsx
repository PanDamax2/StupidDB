// import { useState } from "react";


// export default function DatabasesPage() {
//   const { databases, addDatabase, deleteDatabase } = useDatabases();
//   const [newDb, setNewDb] = useState("");
//   const navigate = useNavigate();


//   const handleAdd = () => {
//     addDatabase(newDb);
//     setNewDb("");
//   };


//   return (
//     <div className="max-w-4xl mx-auto space-y-6">
//       <h1 className="text-3xl font-bold text-white">Bazy danych</h1>


//       <div className="flex gap-2">
//         <input
//           value={newDb}
//           onChange={(e) => setNewDb(e.target.value)}
//           placeholder="Nazwa nowej bazy"
//           className="flex-1 rounded-lg bg-black border border-white/10 px-4 py-2 text-white"
//         />
//         <button
//           onClick={handleAdd}
//           className="px-4 py-2 rounded-lg bg-green-600 hover:bg-green-700 text-white"
//         >
//           Dodaj
//         </button>
//       </div>


//       <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
//         {databases.map((db) => (
//           <div
//             key={db}
//             className="flex items-center justify-between bg-[#111] border border-white/10 rounded-xl p-4 hover:border-blue-500 transition cursor-pointer"
//             onClick={() => navigate(`/databases/${db}`)}
//           >
//             <div className="flex items-center gap-3">
//               <Database className="text-blue-400" />
//               <span className="text-white font-medium">{db}</span>
//             </div>


//             <button
//               onClick={(e) => {
//                 e.stopPropagation();
//                 deleteDatabase(db);
//               }}
//               className="text-red-400 hover:text-red-600"
//             >
//               <Trash2 />
//             </button>
//           </div>
//         ))}
//       </div>
//     </div>
//   );
// }