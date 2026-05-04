#include "Biblioteca.h"
#include "Carte.h"
#include "Autor.h"
#include "Cladiri/Cladire.h"
#include "Utilizatori/Voluntari/Voluntar.h"
#include "Utilizatori/Angajati/Angajat.h"
#include "Utilizatori/Utilizator.h"
#include "Utilizatori/Clienti/Client.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

template <typename T>
bool contine(const std::vector<T>& vec, const T& valoare) {
    if (vec.empty()) return true; 
    return std::find(vec.begin(), vec.end(), valoare) != vec.end();
}
bool comparaAutori(Carte* a, Carte* b) { return a->autor < b->autor; }
bool comparaEdituri(Carte* a, Carte* b) { return a->editura < b->editura; }

bool comparaAni(Carte* a, Carte* b) { 
    return a->data_pub < b->data_pub; 
}

Biblioteca::Biblioteca() {
    incarcaLogistica();
    if (lista_filiale.empty()) {
        initializeazaLogisticaA_F();
    }
    incarcaAutori();
    incarcaBazaDate();

    coreleazaAutoriCuCarti();
    populeazaAutoriDinCarti();
}

Biblioteca::~Biblioteca() {
    for (Carte* c : inventar_general) delete c;
    for (Autor* a : baza_date_autori) delete a;
    for (Client* cl : lista_clienti) delete cl;
    for (Voluntar* v : lista_voluntari) delete v;
    for (Angajat* an : lista_angajati) delete an;
}

void Biblioteca::salveazaLogistica() {
    std::ofstream fC("cladiri.csv");
    if (!fC.is_open()) return;
    fC << "Nume;Adresa;Contact;Email;Etaje;Angajati;Mese;Scaune;Capacitate;Central\n";
    for (const auto& c : lista_filiale) {
        fC << c.nume_filiala << ";" << c.adresa << ";" << c.numar_contact << ";" << c.email_contact << ";"
           << c.numar_etaje << ";" << c.numar_total_angajati << ";" << c.numar_mese << ";" 
           << c.numar_scaune << ";" << c.capacitate_maxima_carti << ";" << c.este_sediu_central << "\n";
    }
}

void Biblioteca::incarcaLogistica() {
    std::ifstream fC("cladiri.csv");
    std::string linie;

    if (fC.is_open()) {
        std::getline(fC, linie); 
        while (std::getline(fC, linie)) {
            if (linie.empty()) continue;
            std::stringstream ss(linie);
            std::vector<std::string> d;
            std::string camp;
            while (std::getline(ss, camp, ';')) d.push_back(camp);

            if (d.size() >= 10) {
                Cladire c(d[0], d[1], std::stoi(d[5]), std::stoi(d[4]), 0, 
                         std::stoi(d[6]), std::stoi(d[7]), std::stoi(d[8]), (d[9] == "1"), "08:00", "20:00");
                c.numar_contact = d[2];
                c.email_contact = d[3];
                lista_filiale.push_back(c);
            }
        }
        fC.close();
    }
    std::ifstream fS("sali.csv");
    if (fS.is_open()) {
        std::getline(fS, linie); 
        while (std::getline(fS, linie)) {
            if (linie.empty()) continue;
            std::stringstream ss(linie);
            std::string fil, numeS, tip, et, cap;
            std::getline(ss, fil, ';'); std::getline(ss, numeS, ';');
            std::getline(ss, tip, ';'); std::getline(ss, et, ';'); std::getline(ss, cap, ';');

            for (auto& cladire : lista_filiale) {
                if (cladire.nume_filiala == fil) {
                    cladire.adaugaSala(numeS, tip, std::stoi(et), std::stoi(cap));
                    break;
                }
            }
        }
    }
}
void Biblioteca::initializeazaLogisticaA_F() {
    Cladire a("Corpul A", "Str. Principala 1", 30, 4, 14, 150, 400, 100000, true, "08:00", "20:00");
    a.numar_contact = "021-001"; a.email_contact = "admin@bibl.ro";
    a.adaugaSala("Secretariat", "Secretariat", 0, 5);
    a.adaugaSala("Aula Magna", "Eveniment", 0, 150);
    a.adaugaSala("Birou Director", "Conducere", 1, 3);
    for(int i=1; i<=11; i++) a.adaugaSala("Sala Lectura A" + std::to_string(i), "Lectura", 2, 30);
    lista_filiale.push_back(a);

    Cladire b("Corpul B", "Str. Principala 1", 15, 3, 10, 100, 200, 60000, false, "08:00", "20:00");
    for(int i=1; i<=5; i++) b.adaugaSala("Cabina Studiu " + std::to_string(i), "Studiu Individual", 1, 1);
    lista_filiale.push_back(b);

    Cladire c("Corpul C", "Str. Principala 1", 8, 1, 3, 30, 60, 5000, false, "09:00", "22:00");
    c.adaugaSala("Cafenea Literara", "Cafenea", 0, 40);
    lista_filiale.push_back(c);

    //CORP D

    // CORP E

    // CORP F


    salveazaLogistica();
}
void Biblioteca::afiseazaHartaGrupata() {
    for (auto& cladire : lista_filiale) {
        std::cout << "\n>>> CARTI IN " << cladire.nume_filiala << " <<<\n";
        bool are_carti = false;
        for (auto c : inventar_general) {
            if (c->adresa_raft.nume_filiala == cladire.nume_filiala) {
                std::cout << " - " << c->titlu << " (Etaj " << c->adresa_raft.etaj << ", Camera " << c->adresa_raft.camera << ")\n";
                are_carti = true;
            }
        }
        if (!are_carti) std::cout << " [Nicio carte depozitata aici]\n";
    }
}
void Biblioteca::populeazaAutoriDinCarti() {
    for (auto c : inventar_general) {
        bool exista = false;
        for (auto a : baza_date_autori) {
            if (a->nume_complet == c->autor) {
                exista = true;
                break;
            }
        }
        if (!exista) {
            baza_date_autori.push_back(new Autor(c->autor, "Biografie in curs de actualizare...", "default_autor.jpg"));
        }
    }
}

void Biblioteca::adaugaCarte(Carte* c, Locatie loc) {
    bool filiala_gasita = false;
    for (size_t i = 0; i < lista_filiale.size(); i++) {
        if (lista_filiale[i].nume_filiala == loc.nume_filiala) {
            filiala_gasita = true;
            break;
        }
    }

    inventar_general.push_back(c);

    if (filiala_gasita) {
        std::cout << "[LOGISTICA] Unitatea '" << c->titlu << "' alocata filialei " 
                  << loc.nume_filiala << ", Raft " << loc.raft << ".\n";
    } else {
        std::cout << "[AVERTISMENT] Locatia '" << loc.nume_filiala 
                  << "' nu este inregistrata! Documentul ramane in depozitul central.\n";
    }
    salveazaBazaDate();
}

void Biblioteca::afiseazaTotInventarul() {
    std::cout << "\n======= INVENTAR COMPLET =======\n";
    for (Carte* c : inventar_general) {
        c->afiseazaFisaCatalog();
    }
}

void Biblioteca::afiseazaRegulament() {
    std::ifstream fisier("Regulament.txt");
    std::string linie;
    if (fisier.is_open()) {
        std::cout << "\n--- REGULAMENTUL INTERN AL BIBLIOTECII ---\n";
        while (std::getline(fisier, linie)) {
            std::cout << linie << std::endl;
        }
        fisier.close();
    } else {
        std::cout << "Eroare: Fisierul 'Regulament.txt' nu a fost gasit!\n";
    }
}

void Biblioteca::adaugaAutor(Autor* a) {
    baza_date_autori.push_back(a);
}

void Biblioteca::coreleazaAutoriCuCarti() {
    for (Autor* a : baza_date_autori) {
        a->carti_in_biblioteca.clear();
        for (Carte* c : inventar_general) {
            if (c->autor == a->nume_complet) {
                a->adaugaCarteInLista(c);
            }
        }
    }
}

void Biblioteca::cautaInfoAutor(const std::string& nume_cautat) {
    coreleazaAutoriCuCarti();
    bool gasit = false;
    for (Autor* a : baza_date_autori) {
        if (a->nume_complet.find(nume_cautat) != std::string::npos) {
            a->afiseazaProfilComplet();
            gasit = true;
            break;
        }
    }
    if (!gasit) std::cout << "Autorul " << nume_cautat << " nu figureaza in baza de date.\n";
}

void Biblioteca::afiseazaDisponibilitateExemplare(const std::string& titlu_cautat, const std::string& autor_cautat) {
    bool gasit = false;
    for (Carte* c : inventar_general) {
        if (c->titlu == titlu_cautat && c->autor == autor_cautat) {
            gasit = true;
            std::cout << "Editura: " << c->editura << " | Suport: " 
                      << (c->suport == TipSuport::FIZIC ? "Fizic" : "Digital");
            
            if (c->este_patrimoniu) {
                std::cout << " | [DOAR CONSULTARE LA SALA - REGIM SPECIAL]\n";
            } else {
                std::cout << " | Stoc: " << c->exemplare_disponibile << " exemplare.\n";
            }
        }
    }
    if (!gasit) std::cout << "Nu s-au gasit exemplare pentru cautarea efectuata.\n";
}

std::vector<Carte*> Biblioteca::cautaDupaTitlu(const std::string& titluCautat) {
    std::vector<Carte*> rezultate;
    
    for (Carte* c : inventar_general) {
        // Verificăm dacă titlul cărții conține subșirul căutat
        if (c->titlu.find(titluCautat) != std::string::npos) {
            rezultate.push_back(c);
        }
    }
    
    return rezultate; 
}

std::vector<Carte*> Biblioteca::cautaDupaAutor(const std::string& autor) {
    std::vector<Carte*> rezultate;
    for (Carte* c : inventar_general) {
        if (c->autor.find(autor) != std::string::npos) {
            rezultate.push_back(c);
        }
    }
    return rezultate;
}



std::vector<Carte*> Biblioteca::filtreazaAvansat(const CriteriiFiltrare& criterii) {
    std::vector<Carte*> rezultate;

    for (Carte* c : inventar_general) {
        if (!contine(criterii.formate, c->suport)) continue;
        if (!contine(criterii.limbi, c->limba)) continue;
        if (!contine(criterii.tari, c->tara_provenienta)) continue;
        if (!contine(criterii.ani, c->an_aparitie)) continue;
        if (c->rating < criterii.ratingMinim) continue;
        if (c->nr_pagini < criterii.paginiMin || c->nr_pagini > criterii.paginiMax) continue;

        if (!criterii.genuri.empty()) {
            CarteFictiune* f = dynamic_cast<CarteFictiune*>(c);
            if (!f || !contine(criterii.genuri, f->gen_specific)) continue;
        }

        rezultate.push_back(c);
    }
    return rezultate;
}

void Biblioteca::filtreazaDupaRating(float ratingMinim) {
    CriteriiFiltrare c;
    c.ratingMinim = ratingMinim;
    std::vector<Carte*> filtrate = filtreazaAvansat(c);
    
    for (Carte* carte : filtrate) {
        std::cout << "[Rating: " << carte->rating << "] " << carte->titlu << "\n";
    }
}

void Biblioteca::sorteazaDupaAutor(std::vector<Carte*>& lista) {
    std::sort(lista.begin(), lista.end(), comparaAutori);
}

void Biblioteca::sorteazaDupaEditura(std::vector<Carte*>& lista) {
    std::sort(lista.begin(), lista.end(), comparaEdituri);
}

void Biblioteca::sorteazaDupaAn(std::vector<Carte*>& lista) {
    std::sort(lista.begin(), lista.end(), [](Carte* a, Carte* b) {
        return a->an_aparitie < b->an_aparitie;
    });
}

void Biblioteca::inregistreazaClient(Client c) {
    lista_clienti.push_back(new Client(c));
    std::cout << "Client inregistrat.\n";
}

void Biblioteca::inregistreazaAngajat(Angajat* a) { 
    if (a != nullptr) {
        lista_angajati.push_back(a); 
        std::cout << "Datele angajatului au fost actualizate.\n";
    }
}

void Biblioteca::inregistreazaVoluntar(Voluntar v) {
    lista_voluntari.push_back(new Voluntar(v));
    std::cout << "Colaborator (Voluntar) inregistrat pentru activitati conexe.\n";
}

void Biblioteca::salveazaBazaDate() {
    std::ofstream f("inventar.txt");
    if (!f.is_open()) return;

    for (Carte* c : inventar_general) {
        char tip = 'C';
        if (dynamic_cast<CarteFictiune*>(c)) tip = 'F';
        else if (dynamic_cast<CarteNonFictiune*>(c)) tip = 'N';
        else if (dynamic_cast<CartePeriodica*>(c)) tip = 'P';

        std::string t = c->titlu; std::replace(t.begin(), t.end(), ' ', '_');
        std::string a = c->autor; std::replace(a.begin(), a.end(), ' ', '_');

        f << tip << " " << c->id_carte << " " << t << " " << a << " " 
          << c->rating << " " << c->nr_pagini << " " << c->adresa_raft.nume_filiala << " " 
          << c->tara_provenienta << " " << c->an_aparitie << " " 
          << c->exemplare_disponibile << " " << (int)c->suport << "\n";
    }
    f.close();
}

void Biblioteca::incarcaBazaDate() {
    std::ifstream f("inventar.txt");
    if (!f.is_open()) return;

    char tip; 
    while (f >> tip) {
        int id, an, pag, stoc, sup_int;
        float rat;
        std::string titlu, autor, nume_corp, tara;

        if(!(f >> id >> titlu >> autor >> rat >> pag >> nume_corp >> tara >> an >> stoc >> sup_int)) break;

        std::replace(titlu.begin(), titlu.end(), '_', ' ');
        std::replace(autor.begin(), autor.end(), '_', ' ');
        TipSuport sup = static_cast<TipSuport>(sup_int);

        Carte* noua = nullptr;

        if (tip == 'F') {
            noua = new CarteFictiune(id, titlu, autor, "Editura Default", "ISBN-TEMP", "Romana", "2026-01-01", 
                                     pag, "Biblioteca", "Disponibil", "Achizitie", tara, an, pag, "SC-"+std::to_string(id), 
                                     stoc, false, 0.0f, "Rezumat...", rat, sup, 0.0f, 
                                     "Gen", "Protagonist", "Serie", "Naratiune", 12, "Standard", 0.5f, "Cartonata");
        } 
        else if (tip == 'N') {
            noua = new CarteNonFictiune(id, titlu, autor, "Editura Stiintifica", "ISBN-TEMP", "Romana", "2026-01-01",
                                        pag, "Cercetare", "Disponibil", "Sursa", tara, an, pag, "SC-"+std::to_string(id),
                                        stoc, false, 0.0f, "Rezumat...", rat, sup, 0.0f,
                                        "Subcategorie", "Domeniu", "Institutie", "v1.0", "Standard", 0.7f);
        }
        else if (tip == 'P') {
            noua = new CartePeriodica(
                    id, titlu, autor, "Editura Media", "ISSN-TEMP", "Romana", "2026-04-28",
                    pag, "Public", "Disponibil", "Abonament", tara, an, pag, "SC-"+std::to_string(id),
                    stoc, false, 0.0f, "Editie periodica...", rat, sup, 0.0f,
                    1, "Lunar" 
             );
        }

        if (noua) {
            noua->adresa_raft.nume_filiala = nume_corp; 
            inventar_general.push_back(noua);
        }
    }
    f.close();
}
void Biblioteca::incarcaClienti(const std::string& nume_fisier) {
    std::ifstream fisier(nume_fisier);
    if (!fisier.is_open()) return;
    std::string linie;
    std::getline(fisier, linie); 

    while (std::getline(fisier, linie)) {
        if (linie.empty()) continue;
        std::stringstream ss(linie);
        std::string t_id, n, p, e, tel, dN, g, pass, poza, user, t_v, dI, r;
        
        std::getline(ss, t_id, ';'); std::getline(ss, n, ';'); std::getline(ss, p, ';');
        std::getline(ss, e, ';'); std::getline(ss, tel, ';'); std::getline(ss, dN, ';');
        std::getline(ss, g, ';'); std::getline(ss, pass, ';'); std::getline(ss, poza, ';');
        std::getline(ss, user, ';'); std::getline(ss, t_v, ';'); std::getline(ss, dI, ';');
        std::getline(ss, r, ';');

        try {
            int id = std::stoi(t_id);
            int varsta = std::stoi(t_v);
            // REPARAT: Constructorul cere 13 argumente
            lista_clienti.push_back(new Client(id, n, p, e, tel, dN, g, pass, user, varsta, dI,"novice", poza));
        } catch (...) {}
    }
}
void Biblioteca::stergeClient(int id_cautat) {
    // Căutăm clientul în vectorul de clienți
    for (auto it = lista_clienti.begin(); it != lista_clienti.end(); ++it) {
        if ((*it)->getId() == id_cautat) {
            
            delete *it;
            lista_clienti.erase(it);
            
            std::cout << "[SISTEM] Clientul cu ID " << id_cautat << " a fost sters.\n";
            
            return;
        }
    }
    std::cout << "[EROARE] Clientul cu ID " << id_cautat << " nu a fost gasit.\n";
}
void Biblioteca::realizeazaImprumut(int id_c, int id_u, const std::string& data_azi) {
    for (Carte* c : inventar_general) {
        if (c->id_carte == id_c) {
            if (c->este_patrimoniu) {
                std::cout << "[EROARE] Patrimoniu - Doar consultare sala!\n";
                return;
            }
            if (c->exemplare_disponibile > 0) {
                c->exemplare_disponibile--;
                InregistrareImprumut nou = {id_u, id_c, data_azi, "Data+21zile", false};
                istoric_imprumuturi.push_back(nou);
                std::cout << "[SUCCES] Imprumutat: " << c->titlu << "\n";
                salveazaBazaDate();
            } else std::cout << "[STOC 0]\n";
            return;
        }
    }
}

void Biblioteca::realizeazaRetur(int id_carte, const std::string& data_retur_reala) {
    bool gasit = false;
    for (auto& imp : istoric_imprumuturi) {
        if (imp.id_carte == id_carte && !imp.returnata) {
            imp.returnata = true;
            gasit = true;
            break;
        }
    }
    if (gasit) {
        for (Carte* c : inventar_general) {
            if (c->id_carte == id_carte) {
                c->exemplare_disponibile++;
                break;
            }
        }
        salveazaBazaDate();
    }
}


void Biblioteca::realizareInventar(const std::string& nume_angajat) {
    std::string data_azi = "2026-04-16"; 
    int gasite = 0;
    std::string observatii = "";

    std::cout << "\n--- PROCEDURA PERIODICA DE INVENTARIERE ---\n";
    for (Carte* c : inventar_general) {
        std::cout << "Verificare integritate serie: " << c->serie_contabila << "... OK\n";
        if (!c->defecte.empty()) observatii += "ID " + std::to_string(c->id_carte) + " prezinta uzura; ";
        gasite++;
    }

    std::ofstream f("Proces_Verbal_Inventar.txt", std::ios::app);
    if (f.is_open()) {
        f << "Data: " << data_azi << " | Responsabil: " << nume_angajat << " | Total unitati: " << gasite << "\n";
        f.close();
        std::cout << "Raportul de control a fost salvat in arhiva.\n";
    }
}

void Biblioteca::incarcaVoluntari(const std::string& nume_fisier) {
    std::ifstream fisier(nume_fisier);
    if (!fisier.is_open()) return;
    std::string linie; std::getline(fisier, linie);
    while (std::getline(fisier, linie)) {
        if (linie.empty()) continue;
        std::stringstream ss(linie);
        std::string id_s, n, p, e, tel, dN, g, pass, poza, cont, dS, dE, lI, lIn, lPr, lPe, ore_s;
        
        std::getline(ss, id_s, ';'); std::getline(ss, n, ';'); std::getline(ss, p, ';');
        std::getline(ss, e, ';'); std::getline(ss, tel, ';'); std::getline(ss, dN, ';');
        std::getline(ss, g, ';'); std::getline(ss, pass, ';'); std::getline(ss, poza, ';');
        std::getline(ss, cont, ';'); std::getline(ss, dS, ';'); std::getline(ss, dE, ';');
        std::getline(ss, lI, ';'); std::getline(ss, lIn, ';'); std::getline(ss, lPr, ';');
        std::getline(ss, lPe, ';'); std::getline(ss, ore_s, ';');

        try {
            Voluntar* v = new Voluntar(std::stoi(id_s), n, p, e, tel, dN, g, pass, poza, 
                           cont, dS, dE, lI, lIn, lPr, lPe, std::stoi(ore_s));
            lista_voluntari.push_back(v);
        } catch (...) {}
    }
}

void Biblioteca::incarcaAngajati(const std::string& /*nume_fisier*/) {
    std::ofstream fisier("angajati.csv");
    if (!fisier.is_open()) return;

    fisier << "ID;Nume;Prenume;Email;Telefon;DataN;Gen;Parola;Poza;Contract;DStart;DEnd;LIdei;LInscriere;LProg;LPers;Ore;Salariu;Concediu;IDSef;ParolaResursa;TipAngajat\n";

    for (auto a : lista_angajati) {
        fisier << a->exportaInCSV() << "\n";}
}
void Biblioteca::stergeAngajat(int id_cautat) {
    // 1. Căutăm angajatul în vector
    for (auto it = lista_angajati.begin(); it != lista_angajati.end(); ++it) {
        if ((*it)->getId() == id_cautat) {
            delete *it;
            lista_angajati.erase(it);
            std::cout << "Angajatul cu ID " << id_cautat << " a fost sters din sistem.\n";
             
            return;
        }
    }
    std::cout << "Eroare: Nu s-a gasit niciun angajat cu ID-ul " << id_cautat << ".\n";
}

void Biblioteca::stergeVoluntar(int id_cautat) {
    for (auto it = lista_voluntari.begin(); it != lista_voluntari.end(); ++it) {
        if ((*it)->getId() == id_cautat) {
            delete *it; 
            lista_voluntari.erase(it);
            
            std::cout << "[SISTEM] Voluntarul cu ID " << id_cautat << " a fost eliminat.\n";
             
            return;
        }
    }
    std::cout << "[EROARE] Nu s-a gasit voluntarul cu ID " << id_cautat << ".\n";
}
void Biblioteca::incarcaAutori() { /* Implementare incarcare autori din fisier daca e cazul */ }