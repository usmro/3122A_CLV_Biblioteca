#ifndef CARTE_H
#define CARTE_H

#include <string>
#include <vector>

class Carte {
public:
    int id_carte;
    std::string titlu;
    std::string autor;
    std::string editura;
    std::string isbn;
    std:: string limba;
    std::string data_pub;
    int nr_pagini;
    std::string destinatar;
    std::string subiect;
    std::string note;
    std::sursa;
    std::string status; 
};

class Utilizator {
public:
    int id_utilizator;
    std::string nume;
    std::string prenume;
    std::string email;
    std::string telefon;
    std::string data_nasterii;
    std::string gen;
    std::string tip_utilizator; 
    std::string data_inscriere;
    int carti_active;
    
    
    std::vector<int> lista_imprumuturi;
};

#endif