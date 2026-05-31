#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H
#include "../src/Database/Database.h"

#include "Utilizatori/Clienti/Client.h"
#include "Utilizatori/Voluntari/Voluntar.h"
#include "Utilizatori/Angajati/Angajat.h"
#include "Carte/Carte.h"
#include "Autor/Autor.h"
#include "Cladiri/Cladire.h"
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

struct CriteriiFiltrare {
    std::vector<TipSuport> formate;      
    std::vector<std::string> limbi;     
    std::vector<std::string> tari;      
    std::vector<int> ani;               
    std::vector<std::string> genuri;    
    std::vector<std::string> edituri;   
    float ratingMinim = 0.0f;
    int paginiMin = 0;
    int paginiMax = 1000000;
};


struct InregistrareImprumut {
    int id_utilizator;
    int id_carte;
    std::string data_imprumut; 
    std::string data_limita;   
    bool returnata;

    bool esteIntarziata(std::string data_curenta) {
        if (returnata) return false;
        return data_curenta > data_limita; 
    }
};


struct RaportInventar {
    std::string data_efectuare;
    std::string nume_responsabil;
    int carti_gasite;
    int carti_lipsa;
    std::vector<int> id_uri_lipsa;
    std::string observatii_defecte;
};

class Biblioteca {
public:
    Database db;
    std::vector<Carte*> inventar_general; 
    std::vector<Autor*> baza_date_autori;
    std::vector<Client*> lista_clienti;
    std::vector<Voluntar*> lista_voluntari; 
    std::vector<Angajat*> lista_angajati;
    std::vector<Utilizator*> lista_utilizatori;
    std::vector<Cladire> lista_filiale;
    std::vector<InregistrareImprumut> istoric_imprumuturi;
    Biblioteca();
    ~Biblioteca(); 

    // Gestiune Cărți și Autori
    void adaugaCarte(Carte* c, Locatie loc);
    void adaugaAutor(Autor* a);
    void coreleazaAutoriCuCarti();
    void populeazaAutoriDinCarti();

    // Căutare și Filtrare
    std::vector<Carte*> cautaDupaAutor(const std::string& autor);
    std::vector<Carte*> cautaDupaTitlu(const std::string& titlu);
    std::vector<Carte*> filtreazaAvansat(const CriteriiFiltrare& criterii);
    void filtreazaDupaRating(float ratingMinim);
    void afiseazaDisponibilitateExemplare(const std::string& titlu_cautat, const std::string& autor_cautat);
    void cautaInfoAutor(const std::string& nume_cautat);

    // Sortare
    void sorteazaDupaAutor(std::vector<Carte*>& lista);
    void sorteazaDupaEditura(std::vector<Carte*>& lista);
    void sorteazaDupaAn(std::vector<Carte*>& lista);

    // Împrumuturi
    void rezervaCarte(int id_carte, int id_utilizator);
void anuleazaRezervare(int id_carte, int id_utilizator);
    void realizeazaImprumut(int id_carte, int id_utilizator, const std::string& data_azi);
    void realizeazaRetur(int id_carte, const std::string& data_retur_reala);

    // Utilizatori - Metode de înregistrare 
    void inregistreazaClient(Client c);
    void inregistreazaAngajat(Angajat* a);
    void inregistreazaVoluntar(Voluntar v);
    Client* getClientDupaId(int id);
    // Încărcare/Ștergere Utilizatori
    void incarcaClienti(const std::string& fisier_clienti);
    void incarcaAngajati(const std::string& nume_fisier);
    void incarcaVoluntari(const std::string& nume_fisier);
    void stergeVoluntar(int id_cautat);
    void stergeAngajat(int id_cautat);
    void stergeClient(int id_cautat);

    // Rapoarte și Logistică
    void afiseazaHartaGrupata();
    void afiseazaTotInventarul();
    void afiseazaRegulament();
    void realizareInventar(const std::string& nume_angajat);
    
    void salveazaLogistica();
    void incarcaLogistica();
    void initializeazaLogisticaA_F();
    void salveazaBazaDate();
    void incarcaBazaDate();
    void incarcaAutori();

    struct RezultatRecomandari {
        std::vector<Carte*> dupa_gen;      // "Ai citit Fantasy, incearca si..."
        std::vector<Carte*> dupa_autor;    // "Mai multe carti de acelasi autor"
        std::vector<Carte*> dupa_prieteni; // "Prietenii tai au citit..."
        std::string gen_favorit;
        std::string autor_favorit;
    };
    RezultatRecomandari getRecomandariClient(int id_client);

};

#endif