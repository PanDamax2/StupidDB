import api from "./client";

export interface LoginSuccess {
  resType: "TOKEN";
  token: string;
}

export interface LoginError {
  resType: "ERROR";
  message: string;
}

export type LoginResponse = LoginSuccess | LoginError;

export const login = async (password: string): Promise<LoginResponse> => {
  const res = await api.post<LoginResponse>("/login", { password });
  return res.data;
};
