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
    std::string status; 
    std::string sursa;
    

    Carte(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, int nr, std::string dest,std::string st,std::string s_sursa);
};


class CarteFictiune: public Carte{
    public:
        std::string personaj_principal;
        std::string serie;
        std::string tip_naratiune;
        int varsta_recomandata;

        CarteFictiune(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, int nr, std::string dest,std::string st, std::string s_sursa,  std::string p_p, std::string ser, std::string t_n, int varsta);
};

class CarteNonFictiune : public Carte {
public:
    std::string domeniu;
    std::string institutie_sursa;
    std::string editie_revizuita;

    CarteNonFictiune(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, int nr, std::string dest,std::string st, std::string s_sursa, std::string dom, std::string inst, std::string revz);
};

class CartePeriodica : public Carte {
public:
    int numar_editie;
    std::string frecventa;

    CartePeriodica(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, int nr, std::string dest, std::string st,std::string s_sursa, int nr_ed, std::string frecv);
};

#endif