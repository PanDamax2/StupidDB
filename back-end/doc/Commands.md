# 📘 StupidDB – Podstawowe komendy

Prosty zestaw komend niezbędnych do pracy z bazą danych **StupidDB**.
Każda komenda kończy się średnikiem `;`

---

## 🗄️ Zarządzanie bazami danych

| Komenda           | Opis                                                              |
| ----------------- | ----------------------------------------------------------------- |
| `SHOW DATABASES;` | Wyświetla listę wszystkich dostępnych baz danych                  |
| `USE nazwa_bazy;` | Przełącza na wskazaną bazę danych (tworzy ją, jeśli nie istnieje) |

**Przykład:**

```
USE szkolna;
SHOW DATABASES;
```

---

## 📋 Zarządzanie tabelami (DDL)

| Komenda                                                 | Opis                                       |
| ------------------------------------------------------- | ------------------------------------------ |
| `SHOW TABLES;`                                          | Pokazuje wszystkie tabele w bieżącej bazie |
| `CREATE TABLE nazwa (kolumna1 TYP, kolumna2 TYP, ...);` | Tworzy nową tabelę                         |
| `DROP TABLE nazwa;`                                     | Usuwa tabelę z bazy                        |
| `DESCRIBE nazwa;` lub `DESC nazwa;`                     | Wyświetla strukturę tabeli                 |

**Przykład:**

```
CREATE TABLE uczniowie (id INT, imie VARCHAR(50), wiek INT);
SHOW TABLES;
DESC uczniowie;
```

---

## 📊 Operacje na danych (DML)

| Komenda                                                      | Opis                              |
| ------------------------------------------------------------ | --------------------------------- |
| `INSERT INTO tabela VALUES (wartosc1, wartosc2, ...);`       | Wstawia nowy wiersz               |
| `SELECT * FROM tabela;`                                      | Wyświetla wszystkie dane z tabeli |
| `SELECT * FROM tabela WHERE kolumna=wartosc;`                | Wyświetla tylko pasujące wiersze  |
| `UPDATE tabela SET kolumna=wartosc WHERE kolumna2=wartosc2;` | Aktualizuje dane                  |
| `DELETE FROM tabela WHERE kolumna=wartosc;`                  | Usuwa wskazane wiersze            |

**Przykład:**

```
INSERT INTO uczniowie VALUES (1, 'Anna', 18);
SELECT * FROM uczniowie;
UPDATE uczniowie SET wiek=19 WHERE id=1;
DELETE FROM uczniowie WHERE id=1;
```

---

## ⚙️ Komendy systemowe

| Komenda             | Opis            |
| ------------------- | --------------- |
| `HELP;` lub `/HELP` | Wyświetla pomoc |
| `CLEAR;` lub `CLS;` | Czyści ekran    |
| `EXIT;` lub `QUIT;` | Zamyka program  |

---

## 🔒 Użytkownicy (opcjonalnie)

| Komenda                      | Opis                                 |
| ---------------------------- | ------------------------------------ |
| `CHANGEUSERNAME nowa_nazwa;` | Zmienia nazwę użytkownika            |
| `CHANGEPASSWORD nowe_haslo;` | Zmienia hasło aktualnego użytkownika |

---

## 💾 Struktura katalogów

```
db/
└── szkolna/               # Baza danych
    ├── db.stdb            # Plik metadanych bazy
    └── tables/
        ├── uczniowie.sttb # Dane tabeli uczniowie
        ├── klasy.sttb
        └── nauczyciele.sttb
```

---

## 📚 Typy danych obsługiwane przez StupidDB

| Typ          | Opis                                |
| ------------ | ----------------------------------- |
| `INT`        | Liczba całkowita (32-bit)           |
| `FLOAT`      | Liczba zmiennoprzecinkowa           |
| `VARCHAR(n)` | Tekst o maksymalnej długości `n`    |
| `BOOL`       | Wartość logiczna (`true` / `false`) |

---

## 🧠 Wskazówki

* Każda komenda **musi kończyć się średnikiem `;`**
* Nazwy tabel i kolumn nie mogą zawierać spacji ani znaków specjalnych
* Możesz mieć wiele baz danych w folderze `db/`
* Domyślnie dane zapisywane są w formacie binarnym `.sttb`

---

> ✨ Przykład szybkiego użycia:

```
USE szkolna;
CREATE TABLE uczniowie (id INT, imie VARCHAR(50), wiek INT);
INSERT INTO uczniowie VALUES (1, 'Jan', 17);
SELECT * FROM uczniowie;
```
