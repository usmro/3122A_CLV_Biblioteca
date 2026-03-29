#include "Utilizator.h"

Utilizator::Utilizator(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string tip, std::string d_i) {
    id_utilizator = id;
    nume = n;
    prenume = p;
    email = e;
    telefon = tel;
    data_nasterii = d_n;
    gen = g;
    tip_utilizator = tip;
    data_inscriere = d_i;
    status="Activ";
    carti_active = 0; 
}