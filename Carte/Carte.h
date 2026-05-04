#ifndef CARTE_H
#define CARTE_H

#include <string>
#include <vector>
#include <iostream>
class Autor;
struct Recenzie {
    int id_utilizator;
    std::string username;
    int nota; // 1-5 stele
    std::string comentariu;
};
enum class TipSuport { FIZIC, DIGITAL_PDF, AUDIOBOOK, VIDEO };
struct Locatie {
    std::string nume_filiala;
    int etaj;
    int camera;
    int raft;
    std::string cod_identificare;
};
// 1. CLASA DE BAZA 
class Carte {
public:
    int id_carte;
    std::string titlu, autor, editura, limba, data_pub, status, sursa, destinatar;
    std::string tara_provenienta; 
    int an_aparitie;
    int nr_pagini;
    std::string serie_contabila;       
    std::vector<std::string> defecte; 
    int exemplare_disponibile; 
    bool este_patrimoniu; 

    float pret_intrare;    // Valoarea de inventar 
    float rating;          
    std::string rezumat;   
    std::string cale_poza; 
    std::string traducere;

    TipSuport suport;
    int valoare_masurabila; // Nr. pagini sau minute 
    float marime_mb;    
    Locatie adresa_raft; 
    
    std::vector<Recenzie> lista_recenzii;
    Autor* autor_ref = nullptr;

    // Constructor complet
    Carte(int id, std::string t, std::string a, std::string ed, std::string l, std::string d_p, 
          int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag, std::string sc, 
          int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb);

    virtual void afiseazaFisaCatalog() ; 
    void adaugaDefect(std::string descriere);
    void afiseazaStareFizica();
    
    void adaugaRecenzieSociala(int id_user, std::string user, int nota, std::string text);
    void actualizeazaRatingMediu();
    bool esteDisponibila() const { return status == "Disponibila" && exemplare_disponibile > 0; }
    
    virtual ~Carte() {}
};

// 2. CARTE FICTIUNE 
class CarteFictiune : public Carte {
public:
    std::string isbn, gen_specific; 
    std::string personaj_principal, serie, tip_naratiune;
    int varsta_recomandata;

    std::string dimensiuni; 
    float greutate;
    std::string tip_coperta;

    CarteFictiune(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, 
                  int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag, std::string sc, 
                  int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb, 
                  std::string gen, std::string p_p, std::string ser, std::string t_n, 
                  int varsta, std::string dim, float gr, std::string coperta);
    
    void afiseazaFisaCatalog() override;
};

// 3. CARTE NON-FICTIUNE 
class CarteNonFictiune : public Carte {
public:
    std::string isbn, subcategorie;
    std::string domeniu, institutie_sursa, editie_revizuita;
    
    std::string detalii_tehnice; 
    std::string dimensiuni;
    float greutate;

    CarteNonFictiune(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, 
                     int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag, std::string sc, 
                     int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb,
                     std::string subcat, std::string dom, std::string inst, 
                     std::string revz, std::string dim, float gr);
    
    void afiseazaFisaCatalog() override;
};

// 4. CARTE PERIODICA 
class CartePeriodica : public Carte {
public:
    std::string issn;
    int numar_editie;
    std::string frecventa;

    CartePeriodica(int id, std::string t, std::string a, std::string ed, std::string i_issn, std::string l, std::string d_p, 
                   int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag, std::string sc, 
                   int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb, 
                   int nr_ed, std::string frecv);
    
    void afiseazaFisaCatalog() override;
};

#endif