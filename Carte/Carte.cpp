#include "Carte.h"
#include <iostream>

// Constructor Baza
Carte::Carte(int id, std::string t, std::string a, std::string ed, std::string l, std::string d_p, 
             int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag, std::string sc, 
             int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb) 
    : id_carte(id), titlu(t), autor(a), editura(ed), limba(l), data_pub(d_p), 
      status(st), sursa(s_sursa), destinatar(dest),tara_provenienta(tara), an_aparitie(an),nr_pagini(pag), serie_contabila(sc), 
      exemplare_disponibile(stoc), este_patrimoniu(p), pret_intrare(pret), 
      rating(rat), rezumat(rez), suport(sup), valoare_masurabila(val), marime_mb(mb) {}



void Carte::afiseazaStareFizica() {
    std::cout << "Serie Contabila: " << serie_contabila << "\n";
    if (defecte.empty()) {
        std::cout << "Stare: Impecabila (fara defecte semnalate).\n";
    } else {
        std::cout << "Defecte identificate (Art. 41): ";
        for (const std::string& def : defecte) {
            std::cout << "[" << def << "] ";
        }
        std::cout << "\n";
    }
}

// Implementare Fictiune
CarteFictiune::CarteFictiune(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, 
                             int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag,std::string sc, 
                             int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb, 
                             std::string gen, std::string p_p, std::string ser, std::string t_n, 
                             int varsta, std::string dim, float gr, std::string coperta)
    : Carte(id, t, a, ed, l, d_p, val, dest, st, s_sursa, tara, an, pag,sc, stoc, p, pret, rez, rat, sup, mb), 
      isbn(i), gen_specific(gen), personaj_principal(p_p), serie(ser), tip_naratiune(t_n),
      varsta_recomandata(varsta), dimensiuni(dim), greutate(gr), tip_coperta(coperta) {}

void CarteFictiune::afiseazaFisaCatalog() {
    std::cout << "[FICTIUNE - " << gen_specific << "] " << titlu << " (" << autor << ")\n"
              << "Rating: " << rating << "/5 | ISBN: " << isbn << " | Personaj: " << personaj_principal << "\n";
    if (suport != TipSuport::FIZIC) std::cout << "Format Digital: " << marime_mb << " MB\n";
    if (este_patrimoniu) std::cout << "!!! UNITATE DE PATRIMONIU - DOAR CONSULTARE LA SALA !!!\n";
    std::cout << "-------------------------------------------\n";
}

// Implementare Non-Fictiune
CarteNonFictiune::CarteNonFictiune(int id, std::string t, std::string a, std::string ed, std::string i, std::string l, std::string d_p, 
                                   int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag, std::string sc, 
                                   int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb, 
                                   std::string subcat, std::string dom, std::string inst, 
                                   std::string revz, std::string dim, float gr)
    : Carte(id, t, a, ed, l, d_p, val, dest, st, s_sursa, tara, an, pag,sc, stoc, p, pret, rez, rat, sup, mb), 
      isbn(i), subcategorie(subcat), domeniu(dom), institutie_sursa(inst), editie_revizuita(revz), 
      dimensiuni(dim), greutate(gr) {}

void CarteNonFictiune::afiseazaFisaCatalog() {
    std::cout << "[NON-FICTIUNE - " << domeniu << "] " << titlu << "\n"
              << "Subcategorie: " << subcategorie << " | Sursa: " << institutie_sursa << " | Revizie: " << editie_revizuita << "\n"
              << "-------------------------------------------\n";
}

// Implementare Periodice
CartePeriodica::CartePeriodica(int id, std::string t, std::string a, std::string ed, std::string i_issn, std::string l, std::string d_p, 
                               int val, std::string dest, std::string st, std::string s_sursa,std::string tara, int an, int pag, std::string sc, 
                               int stoc, bool p, float pret, std::string rez, float rat, TipSuport sup, float mb, 
                               int nr_ed, std::string frecv)
    : Carte(id, t, a, ed, l, d_p, val, dest, st, s_sursa,tara, an, pag, sc, stoc, p, pret, rez, rat, sup, mb), 
      issn(i_issn), numar_editie(nr_ed), frecventa(frecv) {}

void CartePeriodica::afiseazaFisaCatalog() {
    std::cout << "[PERIODIC - ISSN: " << issn << "] " << titlu << "\n"
              << "Editia: " << numar_editie << " | Frecventa: " << frecventa << " | Limba: " << limba << "\n"
              << "-------------------------------------------\n";
}

void Carte::adaugaRecenzieSociala(int id_user, std::string user, int nota, std::string text) {
    Recenzie r = {id_user, user, nota, text};
    lista_recenzii.push_back(r);
    actualizeazaRatingMediu();
}

void Carte::actualizeazaRatingMediu() {
    if (lista_recenzii.empty()) return;
    float suma = 0;
    for (const auto& r : lista_recenzii) suma += r.nota;
    rating = suma / lista_recenzii.size();
}

void Carte::afiseazaFisaCatalog() {
    std::cout << "--- " << titlu << " [" << autor << "] ---\n";
    std::cout << "Status: " << status << " | Rating: " << rating << "*/5\n";
    std::cout << "Rezumat: " << rezumat.substr(0, 100) << "...\n";
}

void Carte::adaugaDefect(std::string descriere) {
    defecte.push_back(descriere);
}