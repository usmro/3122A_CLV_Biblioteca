#ifndef AUTOR_H
#define AUTOR_H

#include <string>
#include <vector>


class Carte;

class Autor {
public:
    std::string nume_complet;
    std::string biografie;    
    std::string cale_poza;    
    std::vector<Carte*> carti_in_biblioteca;

    Autor(const std::string& nume, const std::string& bio, const std::string& poza);
    
    void adaugaCarteInLista(Carte* c);
    void afiseazaProfilComplet();
};

#endif