#ifndef CLIENT_H
#define CLIENT_H

#include "../Utilizator.h"
#include <vector>
#include <string>
#include <set>

 enum class StatusLectura {
    DORITA,      // in wishlist
    REZERVATA,   // rezervata online, neridicate inca
    IN_PROGRES,  // imprumutata activ
    CITITA       // finalizata
};


// Structura pentru folderul "In Progres" si "Citite"
struct ProgresLectura {
    int id_carte;
    std::string titlu;
    int pagini_totale;
    int pagini_citite;
    int stelute;             // 0 daca nu a terminat-o
    std::string observatii;  
    StatusLectura status_lectura;
    double getProcent() const {
        if (pagini_totale <= 0) return 0;
        return (double)pagini_citite / pagini_totale * 100;
    }
};
struct EvenimentClient {
    std::string titlu;
    std::string data;
    std::string ora;
    bool a_participat; // false = viitor/inscris, true = participat deja
};
class Client : public Utilizator {
private:
    std::string username;
    int varsta;
    std::string data_inscriere_client;
    std::string rank;

    // Social
    std::set<int> lista_prieteni; // ID-urile prietenilor
    std::vector<int> cereri_prietenie;

    // Folderele de carti (Sistemul Goodreads)
    std::vector<int> wishlist;               // ID-uri carti dorite
    std::vector<ProgresLectura> lecturi_active; // Folder: "In progres"
    std::vector<ProgresLectura> lecturi_finalizate; // Folder: "Citite"

    // Evenimente
    std::vector<std::string> evenimente_viitoare;
    std::vector<int> carti_rezervate;          // carti rezervate online
    std::vector<EvenimentClient> evenimente_client;
    int total_imprumuturi_istoric;  
public:
    Client(int id, std::string n, std::string p, std::string e, std::string tel,
           std::string d_n, std::string g, std::string pass,
           std::string user, int v, std::string d_i,std::string rank, std::string poza);
    // Rezervare

void rezervaCarte(int id_carte);
void anuleazaRezervare(int id_carte);
void confirmRidicare(int id_carte, std::string titlu, int nr_pagini);
    bool areRezervare(int id_carte) const;

    // Status carte din perspectiva clientului
    StatusLectura getStatusCarte(int id_carte) const;
    bool areCarteInListe(int id_carte) const;

// Statistici împrumut

// Evenimente
void inscrieLaEveniment(std::string titlu, std::string data, std::string ora);
    void marcheazaParticipare(std::string titlu);
    void afiseazaEvenimente() const;

std::vector<std::string> evenimente_participat; // deja ai evenimente_viitoare, adaugi si asta
    // Metode virtuale implementate
    void afiseazaTip() override;
    std::string exportaInCSV() const override;

    // Gestiune Profil & Social
    void actualizeazaRank();
    void trimiteCererePrietenie(int id_prieten);
    void acceptaPrieten(int id_prieten);
    void afiseazaProfilGoodreads() const;

    // Sistemul de Lectura
    void adaugaInWishlist(int id_carte);
    void incepeImprumut(int id_carte, std::string titlu, int tot_pagini);
    void updateProgres(int id_carte, int pagini_noi);
    void finalizeazaCarte(int id_carte, int note, std::string review);

    void feedbackAngajat(int id_angajat, int nota, std::string text);

    std::string getUsername() const { return username; }
    std::string getRank() const { return rank; }
    // Gettere statistici
    int getNrImprumuturiActive() const  { return (int)lecturi_active.size(); }
    int getNrImprumuturiTotale() const  { return total_imprumuturi_istoric; }
    int getNrCartiCitite()       const  { return (int)lecturi_finalizate.size(); }
    int getNrRezervari()         const  { return (int)carti_rezervate.size(); }
    int getVarsta()              const  { return varsta; }

};

#endif