#include "Database.h"
#include <fstream>
#include <sstream>

// ── Constructor / Destructor ──────────────────────────────────

Database::Database(const std::string& path) : db(nullptr), db_path(path) {
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "[DB] Eroare la deschidere: " << sqlite3_errmsg(db) << "\n";
        db = nullptr;
    } else {
        // Activeaza foreign keys si WAL pentru performanta
        exec("PRAGMA foreign_keys = ON;");
        exec("PRAGMA journal_mode = WAL;");
        std::cout << "[DB] Conectat la: " << path << "\n";
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
        std::cout << "[DB] Conexiune inchisa.\n";
    }
}

// ── Helpers ───────────────────────────────────────────────────

bool Database::exec(const std::string& sql) {
    char* err = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "[DB] Eroare SQL: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool Database::query(const std::string& sql,
                     std::function<void(sqlite3_stmt*)> callback) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "[DB] Eroare prepare: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        callback(stmt);
    }
    sqlite3_finalize(stmt);
    return true;
}

// Helper pentru a citi text din stmt fara crash la NULL
static std::string getText(sqlite3_stmt* s, int col) {
    const unsigned char* t = sqlite3_column_text(s, col);
    return t ? reinterpret_cast<const char*>(t) : "";
}

// ── Initializare schema ───────────────────────────────────────

bool Database::initializeaza(const std::string& schema_path) {
    std::ifstream f(schema_path);
    if (!f.is_open()) {
        std::cerr << "[DB] Nu gasesc schema: " << schema_path << "\n";
        return false;
    }
    std::stringstream ss;
    ss << f.rdbuf();
    bool ok = exec(ss.str());
    if (ok) std::cout << "[DB] Schema initializata.\n";
    return ok;
}

// ── Carti ─────────────────────────────────────────────────────

static CarteDB rowToCarte(sqlite3_stmt* s) {
    CarteDB c;
    c.id                    = sqlite3_column_int(s, 0);
    c.titlu                 = getText(s, 1);
    c.autor                 = getText(s, 2);
    c.editura               = getText(s, 3);
    c.limba                 = getText(s, 4);
    c.data_pub              = getText(s, 5);
    c.an_aparitie           = sqlite3_column_int(s, 6);
    c.nr_pagini             = sqlite3_column_int(s, 7);
    c.tara_provenienta      = getText(s, 8);
    c.tip                   = getText(s, 9);
    c.suport                = getText(s, 10);
    c.status                = getText(s, 11);
    c.sursa                 = getText(s, 12);
    c.destinatar            = getText(s, 13);
    c.serie_contabila       = getText(s, 14);
    c.exemplare_disponibile = sqlite3_column_int(s, 15);
    c.este_patrimoniu       = sqlite3_column_int(s, 16) != 0;
    c.pret_intrare          = (float)sqlite3_column_double(s, 17);
    c.rating                = (float)sqlite3_column_double(s, 18);
    c.rezumat               = getText(s, 19);
    c.id_cladire            = sqlite3_column_int(s, 20);
    c.id_sala               = sqlite3_column_int(s, 21);
    c.id_raft               = sqlite3_column_int(s, 22);
    c.cod_identificare      = getText(s, 23);
    c.valoare_masurabila    = sqlite3_column_int(s, 24);
    c.marime_mb             = (float)sqlite3_column_double(s, 25);
    c.isbn                  = getText(s, 26);
    c.gen_specific          = getText(s, 27);
    c.personaj_principal    = getText(s, 28);
    c.serie                 = getText(s, 29);
    c.tip_naratiune         = getText(s, 30);
    c.varsta_recomandata    = sqlite3_column_int(s, 31);
    c.dimensiuni            = getText(s, 32);
    c.greutate              = (float)sqlite3_column_double(s, 33);
    c.tip_coperta           = getText(s, 34);
    c.domeniu               = getText(s, 35);
    c.institutie_sursa      = getText(s, 36);
    c.editie_revizuita      = getText(s, 37);
    c.issn                  = getText(s, 38);
    c.numar_editie          = sqlite3_column_int(s, 39);
    c.frecventa             = getText(s, 40);
    return c;
}

const std::string SELECT_CARTI = 
    "SELECT id, titlu, autor, editura, limba, data_pub, an_aparitie, nr_pagini, "
    "tara_provenienta, tip, suport, status, sursa, destinatar, serie_contabila, "
    "exemplare_disponibile, este_patrimoniu, pret_intrare, rating, rezumat, "
    "id_cladire, id_sala, id_raft, cod_identificare, valoare_masurabila, marime_mb, "
    "isbn, gen_specific, personaj_principal, serie, tip_naratiune, varsta_recomandata, "
    "dimensiuni, greutate, tip_coperta, domeniu, institutie_sursa, editie_revizuita, "
    "issn, numar_editie, frecventa FROM carti";

std::vector<CarteDB> Database::getToateCartile() {
    std::vector<CarteDB> rezultat;
    query(SELECT_CARTI, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    return rezultat;
}

std::vector<CarteDB> Database::cautaDupaTitlu(const std::string& titlu) {
    std::vector<CarteDB> rezultat;
    
    // Intai cauta exact (cu litere mici)
    std::string sql = SELECT_CARTI + 
        " WHERE LOWER(titlu) LIKE LOWER('%" + titlu + "%')";
    query(sql, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    
    // Daca nu s-a gasit nimic, cauta pe cuvinte individuale
    if (rezultat.empty()) {
        // Imparte titlul in cuvinte
        std::vector<std::string> cuvinte;
        std::stringstream ss(titlu);
        std::string cuv;
        while (ss >> cuv) cuvinte.push_back(cuv);
        
        for (const auto& c : cuvinte) {
            if (c.length() < 3) continue; // ignora cuvinte prea scurte
            std::string sql2 = SELECT_CARTI + 
                " WHERE LOWER(titlu) LIKE LOWER('%" + c + "%')";
            query(sql2, [&](sqlite3_stmt* s) {
                CarteDB carte = rowToCarte(s);
                // Evita duplicate
                bool exista = false;
                for (const auto& r : rezultat)
                    if (r.id == carte.id) { exista = true; break; }
                if (!exista) rezultat.push_back(carte);
            });
        }
        
        if (!rezultat.empty())
            std::cout << "[Cautare] Nu am gasit exact. Titluri asemanatoare:\n";
    }
    
    return rezultat;
}

std::vector<CarteDB> Database::cautaDupaAutor(const std::string& autor) {
    std::vector<CarteDB> rezultat;
    std::string sql = SELECT_CARTI + " WHERE LOWER(autor) LIKE LOWER(?)";
    std::string param = "%" + autor + "%";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, param.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            rezultat.push_back(rowToCarte(stmt));
        }
        sqlite3_finalize(stmt);
    }
    return rezultat;
}

std::vector<CarteDB> Database::cautaDupaCladire(int id_cladire) {
    std::vector<CarteDB> rezultat;
    std::string sql = SELECT_CARTI + " WHERE id_cladire = " + std::to_string(id_cladire);
    query(sql, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    return rezultat;
}

std::vector<CarteDB> Database::filtreazaDupaRating(float rating_min) {
    std::vector<CarteDB> rezultat;
    std::string sql = SELECT_CARTI + " WHERE rating >= " + std::to_string(rating_min)
                      + " ORDER BY rating DESC";
    query(sql, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    return rezultat;
}

CarteDB Database::getCarteDupaId(int id) {
    CarteDB c; c.id = -1;
    std::string sql = SELECT_CARTI + " WHERE id = " + std::to_string(id);
    query(sql, [&](sqlite3_stmt* s) { c = rowToCarte(s); });
    return c;
}

bool Database::actualizeazaStocCarte(int id_carte, int stoc_nou) {
    return exec("UPDATE carti SET exemplare_disponibile = " + std::to_string(stoc_nou)
                + " WHERE id = " + std::to_string(id_carte));
}

bool Database::actualizeazaStatusCarte(int id_carte, const std::string& status) {
    return exec("UPDATE carti SET status = '" + status + "' WHERE id = " 
                + std::to_string(id_carte));
}

bool Database::stergeCarte(int id) {
    return exec("DELETE FROM carti WHERE id = " + std::to_string(id));
}

// ── Autori ────────────────────────────────────────────────────

std::vector<AutorDB> Database::getToatiAutorii() {
    std::vector<AutorDB> rezultat;
    query("SELECT id, nume_complet, biografie, cale_poza FROM autori",
    [&](sqlite3_stmt* s) {
        AutorDB a;
        a.id           = sqlite3_column_int(s, 0);
        a.nume_complet = getText(s, 1);
        a.biografie    = getText(s, 2);
        a.cale_poza    = getText(s, 3);
        rezultat.push_back(a);
    });
    return rezultat;
}

AutorDB Database::getAutorDupaNumele(const std::string& nume) {
    AutorDB a; a.id = -1;
    query("SELECT id, nume_complet, biografie, cale_poza FROM autori WHERE nume_complet LIKE '%" + nume + "%'",
    [&](sqlite3_stmt* s) {
        a.id           = sqlite3_column_int(s, 0);
        a.nume_complet = getText(s, 1);
        a.biografie    = getText(s, 2);
        a.cale_poza    = getText(s, 3);
    });
    return a;
}

// ── Clienti ───────────────────────────────────────────────────

std::vector<ClientDB> Database::getToatiClientii() {
    std::vector<ClientDB> rezultat;
    std::string sql = 
        "SELECT u.id, u.nume, u.prenume, u.email, u.telefon, u.data_nasterii, "
        "u.gen, u.parola_criptata, u.data_inscriere, u.status, u.cale_poza, "
        "c.username, c.rank, c.varsta, c.total_imprumuturi, c.data_inscriere_client "
        "FROM utilizatori u JOIN clienti c ON u.id = c.id_utilizator";
    query(sql, [&](sqlite3_stmt* s) {
        ClientDB c;
        c.id                  = sqlite3_column_int(s, 0);
        c.nume                = getText(s, 1);
        c.prenume             = getText(s, 2);
        c.email               = getText(s, 3);
        c.telefon             = getText(s, 4);
        c.data_nasterii       = getText(s, 5);
        c.gen                 = getText(s, 6);
        c.parola_criptata     = getText(s, 7);
        c.data_inscriere      = getText(s, 8);
        c.status              = getText(s, 9);
        c.cale_poza           = getText(s, 10);
        c.username            = getText(s, 11);
        c.rank                = getText(s, 12);
        c.varsta              = sqlite3_column_int(s, 13);
        c.total_imprumuturi   = sqlite3_column_int(s, 14);
        c.data_inscriere_client = getText(s, 15);
        rezultat.push_back(c);
    });
    return rezultat;
}

bool Database::adaugaClient(const ClientDB& c) {
    std::string sql1 = 
        "INSERT INTO utilizatori (nume, prenume, email, telefon, data_nasterii, "
        "gen, parola_criptata, data_inscriere, status, cale_poza, tip) "
        "VALUES ('" + c.nume + "','" + c.prenume + "','" + c.email + "','"
        + c.telefon + "','" + c.data_nasterii + "','" + c.gen + "','"
        + c.parola_criptata + "','" + c.data_inscriere + "','Activ','"
        + c.cale_poza + "','Client')";
    if (!exec(sql1)) return false;

    int new_id = (int)sqlite3_last_insert_rowid(db);
    std::string sql2 = 
        "INSERT INTO clienti (id_utilizator, username, varsta, rank, total_imprumuturi, data_inscriere_client) "
        "VALUES (" + std::to_string(new_id) + ",'" + c.username + "',"
        + std::to_string(c.varsta) + ",'" + c.rank + "',0,'"
        + c.data_inscriere_client + "')";
    return exec(sql2);
}

bool Database::actualizeazaRankClient(int id, const std::string& rank) {
    return exec("UPDATE clienti SET rank = '" + rank + "' WHERE id_utilizator = " 
                + std::to_string(id));
}

bool Database::actualizeazaTotalImprumuturi(int id, int total) {
    return exec("UPDATE clienti SET total_imprumuturi = " + std::to_string(total)
                + " WHERE id_utilizator = " + std::to_string(id));
}

bool Database::stergeClient(int id) {
    return exec("DELETE FROM utilizatori WHERE id = " + std::to_string(id));
}

// ── Voluntari ─────────────────────────────────────────────────

std::vector<VoluntarDB> Database::getToatiVoluntarii() {
    std::vector<VoluntarDB> rezultat;
    std::string sql =
        "SELECT u.id, u.nume, u.prenume, u.email, u.telefon, u.data_nasterii, "
        "u.gen, u.parola_criptata, u.data_inscriere, u.status, u.cale_poza, "
        "c.username, c.rank, c.varsta, c.total_imprumuturi, c.data_inscriere_client, "
        "v.cale_contract, v.data_start_contract, v.data_end_contract, "
        "v.ore_lucrate, v.rating_mediu, v.observatii_activitate, "
        "v.link_idei_eveniment, v.link_inscriere_org, v.link_program_colegi, "
        "v.link_program_personal, v.id_cladire_activa "
        "FROM utilizatori u "
        "JOIN clienti c ON u.id = c.id_utilizator "
        "JOIN voluntari v ON c.id_utilizator = v.id_utilizator";
    query(sql, [&](sqlite3_stmt* s) {
        VoluntarDB v;
        v.id                    = sqlite3_column_int(s, 0);
        v.nume                  = getText(s, 1);
        v.prenume               = getText(s, 2);
        v.email                 = getText(s, 3);
        v.telefon               = getText(s, 4);
        v.data_nasterii         = getText(s, 5);
        v.gen                   = getText(s, 6);
        v.parola_criptata       = getText(s, 7);
        v.data_inscriere        = getText(s, 8);
        v.status                = getText(s, 9);
        v.cale_poza             = getText(s, 10);
        v.username              = getText(s, 11);
        v.rank                  = getText(s, 12);
        v.varsta                = sqlite3_column_int(s, 13);
        v.total_imprumuturi     = sqlite3_column_int(s, 14);
        v.data_inscriere_client = getText(s, 15);
        v.cale_contract         = getText(s, 16);
        v.data_start            = getText(s, 17);
        v.data_end              = getText(s, 18);
        v.ore_lucrate           = sqlite3_column_int(s, 19);
        v.rating_mediu          = sqlite3_column_double(s, 20);
        v.observatii            = getText(s, 21);
        v.link_idei             = getText(s, 22);
        v.link_inscriere        = getText(s, 23);
        v.link_program_colegi   = getText(s, 24);
        v.link_program_personal = getText(s, 25);
        v.id_cladire_activa     = sqlite3_column_int(s, 26);
        rezultat.push_back(v);
    });
    return rezultat;
}

bool Database::actualizeazaOreVoluntar(int id, int ore) {
    return exec("UPDATE voluntari SET ore_lucrate = " + std::to_string(ore)
                + " WHERE id_utilizator = " + std::to_string(id));
}

bool Database::stergeVoluntar(int id) {
    return exec("DELETE FROM utilizatori WHERE id = " + std::to_string(id));
}

// ── Angajati ──────────────────────────────────────────────────

std::vector<AngajatDB> Database::getToatiAngajatii() {
    std::vector<AngajatDB> rezultat;
    std::string sql =
        "SELECT u.id, u.nume, u.prenume, u.email, u.telefon, u.data_nasterii, "
        "u.gen, u.parola_criptata, u.data_inscriere, u.status, u.cale_poza, "
        "c.username, c.rank, c.varsta, c.total_imprumuturi, c.data_inscriere_client, "
        "v.cale_contract, v.data_start_contract, v.data_end_contract, "
        "v.ore_lucrate, v.rating_mediu, v.observatii_activitate, "
        "v.link_idei_eveniment, v.link_inscriere_org, v.link_program_colegi, "
        "v.link_program_personal, v.id_cladire_activa, "
        "a.salariu_baza, a.zile_concediu_ramase, a.id_superior, "
        "a.parola_resursa, a.tip_angajat, a.id_cladire_post, a.id_sala_post "
        "FROM utilizatori u "
        "JOIN clienti c ON u.id = c.id_utilizator "
        "JOIN voluntari v ON c.id_utilizator = v.id_utilizator "
        "JOIN angajati a ON v.id_utilizator = a.id_utilizator";
    query(sql, [&](sqlite3_stmt* s) {
        AngajatDB a;
        a.id                    = sqlite3_column_int(s, 0);
        a.nume                  = getText(s, 1);
        a.prenume               = getText(s, 2);
        a.email                 = getText(s, 3);
        a.telefon               = getText(s, 4);
        a.data_nasterii         = getText(s, 5);
        a.gen                   = getText(s, 6);
        a.parola_criptata       = getText(s, 7);
        a.data_inscriere        = getText(s, 8);
        a.status                = getText(s, 9);
        a.cale_poza             = getText(s, 10);
        a.username              = getText(s, 11);
        a.rank                  = getText(s, 12);
        a.varsta                = sqlite3_column_int(s, 13);
        a.total_imprumuturi     = sqlite3_column_int(s, 14);
        a.data_inscriere_client = getText(s, 15);
        a.cale_contract         = getText(s, 16);
        a.data_start            = getText(s, 17);
        a.data_end              = getText(s, 18);
        a.ore_lucrate           = sqlite3_column_int(s, 19);
        a.rating_mediu          = sqlite3_column_double(s, 20);
        a.observatii            = getText(s, 21);
        a.link_idei             = getText(s, 22);
        a.link_inscriere        = getText(s, 23);
        a.link_program_colegi   = getText(s, 24);
        a.link_program_personal = getText(s, 25);
        a.id_cladire_activa     = sqlite3_column_int(s, 26);
        a.salariu_baza          = sqlite3_column_double(s, 27);
        a.zile_concediu         = sqlite3_column_int(s, 28);
        a.id_superior           = sqlite3_column_int(s, 29);
        a.parola_resursa        = getText(s, 30);
        a.tip_angajat           = getText(s, 31);
        a.id_cladire_post       = sqlite3_column_int(s, 32);
        a.id_sala_post          = sqlite3_column_int(s, 33);
        rezultat.push_back(a);
    });
    return rezultat;
}

bool Database::actualizeazaConcediuAngajat(int id, int zile) {
    return exec("UPDATE angajati SET zile_concediu_ramase = " + std::to_string(zile)
                + " WHERE id_utilizator = " + std::to_string(id));
}

bool Database::stergeAngajat(int id) {
    return exec("DELETE FROM utilizatori WHERE id = " + std::to_string(id));
}

// ── Imprumuturi ───────────────────────────────────────────────

bool Database::adaugaImprumut(int id_client, int id_carte,
                               const std::string& data_azi, const std::string& data_limita) {
    // Scade stocul
    exec("UPDATE carti SET exemplare_disponibile = exemplare_disponibile - 1 WHERE id = "
         + std::to_string(id_carte));
    return exec(
        "INSERT INTO imprumuturi (id_client, id_carte, data_imprumut, data_limita, returnata) "
        "VALUES (" + std::to_string(id_client) + "," + std::to_string(id_carte)
        + ",'" + data_azi + "','" + data_limita + "',0)");
}

bool Database::returneazaCarte(int id_carte, int id_client, const std::string& data_retur) {
    // Marcheaza returnata
    exec("UPDATE imprumuturi SET returnata = 1, data_retur_real = '" + data_retur
         + "' WHERE id_carte = " + std::to_string(id_carte)
         + " AND id_client = " + std::to_string(id_client)
         + " AND returnata = 0");
    // Creste stocul
    return exec("UPDATE carti SET exemplare_disponibile = exemplare_disponibile + 1 WHERE id = "
                + std::to_string(id_carte));
}

std::vector<ImprumutDB> Database::getImprumuturiClient(int id_client) {
    std::vector<ImprumutDB> rezultat;
    query("SELECT id, id_client, id_carte, data_imprumut, data_limita, "
          "data_retur_real, returnata FROM imprumuturi WHERE id_client = "
          + std::to_string(id_client),
    [&](sqlite3_stmt* s) {
        ImprumutDB i;
        i.id             = sqlite3_column_int(s, 0);
        i.id_client      = sqlite3_column_int(s, 1);
        i.id_carte       = sqlite3_column_int(s, 2);
        i.data_imprumut  = getText(s, 3);
        i.data_limita    = getText(s, 4);
        i.data_retur_real= getText(s, 5);
        i.returnata      = sqlite3_column_int(s, 6) != 0;
        rezultat.push_back(i);
    });
    return rezultat;
}

std::vector<ImprumutDB> Database::getImprumuturiActive() {
    std::vector<ImprumutDB> rezultat;
    query("SELECT id, id_client, id_carte, data_imprumut, data_limita, "
          "data_retur_real, returnata FROM imprumuturi WHERE returnata = 0",
    [&](sqlite3_stmt* s) {
        ImprumutDB i;
        i.id             = sqlite3_column_int(s, 0);
        i.id_client      = sqlite3_column_int(s, 1);
        i.id_carte       = sqlite3_column_int(s, 2);
        i.data_imprumut  = getText(s, 3);
        i.data_limita    = getText(s, 4);
        i.data_retur_real= getText(s, 5);
        i.returnata      = false;
        rezultat.push_back(i);
    });
    return rezultat;
}

// ── Rezervari ─────────────────────────────────────────────────

bool Database::adaugaRezervare(int id_client, int id_carte,
                                const std::string& data_azi, const std::string& data_exp) {
    exec("UPDATE carti SET exemplare_disponibile = exemplare_disponibile - 1 WHERE id = "
         + std::to_string(id_carte));
    return exec(
        "INSERT INTO rezervari (id_client, id_carte, data_rezervare, data_expirare, ridicata, anulata) "
        "VALUES (" + std::to_string(id_client) + "," + std::to_string(id_carte)
        + ",'" + data_azi + "','" + data_exp + "',0,0)");
}

bool Database::anuleazaRezervare(int id_client, int id_carte) {
    exec("UPDATE carti SET exemplare_disponibile = exemplare_disponibile + 1 WHERE id = "
         + std::to_string(id_carte));
    return exec("UPDATE rezervari SET anulata = 1 WHERE id_client = "
                + std::to_string(id_client) + " AND id_carte = "
                + std::to_string(id_carte) + " AND ridicata = 0 AND anulata = 0");
}

bool Database::confirmaRidicare(int id_client, int id_carte) {
    return exec("UPDATE rezervari SET ridicata = 1 WHERE id_client = "
                + std::to_string(id_client) + " AND id_carte = "
                + std::to_string(id_carte) + " AND ridicata = 0 AND anulata = 0");
}

bool Database::areRezervareActiva(int id_client, int id_carte) {
    bool exista = false;
    query("SELECT id FROM rezervari WHERE id_client = " + std::to_string(id_client)
          + " AND id_carte = " + std::to_string(id_carte)
          + " AND ridicata = 0 AND anulata = 0",
    [&](sqlite3_stmt*) { exista = true; });
    return exista;
}

// ── Wishlist ──────────────────────────────────────────────────

bool Database::adaugaInWishlist(int id_client, int id_carte) {
    return exec("INSERT OR IGNORE INTO wishlist (id_client, id_carte) VALUES ("
                + std::to_string(id_client) + "," + std::to_string(id_carte) + ")");
}

bool Database::stergedinWishlist(int id_client, int id_carte) {
    return exec("DELETE FROM wishlist WHERE id_client = " + std::to_string(id_client)
                + " AND id_carte = " + std::to_string(id_carte));
}

std::vector<int> Database::getWishlistClient(int id_client) {
    std::vector<int> rezultat;
    query("SELECT id_carte FROM wishlist WHERE id_client = " + std::to_string(id_client),
    [&](sqlite3_stmt* s) { rezultat.push_back(sqlite3_column_int(s, 0)); });
    return rezultat;
}

// ── Progres lectura ───────────────────────────────────────────

bool Database::seteazaStatusLectura(int id_client, int id_carte, const std::string& status) {
    return exec(
        "INSERT INTO progres_lectura (id_client, id_carte, status) "
        "VALUES (" + std::to_string(id_client) + "," + std::to_string(id_carte)
        + ",'" + status + "') "
        "ON CONFLICT(id_client, id_carte) DO UPDATE SET status = '" + status + "'");
}

bool Database::actualizeazaProgres(int id_client, int id_carte, int pagini) {
    return exec("UPDATE progres_lectura SET pagini_citite = " + std::to_string(pagini)
                + " WHERE id_client = " + std::to_string(id_client)
                + " AND id_carte = " + std::to_string(id_carte));
}

bool Database::finalizeazaLectura(int id_client, int id_carte,
                                   int stele, const std::string& obs) {
    return exec(
        "UPDATE progres_lectura SET status = 'CITITA', stelute = " + std::to_string(stele)
        + ", observatii = '" + obs + "', data_finalizare = date('now') "
        "WHERE id_client = " + std::to_string(id_client)
        + " AND id_carte = " + std::to_string(id_carte));
}

// ── Cladiri ───────────────────────────────────────────────────

int Database::getNrCladiri() {
    int nr = 0;
    query("SELECT COUNT(*) FROM cladiri", [&](sqlite3_stmt* s) {
        nr = sqlite3_column_int(s, 0);
    });
    return nr;
}

std::string Database::getNumeCladire(int id) {
    std::string nume;
    query("SELECT nume FROM cladiri WHERE id = " + std::to_string(id),
    [&](sqlite3_stmt* s) { nume = getText(s, 0); });
    return nume;
}
// Returneaza genurile din cartile finalizate de client, ordonate dupa frecventa
std::vector<std::string> Database::getGenuriFavoriteClient(int id_client) {
    std::vector<std::string> rezultat;
    query(
        "SELECT c.gen_specific, COUNT(*) as freq "
        "FROM progres_lectura pl "
        "JOIN carti c ON pl.id_carte = c.id "
        "WHERE pl.id_client = " + std::to_string(id_client) + " "
        "AND pl.status = 'CITITA' AND c.gen_specific IS NOT NULL "
        "GROUP BY c.gen_specific ORDER BY freq DESC LIMIT 3",
    [&](sqlite3_stmt* s) {
        rezultat.push_back(getText(s, 0));
    });
    return rezultat;
}

// Returneaza autorii din cartile finalizate de client, ordonati dupa frecventa
std::vector<std::string> Database::getAutoriFavoritiClient(int id_client) {
    std::vector<std::string> rezultat;
    query(
        "SELECT c.autor, COUNT(*) as freq "
        "FROM progres_lectura pl "
        "JOIN carti c ON pl.id_carte = c.id "
        "WHERE pl.id_client = " + std::to_string(id_client) + " "
        "AND pl.status = 'CITITA' "
        "GROUP BY c.autor ORDER BY freq DESC LIMIT 3",
    [&](sqlite3_stmt* s) {
        rezultat.push_back(getText(s, 0));
    });
    return rezultat;
}

// Returneaza ID-urile prietenilor unui client
std::vector<int> Database::getIdPrieteniClient(int id_client) {
    std::vector<int> rezultat;
    query(
        "SELECT id_client2 FROM prietenii WHERE id_client1 = " + std::to_string(id_client) + " "
        "UNION "
        "SELECT id_client1 FROM prietenii WHERE id_client2 = " + std::to_string(id_client),
    [&](sqlite3_stmt* s) {
        rezultat.push_back(sqlite3_column_int(s, 0));
    });
    return rezultat;
}

// Returneaza cartile citite de un client
std::vector<CarteDB> Database::getCartiCititeDeClient(int id_client) {
    std::vector<CarteDB> rezultat;
    std::string sql = SELECT_CARTI + 
        " JOIN progres_lectura pl ON carti.id = pl.id_carte"
        " WHERE pl.id_client = " + std::to_string(id_client) +
        " AND pl.status = 'CITITA'";
    query(sql, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    return rezultat;
}

// Recomandari bazate pe gen — exclude cartile deja citite de client
std::vector<CarteDB> Database::getRecomandariDupaGen(const std::string& gen, 
                                                       int id_client, int limita) {
    std::vector<CarteDB> rezultat;
    std::string sql = SELECT_CARTI +
        " WHERE LOWER(gen_specific) = LOWER('" + gen + "')"
        " AND exemplare_disponibile > 0"
        " AND id NOT IN ("
        "   SELECT id_carte FROM progres_lectura "
        "   WHERE id_client = " + std::to_string(id_client) + 
        " )"
        " ORDER BY rating DESC LIMIT " + std::to_string(limita);
    query(sql, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    return rezultat;
}

// Recomandari bazate pe autor — exclude cartile deja citite
std::vector<CarteDB> Database::getRecomandariDupaAutor(const std::string& autor,
                                                         int id_client, int limita) {
    std::vector<CarteDB> rezultat;
    std::string sql = SELECT_CARTI +
        " WHERE LOWER(autor) = LOWER('" + autor + "')"
        " AND exemplare_disponibile > 0"
        " AND id NOT IN ("
        "   SELECT id_carte FROM progres_lectura "
        "   WHERE id_client = " + std::to_string(id_client) +
        " )"
        " ORDER BY rating DESC LIMIT " + std::to_string(limita);
    query(sql, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    return rezultat;
}

// Recomandari bazate pe prieteni — ce au citit prietenii si clientul nu a citit inca
std::vector<CarteDB> Database::getRecomandariDupaPrieteni(int id_client, int limita) {
    std::vector<CarteDB> rezultat;
    auto prieteni = getIdPrieteniClient(id_client);
    if (prieteni.empty()) return rezultat;

    // Construieste lista de ID-uri prieteni pentru query
    std::string lista_prieteni = "";
    for (size_t i = 0; i < prieteni.size(); i++) {
        lista_prieteni += std::to_string(prieteni[i]);
        if (i < prieteni.size() - 1) lista_prieteni += ",";
    }

    std::string sql = SELECT_CARTI +
        " WHERE id IN ("
        "   SELECT id_carte FROM progres_lectura "
        "   WHERE id_client IN (" + lista_prieteni + ")"
        "   AND status = 'CITITA'"
        "   AND stelute >= 4"  // doar cartile cu rating bun de la prieteni
        " )"
        " AND id NOT IN ("
        "   SELECT id_carte FROM progres_lectura "
        "   WHERE id_client = " + std::to_string(id_client) +
        " )"
        " AND exemplare_disponibile > 0"
        " ORDER BY rating DESC LIMIT " + std::to_string(limita);
    query(sql, [&](sqlite3_stmt* s) {
        rezultat.push_back(rowToCarte(s));
    });
    return rezultat;
}

ClientDB Database::getClientDupaId(int id) {
    ClientDB c; c.id = -1;
    std::string sql = 
        "SELECT u.id, u.nume, u.prenume, u.email, u.telefon, u.data_nasterii, "
        "u.gen, u.parola_criptata, u.data_inscriere, u.status, u.cale_poza, "
        "c.username, c.rank, c.varsta, c.total_imprumuturi, c.data_inscriere_client "
        "FROM utilizatori u JOIN clienti c ON u.id = c.id_utilizator "
        "WHERE u.id = " + std::to_string(id);
    query(sql, [&](sqlite3_stmt* s) {
        c.id                    = sqlite3_column_int(s, 0);
        c.nume                  = getText(s, 1);
        c.prenume               = getText(s, 2);
        c.email                 = getText(s, 3);
        c.telefon               = getText(s, 4);
        c.data_nasterii         = getText(s, 5);
        c.gen                   = getText(s, 6);
        c.parola_criptata       = getText(s, 7);
        c.data_inscriere        = getText(s, 8);
        c.status                = getText(s, 9);
        c.cale_poza             = getText(s, 10);
        c.username              = getText(s, 11);
        c.rank                  = getText(s, 12);
        c.varsta                = sqlite3_column_int(s, 13);
        c.total_imprumuturi     = sqlite3_column_int(s, 14);
        c.data_inscriere_client = getText(s, 15);
    });
    return c;
}
bool Database::adaugaMesaj(int id_grup, int id_client, const std::string& username, const std::string& text) {
    std::string sql = "INSERT INTO mesaje_grup (id_grup, id_client, username, text) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_grup);
        sqlite3_bind_int(stmt, 2, id_client);
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, text.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return true;
    }
    return false;
}

std::vector<MesajDB> Database::getMesajeGrup(int id_grup) {
    std::vector<MesajDB> rezultat;
    std::string sql = "SELECT id, id_grup, id_client, username, text, data_ora FROM mesaje_grup WHERE id_grup = ? ORDER BY id ASC LIMIT 100";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_grup);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            MesajDB m;
            m.id = sqlite3_column_int(stmt, 0);
            m.id_grup = sqlite3_column_int(stmt, 1);
            m.id_client = sqlite3_column_int(stmt, 2);
            m.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            m.text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            m.data_ora = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            rezultat.push_back(m);
        }
        sqlite3_finalize(stmt);
    }
    return rezultat;
}

bool Database::adaugaIdeieEveniment(int id_voluntar, const std::string& titlu, const std::string& tip, const std::string& data_propusa, const std::string& corp, const std::string& descriere, const std::string& resurse) {
    std::string sql = "INSERT INTO idei_evenimente (id_voluntar, titlu, tip, data_propusa, corp, descriere, resurse) VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_voluntar);
        sqlite3_bind_text(stmt, 2, titlu.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, tip.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, data_propusa.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, corp.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, descriere.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, resurse.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return true;
    }
    return false;
}

bool Database::adaugaInscriere(int id_voluntar, const std::string& eveniment, const std::string& rol, const std::string& disponibilitate, const std::string& competente, const std::string& mentiuni) {
    std::string sql = "INSERT INTO inscrieri_organizare (id_voluntar, eveniment, rol, disponibilitate, competente, mentiuni) VALUES (?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_voluntar);
        sqlite3_bind_text(stmt, 2, eveniment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, rol.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, disponibilitate.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, competente.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, mentiuni.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return true;
    }
    return false;
}
