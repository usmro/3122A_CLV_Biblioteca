#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <map>

// Structuri simple pentru transfer date intre DB si clase
struct CarteDB {
    int id;
    std::string titlu, autor, editura, limba, data_pub;
    int an_aparitie, nr_pagini;
    std::string tara_provenienta, tip, suport, status, sursa, destinatar;
    std::string serie_contabila;
    int exemplare_disponibile;
    bool este_patrimoniu;
    float pret_intrare, rating;
    std::string rezumat, cale_poza;
    int valoare_masurabila;
    float marime_mb;
    int id_cladire, id_sala, id_raft;
    std::string cod_identificare;
    // Fictiune
    std::string isbn, gen_specific, personaj_principal, serie, tip_naratiune;
    int varsta_recomandata;
    std::string dimensiuni, tip_coperta;
    float greutate;
    // NonFictiune
    std::string subcategorie, domeniu, institutie_sursa, editie_revizuita;
    // Periodice
    std::string issn, frecventa;
    int numar_editie;
};

struct AutorDB {
    int id;
    std::string nume_complet, biografie, cale_poza;
};

struct ClientDB {
    int id;
    std::string nume, prenume, email, telefon;
    std::string data_nasterii, gen, parola_criptata;
    std::string data_inscriere, status, cale_poza;
    std::string username, rank, data_inscriere_client;
    int varsta, total_imprumuturi;
};

struct VoluntarDB : public ClientDB {
    std::string cale_contract, data_start, data_end;
    int ore_lucrate;
    double rating_mediu;
    std::string observatii;
    std::string link_idei, link_inscriere, link_program_colegi, link_program_personal;
    int id_cladire_activa;
};

struct AngajatDB : public VoluntarDB {
    double salariu_baza;
    int zile_concediu, id_superior;
    std::string parola_resursa, tip_angajat;
    int id_cladire_post, id_sala_post;
};

struct ImprumutDB {
    int id, id_client, id_carte;
    std::string data_imprumut, data_limita, data_retur_real;
    bool returnata;
};

struct RezervareDB {
    int id, id_client, id_carte;
    std::string data_rezervare, data_expirare;
    bool ridicata, anulata;
};

struct MesajDB {
    int id, id_grup, id_client;
    std::string username, text, data_ora;
};

class Database {
private:
    sqlite3* db;
    std::string db_path;

    // Helper: executa un query fara rezultate
    bool exec(const std::string& sql);
    
    // Helper: executa query cu callback pentru rezultate
    bool query(const std::string& sql, 
               std::function<void(sqlite3_stmt*)> callback);

public:
    sqlite3* getDb() { return db; }
    bool adaugaMesaj(int id_grup, int id_client, const std::string& username, const std::string& text);
    std::vector<MesajDB> getMesajeGrup(int id_grup);
    bool adaugaIdeieEveniment(int id_voluntar, const std::string& titlu, const std::string& tip, const std::string& data_propusa, const std::string& corp, const std::string& descriere, const std::string& resurse);
    bool adaugaInscriere(int id_voluntar, const std::string& eveniment, const std::string& rol, const std::string& disponibilitate, const std::string& competente, const std::string& mentiuni);
    std::vector<std::map<std::string,std::string>> getIdeiEvenimente();
    std::vector<std::map<std::string,std::string>> getInscrieri();
    Database(const std::string& path = "biblioteca.db");
    ~Database();

    bool esteDeschisDB() const { return db != nullptr; }
    bool initializeaza(const std::string& schema_path);

    // ── Carti ────────────────────────────────────────────────
    std::vector<CarteDB> getToateCartile();
    std::vector<CarteDB> cautaDupaTitlu(const std::string& titlu);
    std::vector<CarteDB> cautaDupaAutor(const std::string& autor);
    std::vector<CarteDB> cautaDupaCladire(int id_cladire);
    std::vector<CarteDB> filtreazaDupaRating(float rating_min);
    CarteDB getCarteDupaId(int id);
    bool adaugaCarte(const CarteDB& c);
    bool actualizeazaStocCarte(int id_carte, int stoc_nou);
    bool actualizeazaStatusCarte(int id_carte, const std::string& status);
    bool stergeCarte(int id);

    // ── Autori ───────────────────────────────────────────────
    std::vector<AutorDB> getToatiAutorii();
    AutorDB getAutorDupaId(int id);
    AutorDB getAutorDupaNumele(const std::string& nume);
    bool adaugaAutor(const AutorDB& a);

    // ── Clienti ──────────────────────────────────────────────
    std::vector<ClientDB> getToatiClientii();
    ClientDB getClientDupaId(int id);
    bool adaugaClient(const ClientDB& c);
    bool actualizeazaRankClient(int id, const std::string& rank);
    bool actualizeazaTotalImprumuturi(int id, int total);
    bool stergeClient(int id);

    // ── Voluntari ────────────────────────────────────────────
    std::vector<VoluntarDB> getToatiVoluntarii();
    VoluntarDB getVoluntarDupaId(int id);
    bool adaugaVoluntar(const VoluntarDB& v);
    bool actualizeazaOreVoluntar(int id, int ore);
    bool stergeVoluntar(int id);

    // ── Angajati ─────────────────────────────────────────────
    std::vector<AngajatDB> getToatiAngajatii();
    AngajatDB getAngajatDupaId(int id);
    bool adaugaAngajat(const AngajatDB& a);
    bool actualizeazaConcediuAngajat(int id, int zile);
    bool stergeAngajat(int id);

    // ── Imprumuturi ──────────────────────────────────────────
    bool adaugaImprumut(int id_client, int id_carte, 
                        const std::string& data_azi, const std::string& data_limita);
    bool returneazaCarte(int id_carte, int id_client, const std::string& data_retur);
    std::vector<ImprumutDB> getImprumuturiClient(int id_client);
    std::vector<ImprumutDB> getImprumuturiActive();

    // ── Rezervari ────────────────────────────────────────────
    bool adaugaRezervare(int id_client, int id_carte,
                         const std::string& data_azi, const std::string& data_expirare);
    bool anuleazaRezervare(int id_client, int id_carte);
    bool confirmaRidicare(int id_client, int id_carte);
    bool areRezervareActiva(int id_client, int id_carte);

    // ── Wishlist ─────────────────────────────────────────────
    bool adaugaInWishlist(int id_client, int id_carte);
    bool stergedinWishlist(int id_client, int id_carte);
    std::vector<int> getWishlistClient(int id_client);

    // ── Progres lectura ──────────────────────────────────────
    bool seteazaStatusLectura(int id_client, int id_carte, const std::string& status);
    bool actualizeazaProgres(int id_client, int id_carte, int pagini);
    bool finalizeazaLectura(int id_client, int id_carte, int stele, const std::string& obs);

    // ── Cladiri ──────────────────────────────────────────────
    int getNrCladiri();
    std::string getNumeCladire(int id);

    std::vector<std::string> getGenuriFavoriteClient(int id_client);
    std::vector<std::string> getAutoriFavoritiClient(int id_client);
    std::vector<int> getIdPrieteniClient(int id_client);
    std::vector<CarteDB> getCartiCititeDeClient(int id_client);
    std::vector<CarteDB> getRecomandariDupaGen(const std::string& gen, 
                                                int id_client, int limita = 5);
    std::vector<CarteDB> getRecomandariDupaAutor(const std::string& autor, 
                                                  int id_client, int limita = 3);
    std::vector<CarteDB> getRecomandariDupaPrieteni(int id_client, int limita = 5);
};

#endif
