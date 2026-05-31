#ifndef ANGAJAT_H
#define ANGAJAT_H

#include "../Voluntari/Voluntar.h"
#include "../../Cladiri/Cladire.h"
#include "../../Carte/Carte.h"
#include <vector>
#include <string>
#include <sstream>

// --- CLASA BAZA ANGAJAT (Moștenește Voluntar) ---
class Angajat : public Voluntar {
protected:
    double salariu_baza;
    int zile_concediu_ramase;
    int id_superior;
    std::string parola_resursa; // Parola pentru Drive/Arhiva/Server
    std::vector<std::string> istoric_pozitii;

public:
    Angajat(int id, std::string n, std::string p, std::string e, std::string tel,
            std::string dn, std::string g, std::string pass, std::string poza,
            std::string c_contract, std::string d_start, std::string d_end,
            std::string l_idei, std::string l_inscriere, std::string l_prog, 
            std::string l_pers, int ore,
            double sal, int concediu, int id_sup, std::string pass_res);

    virtual ~Angajat() {}

    // Metode specifice angajatilor
    void adaugaIstoricPozitie(std::string pozitie_noua);
    bool verificaParolaResursa(std::string incercare) const;
    
    // Functii virtuale pentru personalizare
    virtual double calculeazaVenitTotal() const;
    virtual void afiseazaInterfataSpecifica() const;
    void solicitaConcediu(int nr_zile);
    void aprobaConcediu(int nr_zile);   // apelata de superior / HR
    void afiseazaFluturas() const;

    int getZileConcediu() const { return zile_concediu_ramase; }
    double getSalariu()   const { return salariu_baza; }

    // Gettere
    int getIdSuperior() const { return id_superior; }
    std::string exportaInCSV() const override {
        std::stringstream ss;
        ss << Voluntar::exportaInCSV() << ";" 
           << salariu_baza << ";" 
           << zile_concediu_ramase << ";" 
           << id_superior << ";" 
           << parola_resursa;
        return ss.str();
    }
};

// --- 1. CONDUCERE (Director, HR, Secretariat, IT) ---
class AngajatConducere : public Angajat {
public:
    using Angajat::Angajat;
    double calculeazaVenitTotal() const override;
    void afiseazaInterfataSpecifica() const override;
};

// --- 2. SPECIALISTI (Bibliotecari, Catalogare) ---
class AngajatSpecialist : public Angajat {
public:
    using Angajat::Angajat;
    double calculeazaVenitTotal() const override;
    void afiseazaInterfataSpecifica() const override;
    void adaugaCarteInSistem(Cladire& filiala, Carte* noua_carte);
    void stergeCarteDinSistem(int id_carte);
    void modificaStatusCarte(int id_carte, std::string noul_status);
};

// --- 3. CREATIVI (PR, Evenimente, Designeri) ---
class AngajatCreativ : public Angajat {
public:
    using Angajat::Angajat;
    double calculeazaVenitTotal() const override;
    void afiseazaInterfataSpecifica() const override;
};

// --- 4. SUPORT (Paza, Cafenea, Curatenie) ---
class AngajatSuport : public Angajat {
public:
    using Angajat::Angajat;
    double calculeazaVenitTotal() const override;
    void afiseazaInterfataSpecifica() const override;
};

#endif