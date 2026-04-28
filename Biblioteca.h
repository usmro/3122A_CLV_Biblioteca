#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "Carte.h"
#include "Utilizator.h"
#include "Cladire.h"
#include "Autor.h" 
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

    std::vector<Carte*> inventar_general; 
    std::vector<Autor*> baza_date_autori;
    std::vector<Client> lista_clienti;
    std::vector<Angajat> lista_angajati;
    std::vector<Voluntar> lista_voluntari;
    std::vector<Cladire> lista_filiale;
    std::vector<InregistrareImprumut> istoric_imprumuturi;
    std::vector<Carte*> cautaDupaAutor(const std::string& autor);
    std::vector<Carte*> cautaDupaTitlu(const std::string& titlu);
    std::vector<Carte*> filtreazaAvansat(const CriteriiFiltrare& criterii);
    
    Biblioteca();
    ~Biblioteca(); 


    void adaugaCarte(Carte* c, Locatie loc);
    void adaugaAutor(Autor* a);

    void afiseazaDisponibilitateExemplare(const std::string& titlu_cautat, const std::string& autor_cautat);
    void cautaInfoAutor(const std::string& nume_cautat);
    void realizareInventar(const std::string& nume_angajat);
    void filtreazaDupaRating(float ratingMinim);

    void sorteazaDupaAutor(std::vector<Carte*>& lista);
    void sorteazaDupaEditura(std::vector<Carte*>& lista);
    void sorteazaDupaAn(std::vector<Carte*>& lista);

    void realizeazaImprumut(int id_carte, int id_utilizator, const std::string& data_azi);
    void realizeazaRetur(int id_carte, const std::string& data_retur_reala);

    void afiseazaHartaGrupata();
    
    void afiseazaTotInventarul();
    void afiseazaRegulament();

    void salveazaLogistica();
    void incarcaLogistica();
    void initializeazaLogisticaA_F();

    void coreleazaAutoriCuCarti();

    void inregistreazaClient(Client c);
    void inregistreazaAngajat(Angajat a);
    void inregistreazaVoluntar(Voluntar v);

    void salveazaBazaDate();
    void incarcaBazaDate();
    void incarcaAutori();
    void populeazaAutoriDinCarti();

    void realizeazaImprumut(int id_carte, int id_utilizator, std::string data_azi);
    void realizeazaRetur(int id_carte, std::string data_retur_reala);
};

#endif