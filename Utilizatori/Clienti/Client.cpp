#include "Client.h"
#include <iostream>
#include <algorithm>

Client::Client(int id, std::string n, std::string p, std::string e, std::string tel,
               std::string d_n, std::string g, std::string pass, 
               std::string user, int v, std::string d_i, std::string rank_p, std::string poza)
    : Utilizator(id, n, p, e, tel, d_n, g, pass, d_i, poza), 
      username(user), varsta(v), data_inscriere_client(d_i), rank(rank_p) 
{
   
    if(rank.empty()) rank = "Novice";
}

void Client::afiseazaTip() {
    std::cout << "Tip Utilizator: Client [" << rank << "]\n";
}

std::string Client::exportaInCSV() const {
    return std::to_string(id_utilizator) + ";" + nume + ";" + prenume + ";" + username + ";" + rank + ";Client";
}

void Client::actualizeazaRank() {
    size_t total_citite = lecturi_finalizate.size();
    if (total_citite > 20) rank = "Elite Reader";
    else if (total_citite > 10) rank = "Pasionat";
    else if (total_citite > 5) rank = "Cititor Constant";
    else rank = "Novice";
}

void Client::adaugaInWishlist(int id_carte) {
    wishlist.push_back(id_carte);
    std::cout << "[Wishlist] Cartea cu ID " << id_carte << " a fost adaugata.\n";
}

void Client::incepeImprumut(int id_carte, std::string titlu, int tot_pagini) {
    auto it = std::find(wishlist.begin(), wishlist.end(), id_carte);
    if (it != wishlist.end()) wishlist.erase(it);

    lecturi_active.push_back({id_carte, titlu, tot_pagini, 0, 0, ""});
    std::cout << "[Lectura] Ai inceput sa citesti: " << titlu << "\n";
}

void Client::updateProgres(int id_carte, int pagini_noi) {
    for (auto &l : lecturi_active) {
        if (l.id_carte == id_carte) {
            l.pagini_citite = pagini_noi;
            std::cout << "[Progres] " << l.titlu << ": " << l.getProcent() << "%\n";
            
            if (l.pagini_citite >= l.pagini_totale) {
                std::cout << "Felicitari! Ai terminat cartea. Te rugam sa lasi o nota.\n";
            }
            return;
        }
    }
}

void Client::finalizeazaCarte(int id_carte, int note, std::string review) {
    for (auto it = lecturi_active.begin(); it != lecturi_active.end(); ++it) {
        if (it->id_carte == id_carte) {
            it->stelute = note;
            it->observatii = review;
            it->pagini_citite = it->pagini_totale;

            lecturi_finalizate.push_back(*it);
            lecturi_active.erase(it);
            
            actualizeazaRank();
            std::cout << "[Goodreads] Carte mutata in 'Citite'. Rank actualizat: " << rank << "\n";
            return;
        }
    }
}

void Client::afiseazaProfilGoodreads() const {
    std::cout << "\n--- PROFIL " << username << " (" << rank << ") ---\n";
    std::cout << "Prietenii tai: " << lista_prieteni.size() << "\n";
    std::cout << "Carti in progres: " << lecturi_active.size() << "\n";
    std::cout << "Carti citite: " << lecturi_finalizate.size() << "\n";
    std::cout << "Wishlist: " << wishlist.size() << " carti.\n";
    std::cout << "------------------------------\n";
}

void Client::acceptaPrieten(int id_prieten) {
    lista_prieteni.insert(id_prieten);
    std::cout << "[Social] Acum esti prieten cu utilizatorul " << id_prieten << "!\n";
}

void Client::feedbackAngajat(int id_angajat, int nota, std::string text) {
    std::cout << "[Feedback] Nota " << nota << " trimisa angajatului " << id_angajat << "\n";
}