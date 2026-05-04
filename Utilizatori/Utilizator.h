#ifndef UTILIZATOR_H
#define UTILIZATOR_H

#include <string>
#include <vector>
#include <iostream>

// Clasa de Baza 
class Utilizator {
public:
    int id_utilizator;
    std::string nume, prenume, email, telefon, data_nasterii, gen, parola_criptata;
    std::string data_inscriere, status,cale_poza;
    std::vector<std::string> istoric; 

    Utilizator(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string pass, std::string d_i, std::string poza);
    int getId() const { return id_utilizator; }
    
    virtual void afiseazaTip(); 
    virtual std::string exportaInCSV() const = 0;
    virtual ~Utilizator() {}
};

#endif