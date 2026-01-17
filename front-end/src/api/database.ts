import api from "./client";

export interface QueryResponse {
  resType: "TABLE" | "ERROR" | "OK" | string;
  cols?: string[];
  rows?: any[][];
  message?: string;
}

export const sendQuery = async (query: string): Promise<QueryResponse> => {
  const res = await api.post<QueryResponse>("/query", { query });
  return res.data;
};

export const getDatabases = () => sendQuery("SHOWDATABASES");

export const createDatabase = (name: string) =>{
 sendQuery(`USE "${name}"`);
 sendQuery("USENONE");
}

export const dropDatabase = (name: string) =>
  sendQuery(`DROPDATABASE "${name}"`);

export const useDatabase = async (name: string): Promise<void> => {
  await sendQuery(`USE "${name}"`);
};