#include "Client.h"
#include <iostream>
#include <algorithm>

Client::Client(int id, std::string n, std::string p, std::string e, std::string tel,
               std::string d_n, std::string g, std::string pass, 
               std::string user, int v, std::string d_i, std::string rank_p, std::string poza)
    : Utilizator(id, n, p, e, tel, d_n, g, pass, d_i, poza), 
      username(user), varsta(v), data_inscriere_client(d_i), rank(rank_p),  total_imprumuturi_istoric(0)

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
// ── Rezervare ─────────────────────────────────────────────────────────────────

void Client::rezervaCarte(int id_carte) {
    if (areCarteInListe(id_carte)) {
        std::cout << "[Rezervare] Aceasta carte este deja intr-una din listele tale.\n";
        return;
    }
    carti_rezervate.push_back(id_carte);
    std::cout << "[Rezervare] Cartea cu ID " << id_carte
              << " a fost rezervata online. O poti ridica in 48 de ore.\n";
}

void Client::anuleazaRezervare(int id_carte) {
    auto it = std::find(carti_rezervate.begin(), carti_rezervate.end(), id_carte);
    if (it != carti_rezervate.end()) {
        carti_rezervate.erase(it);
        std::cout << "[Rezervare] Rezervarea pentru cartea cu ID " << id_carte << " a fost anulata.\n";
    } else {
        std::cout << "[Rezervare] Nu ai nicio rezervare activa pentru cartea cu ID " << id_carte << ".\n";
    }
}

// Cartea rezervata a fost ridicata fizic -> trece in imprumut activ
void Client::confirmRidicare(int id_carte, std::string titlu, int nr_pagini) {
    auto it = std::find(carti_rezervate.begin(), carti_rezervate.end(), id_carte);
    if (it != carti_rezervate.end()) {
        carti_rezervate.erase(it);
        incepeImprumut(id_carte, titlu, nr_pagini);
        std::cout << "[Rezervare] Cartea a fost ridicata si trecuta in 'In Progres'.\n";
    } else {
        std::cout << "[Rezervare] Nu exista o rezervare activa pentru cartea cu ID " << id_carte << ".\n";
    }
}

bool Client::areRezervare(int id_carte) const {
    return std::find(carti_rezervate.begin(), carti_rezervate.end(), id_carte)
           != carti_rezervate.end();
}

// ── Status carte ──────────────────────────────────────────────────────────────

bool Client::areCarteInListe(int id_carte) const {
    if (std::find(wishlist.begin(), wishlist.end(), id_carte) != wishlist.end())
        return true;
    if (areRezervare(id_carte))
        return true;
    for (const auto& l : lecturi_active)
        if (l.id_carte == id_carte) return true;
    for (const auto& l : lecturi_finalizate)
        if (l.id_carte == id_carte) return true;
    return false;
}

StatusLectura Client::getStatusCarte(int id_carte) const {
    for (const auto& l : lecturi_active)
        if (l.id_carte == id_carte) return StatusLectura::IN_PROGRES;
    for (const auto& l : lecturi_finalizate)
        if (l.id_carte == id_carte) return StatusLectura::CITITA;
    if (areRezervare(id_carte))
        return StatusLectura::REZERVATA;
    return StatusLectura::DORITA;
}

// ── Evenimente ───────────────────────────────────────────────────────────────

void Client::inscrieLaEveniment(std::string titlu, std::string data, std::string ora) {
    for (const auto& ev : evenimente_client) {
        if (ev.titlu == titlu && ev.data == data) {
            std::cout << "[Evenimente] Esti deja inscris la '" << titlu << "'.\n";
            return;
        }
    }
    EvenimentClient ev;
    ev.titlu        = titlu;
    ev.data         = data;
    ev.ora          = ora;
    ev.a_participat = false;
    evenimente_client.push_back(ev);
    std::cout << "[Evenimente] Inscris la '" << titlu << "' pe " << data << " ora " << ora << ".\n";
}

void Client::marcheazaParticipare(std::string titlu) {
    for (auto& ev : evenimente_client) {
        if (ev.titlu == titlu) {
            ev.a_participat = true;
            std::cout << "[Evenimente] Participarea la '" << titlu << "' confirmata.\n";
            return;
        }
    }
    std::cout << "[Evenimente] Evenimentul '" << titlu << "' nu a fost gasit in lista ta.\n";
}

void Client::afiseazaEvenimente() const {
    std::cout << "\n--- EVENIMENTE: " << getUsername() << " ---\n";
    bool are_viitoare = false, are_trecute = false;

    for (const auto& ev : evenimente_client) {
        if (!ev.a_participat) {
            if (!are_viitoare) { std::cout << "[VIITOARE]\n"; are_viitoare = true; }
            std::cout << "  - " << ev.titlu << " | " << ev.data << " " << ev.ora << "\n";
        }
    }
    for (const auto& ev : evenimente_client) {
        if (ev.a_participat) {
            if (!are_trecute) { std::cout << "[PARTICIPAT]\n"; are_trecute = true; }
            std::cout << "  - " << ev.titlu << " | " << ev.data << "\n";
        }
    }
    if (!are_viitoare && !are_trecute)
        std::cout << "  Nu esti inscris la niciun eveniment.\n";
    std::cout << "------------------------------------\n";
}

// ── Modificare incepeImprumut (inlocuieste metoda existenta) ──────────────────
// Adauga total_imprumuturi_istoric++ si seteaza status_lectura.
// Daca ai deja incepeImprumut(), inlocuieste-o cu versiunea asta:

void Client::incepeImprumut(int id_carte, std::string titlu, int tot_pagini) {
    auto it_w = std::find(wishlist.begin(), wishlist.end(), id_carte);
    if (it_w != wishlist.end()) wishlist.erase(it_w);

    auto it_r = std::find(carti_rezervate.begin(), carti_rezervate.end(), id_carte);
    if (it_r != carti_rezervate.end()) carti_rezervate.erase(it_r);

    ProgresLectura nou;
    nou.id_carte       = id_carte;
    nou.titlu          = titlu;
    nou.pagini_totale  = tot_pagini;
    nou.pagini_citite  = 0;
    nou.stelute        = 0;
    nou.observatii     = "";
    nou.status_lectura = StatusLectura::IN_PROGRES;   // <-- NOU

    lecturi_active.push_back(nou);
    total_imprumuturi_istoric++;                       // <-- NOU

    std::cout << "[Lectura] Ai inceput sa citesti: " << titlu
              << " | Total imprumuturi: " << total_imprumuturi_istoric << "\n";
}
