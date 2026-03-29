#include "Carte.h"

Carte::Carte(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, int nr, std::string dest, std::string st,std::string s_sursa) {
    id_carte = id;
    titlu = t;
    autor = a;
    editura = ed;
    isbn = i;
    limba = l;
    data_pub = d_p;
    nr_pagini = nr;
    destinatar = dest;
    status = "Disponibila";
    sursa = s_sursa;
     
}

CarteFictiune::CarteFictiune(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, int nr, std::string dest,std::string st, std::string s_sursa, std::string p_p, std::string ser,std::string t_n, int varsta) : Carte(id, t, a, ed, i, l, d_p, nr, dest,st, s_sursa) {
    personaj_principal = p_p;
    serie = ser;
    tip_naratiune=t_n;
    varsta_recomandata=varsta;
}

CarteNonFictiune::CarteNonFictiune(int id, std::string t, std::string a, std::string ed, std::string i,  std::string l, std::string d_p, int nr, std::string dest,std::string st, std::string s_sursa,  std::string dom, std::string inst, std::string revz): Carte(id, t, a, ed, i, l, d_p, nr, dest,st, s_sursa) {
    domeniu = dom;
    institutie_sursa = inst;
    editie_revizuita=revz;
}

CartePeriodica::CartePeriodica(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, int nr, std::string dest, std::string st,std::string s_sursa, int nr_ed, std::string frecv): Carte(id, t, a, ed, i, l, d_p, nr, dest,st, s_sursa) {
    numar_editie = nr_ed;
    frecventa = frecv;
}