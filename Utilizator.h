#ifndef UTILIZATOR_H
#define UTILIZATOR_H

#include <string>
#include <vector>

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
    std::string status;
    int carti_active;
    
    
    std::vector<int> lista_imprumuturi;

    Utilizator(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string tip, std::string d_i);
};

#endif