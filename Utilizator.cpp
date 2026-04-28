#include "Utilizator.h"
#include<iostream>

std::string cripteazaParola(std::string pass) {
    for(int i = 0; i < pass.length(); i++) 
        pass[i] += 2; 
    return pass;
}


Utilizator::Utilizator(int id, std::string n, std::string p, std::string e, std::string tel, 
                       std::string d_n, std::string g, std::string pass, std::string d_i) 
    : id_utilizator(id), nume(n), prenume(p), email(e), telefon(tel), 
      data_nasterii(d_n), gen(g), data_inscriere(d_i), status("Activ") 
{
    parola_criptata = cripteazaParola(pass); 
}


Angajat::Angajat(int id, std::string n, std::string p, std::string e, std::string tel, 
                 std::string d_n, std::string g, std::string pass, std::string d_i, 
                 std::string functie_noua)
    : Utilizator(id, n, p, e, tel, d_n, g, pass, d_i) {
    
    functie = functie_noua;
    istoric.push_back("Schimbare statut: Devenit angajat cu functia " + functie_noua);
}

void Angajat::afiseazaTip() {
    std::cout << "[ANGAJAT] " << nume << " " << prenume << " - Functie: " << functie << "\n";
}


Voluntar::Voluntar(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string pass, std::string d_i, std::string sectie_noua, std::string coord_nume): Utilizator(id, n, p, e, tel, d_n, g, pass, d_i) {
    
    sectie_atribuita = sectie_noua;
    coordonator = coord_nume;
    ore_voluntariat = 0;
    istoric.push_back("Statut nou: Voluntar la sectia " + sectie_noua);
}

void Voluntar::afiseazaTip() {
    std::cout << "[VOLUNTAR] " << nume << " " << prenume << " | Coordonator: " << coordonator << " | Ore: " << ore_voluntariat << "\n";
}

void Voluntar::adaugaOre(int ore) {
    if (ore > 0) {
        ore_voluntariat += ore;
        istoric.push_back("S-au adaugat " + std::to_string(ore) + " ore de voluntariat.");
    }
}


Client::Client(int id, std::string n, std::string p, std::string e, std::string tel, std::string d_n, std::string g, std::string pass, std::string d_i, std::string cat)
    : Utilizator(id, n, p, e, tel, d_n, g, pass, d_i) {
    categorie = cat; 
    carti_active = 0;
    istoric.push_back("Client nou inregistrat (Categoria: " + cat + ").");
}

void Client::afiseazaTip() {
    std::cout << "[CLIENT] " << nume << " " << prenume << " - Categorie: " << categorie << " | Status: " << status << "\n";
}