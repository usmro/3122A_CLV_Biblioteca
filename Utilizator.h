#ifndef UTILIZATOR_H
#define UTILIZATOR_H

#include <string>
#include <vector>

// Clasa de Baza 
class Utilizator {
public:
    int id_utilizator;
    std::string nume, prenume, email, telefon, data_nasterii, gen, parola_criptata;
    std::string data_inscriere, status;
    std::vector<std::string> istoric; 

    Utilizator(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string pass, std::string d_i);
    
    
    virtual void afiseazaTip() = 0; 
    virtual ~Utilizator() {}
};

// Clasa pentru Clienti
class Client : public Utilizator {
public:
    std::string categorie; 
    int carti_active;
    std::vector<int> lista_imprumuturi_id;

    Client(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string pass, std::string d_i, std::string cat);
    void afiseazaTip() override;
};

// Clasa pentru Angajati
class Angajat : public Utilizator {
public:
    std::string functie; 
    std::vector<std::string> istoric_pozitii; 
    std::vector<std::string> concedii;       

    Angajat(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string pass, std::string d_i, std::string func);
    void afiseazaTip() override;
};

// Clasa pentru Voluntari
class Voluntar : public Utilizator {
public:
    std::string sectie_atribuita; 
    int ore_voluntariat;
    std::string coordonator; 

    Voluntar(int id, std::string n, std::string p, std::string e, std::string tel, 
             std::string d_n, std::string g, std::string pass, std::string d_i, 
             std::string sectie, std::string coord);
    void afiseazaTip() override;
    void adaugaOre(int ore);
};
#endif