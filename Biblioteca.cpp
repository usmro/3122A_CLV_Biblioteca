#include "Biblioteca.h"
#include <iostream>
Biblioteca::Biblioteca() {}

void Biblioteca::adaugaCarte(Carte c) {
    inventar_carti.push_back(c);
    std::cout << "Cartea '" << c.titlu << "' a fost adaugata in biblioteca.\n";
}

void Biblioteca::inregistreazaUtilizator(Utilizator u) {
    lista_utilizatori.push_back(u);
    std::cout << "Utilizatorul " << u.nume << " " << u.prenume << " a fost inregistrat.\n";
}

void Biblioteca::realizeazaImprumut(int id_carte, int id_utilizator) {
    std::cout << "Se proceseaza imprumutul pentru cartea cu ID: " << id_carte 
              << " de catre utilizatorul cu ID: " << id_utilizator << ".\n";
}

void Biblioteca::realizeazaRetur(int id_carte) {
    std::cout << "S-a realizat returul pentru cartea cu ID: " << id_carte << ".\n";
}