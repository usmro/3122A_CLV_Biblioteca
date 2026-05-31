Biblioteca Aster 📚
Sistem de management pentru bibliotecă — backend C++ cu interfață web și bază de date SQLite.

Tehnologii

C++17 + cpp-httplib + nlohmann/json
SQLite3
HTML / CSS / JavaScript

Compilare
bash g++ src/server.cpp src/Database/Database.cpp Biblioteca/Biblioteca.cpp \
    Carte/Carte.cpp Autor/Autor.cpp Cladiri/Cladire.cpp \
    Utilizatori/Utilizator.cpp Utilizatori/Clienti/Client.cpp \
    Utilizatori/Voluntari/Voluntar.cpp Utilizatori/Angajati/Angajat.cpp \
    -I. -Ilib -lsqlite3 -lpthread -std=c++17 -O2 -o server

Pornire
bash./server

Accesează: http://localhost:8080/web/primapagina.html

Conturi de test
Tip        Email                  Parolă    
Client    maria@test.ro           test123
Director  director@aster.ro       test123 + admin123
Voluntar  radu.voluntar@aster.ro  test123