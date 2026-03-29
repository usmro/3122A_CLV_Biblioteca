#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "Carte.h"
#include "Utilizator.h"
#include <vector>

class Biblioteca{
    public:
        std::vector<Carte> inventar_carti;
        std::vector<Utilizator> lista_utilizatori;

        Biblioteca();

        void adaugaCarte(Carte c);
        void inregistreazaUtilizator(Utilizator u);
        void realizeazaImprumut(int id_carte, int id_utilizator);
        void realizeazaRetur(int id_carte);
};

#endif