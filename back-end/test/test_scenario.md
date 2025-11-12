# Scenariusz testowy — baza danych **schooldb**

## A. Start / pomoc / środowisko

**HELP**  
Powinno wypisać listę komend.

**SHOWDATABASES**  
Powinno pokazać istniejące bazy lub pustą listę.

**USE schooldb**  
Przełącza na / tworzy bazę `schooldb`.
Nazwa bazy powinna składać się z małych liter.

**SHOWTABLES**  
Powinno pokazać brak tabel lub listę.

---

## B. Tworzenie tabeli i struktury

**CREATETABLE users**  
Tworzy pustą tabelę `users`

**CREATETABLE students (id INT, name VARCHAR(50), age INT)**  
Tworzy tabelę `students` z trzema kolumnami.

**SHOWTABLES**  
Powinno pokazać tabele `students`.

**DESCRIBE students** _(albo DESC students)_  
Powinien wydrukować strukturę kolumn (id, name, age) i statystyki.

**ADDCOLUMN students email VARCHAR(50)**  
Dodaje kolumnę `email` — jeśli tabela ma już wiersze, operacja się nie powiedzie; teraz tabela jest pusta, więc powinna się powieść.

**DESCRIBE students**  
Sprawdź, że pojawiła się kolumna `email`.

---

## C. Wstawianie i odczyt (DML)

**INSERTINTO students VALUES (1, 'Anna', 20, 'anna@example.com')**  
**INSERTINTO students VALUES (2, 'Bartek', 22, 'bartek@x.com')**  
**INSERTINTO students VALUES (3, 'Celina', 19, 'celina@y.pl')**  
Dla każdego powinien się pojawić komunikat o wstawieniu i ID.
(wartościu musi być zgodne z kolumnami, jesli nie chesz dać pozotaw puste)

**SELECTALL students**  
Powinny się wypisać wszystkie wiersze (3 rekordy).

**SELECT students WHERE name=Anna**  
Powinien pokazać tylko rekordy z name = Anna.

**SELECT students WHERE age=22**  
Powinien pokazać Bartek.

**selectall students** _(małymi literami)_  
Sprawdź case-insensitive: wynik taki sam jak wyżej.

---

## D. Aktualizacje i usuwanie

**UPDATE students SET age=21 WHERE name=Anna**  
Zaktualizuje wiek Anny; log operacji.

**SELECT students WHERE name=Anna**  
Sprawdź, że age = 21.

**DELETE students WHERE id=2**  
Usunie Bartek (id=2).

**SELECTALL students**  
Powinno być teraz 2 rekordy (Anna i Celina).

**CLEARTABLE students**  
Usuwa wszystkie dane z tabeli.

**SELECTALL students**  
Powinno zwrócić pustą tabelę (0 wierszy).

---

## E. Testy strukturalne po danych (edge cases)

**INSERTINTO students VALUES (4, 'Dawid', 25, 'd@x.com')**  
Wstaw nowy wiersz (tabela nie jest pusta).

**ADDCOLUMN students phone VARCHAR(20)**  
Powinno nie pozwolić dodać kolumny, jeśli w tabeli są dane (oczekiwany błąd/log).

**DROPCOLUMN students email**  
Spróbuj usunąć kolumnę `email` —  sprawdź `DESCRIBE`.

**MODIFYCOLUMN students name fullname**  
Zmień nazwę kolumny `name` → `fullname`.

**MODIFYTABLE students pupils**  
Zmień nazwę tabeli `students` na `pupils`.

**SHOWTABLES**  
Powinieneś zobaczyć `pupils`.

**DESCRIBE pupils**  
Sprawdź strukturę po zmianie nazwy.

---

## F. Testy DROP / RENAME bazy i tabel

**DROPTABLE pupils**  
Usuwa tabelę `pupils`.

**SHOWTABLES**  
Powinno być pusto w bazie `schooldb`.

**CHANGEDBNAME schooldb school_db_new**  
Zmień nazwę bazy danych.

**SHOWDATABASES**  
Sprawdź, że jest `school_db_new`.

**USE school_db_new**  
Przełącz na nową nazwę.

**DROPDATABASE school_db_new**  
Usuń całą bazę `school_db_new`.

**SHOWDATABASES**  
Sprawdź, że `school_db_new` zniknęła.

---

## G. Testy narzędziowe / help / exit

**HELP**  
Jeszcze raz — powinna być pomoc.

**LOGOUT**  
Wyloguj z bazy (jeśli zaimplementowane).

**EXIT**  
Zamknij program.

---

## H. Testy błędów / odporności parsera

Niepoprawne komendy:

- `INSERT INTO` (bez VALUES)
- `CREATETABLE badsyntax id INT)`
- `SELECT pupils WHERE`
- `SOMECMD abc`

Powinieneś dostać komunikaty „Unknown command” lub ostrzeżenia parsera.

Test _case-insensitive_:

- `insertinto pupils values (1, 'X', 10)`
- `ShowTables`
- `selectall PUPILS`

Wszystkie powinny działać.

Test zapisu/odczytu (_persistence_):  
Zamknij program (`EXIT`), uruchom ponownie, użyj `USE schooldb`, `SHOWTABLES`, `DESCRIBE pupils`, `SELECTALL` — dane powinny być zachowane.
