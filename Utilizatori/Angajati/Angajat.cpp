#include "Angajat.h"
#include <iostream>

// Constructorul Clasei de Bază Angajat
Angajat::Angajat(int id, std::string n, std::string p, std::string e, std::string tel,
                 std::string dn, std::string g, std::string pass, std::string poza,
                 std::string c_contract, std::string d_start, std::string d_end,
                 std::string l_idei, std::string l_inscriere, std::string l_prog, 
                 std::string l_pers, int ore,
                 double sal, int concediu, int id_sup, std::string pass_res)
    : Voluntar(id, n, p, e, tel, dn, g, pass, poza, c_contract, d_start, d_end, 
               l_idei, l_inscriere, l_prog, l_pers, ore),
      salariu_baza(sal), zile_concediu_ramase(concediu), 
      id_superior(id_sup), parola_resursa(pass_res) 
{
    this->status = "Online";
}
double Angajat::calculeazaVenitTotal() const {
    return salariu_baza;
}

void Angajat::afiseazaInterfataSpecifica() const {
    std::cout << "Interfata standard angajat.\n";
}

void Angajat::adaugaIstoricPozitie(std::string pozitie_noua) {
    istoric_pozitii.push_back(pozitie_noua);
}

bool Angajat::verificaParolaResursa(std::string incercare) const {
    return parola_resursa == incercare;
}

// --- 1. Implementări AngajatConducere ---
double AngajatConducere::calculeazaVenitTotal() const {
    return salariu_baza + 1000; // Bonus de conducere
}

void AngajatConducere::afiseazaInterfataSpecifica() const {
    std::cout << "Interfata ADMIN: Gestionare bugete si personal.\n";
}

// --- 2. Implementări AngajatSpecialist ---
double AngajatSpecialist::calculeazaVenitTotal() const {
    return salariu_baza + 500; // Bonus de specialitate
}

void AngajatSpecialist::afiseazaInterfataSpecifica() const {
    std::cout << "Interfata CATALOGARE: Adaugare/Stergere carti.\n";
}

void AngajatSpecialist::adaugaCarteInSistem(Cladire& filiala, Carte* noua_carte) {
    filiala.adaugaCarteInDepozit(noua_carte);
    std::cout << "[SISTEM] Cartea a fost inregistrata de " << nume << ".\n";
}

void AngajatSpecialist::stergeCarteDinSistem(int id_carte) {
    std::cout << "[SISTEM] Cerere stergere pentru ID " << id_carte << " trimisa.\n";
}

void AngajatSpecialist::modificaStatusCarte(int id_carte, std::string noul_status) {
    std::cout << "[SISTEM] Status actualizat pentru " << id_carte << " la: " << noul_status << "\n";
}

// --- 3. Implementări AngajatCreativ ---
double AngajatCreativ::calculeazaVenitTotal() const {
    return salariu_baza + (ore_lucrate * 10); // Bonus bazat pe implicare
}

void AngajatCreativ::afiseazaInterfataSpecifica() const {
    std::cout << "Interfata DESIGN: Promovare evenimente si PR.\n";
}

// --- 4. Implementări AngajatSuport ---
double AngajatSuport::calculeazaVenitTotal() const {
    return salariu_baza;
}

void AngajatSuport::afiseazaInterfataSpecifica() const {
    std::cout << "Interfata LOGISTICA: Mentenanta si securitate.\n";
}