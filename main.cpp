#include "Biblioteca/Biblioteca.h"
#include "Utilizatori/Angajati/Angajat.h"
#include "Utilizatori/Voluntari/Voluntar.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

void afiseazaMeniu() {
    std::cout << "\n" << std::setfill('=') << std::setw(50) << "" << "\n";
    std::cout << "     SISTEM INTEGRAT BIBLIOTECA - CORPURI A-F\n";
    std::cout << std::setfill('=') << std::setw(50) << "" << "\n";
    std::cout << " 1. Harta Filiale (Corpuri, Sali, Facilitati)\n";
    std::cout << " 2. Catalog Complet (Primele 10 unitati)\n";
    std::cout << " 3. Cautare Carte (Titlu/Locatie Raft)\n";
    std::cout << " 4. Profil Autor (Biografie & Bibliografie)\n";
    std::cout << " 5. Filtrare Rating (Vezi cele mai apreciate carti)\n";
    std::cout << " 6. Raport Resurse (Logistica per Filiala)\n";
    std::cout << "\n --- MODUL SOCIAL & CLIENTI ---\n";
    std::cout << " 7. Profil Utilizator (Status & Goodreads)\n";
    std::cout << " 8. Gestiune Lectura (Adaugare in Wishlist)\n";
    std::cout << "\n --- MODUL RESURSE UMANE (ADMIN) ---\n";
    std::cout << " 9. Statstatistici Personal (Salarizare & Roluri)\n";
    std::cout << " 10. Stergere definitiva (Client/Angajat)\n";
    std::cout << " 0. Iesire (Salvare automata date)\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "Alegerea ta: ";
}

int main() {
    std::cout << "Initializare sistem... Incarcare baze de date CSV/TXT.\n";
    
    Biblioteca bibl; 

    int optiune;
    do {
        afiseazaMeniu();
        
        if (!(std::cin >> optiune)) {
            std::cout << "[EROARE] Introdu o cifra valida (0-10)!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        switch (optiune) {
            case 1: {
                std::cout << "\n=== CONFIGURATIE FILIALE SI CORPURI ===\n";
                for (auto& corp : bibl.lista_filiale) {
                    corp.afiseazaDetalii();
                }
                break;
            }

            case 2: {
                std::cout << "\n=== CATALOG UNITATI DISPONIBILE (PAGINA 1) ===\n";
                int limita = (bibl.inventar_general.size() > 10) ? 10 : bibl.inventar_general.size();
                for (int i = 0; i < limita; ++i) {
                    bibl.inventar_general[i]->afiseazaFisaCatalog();
                }
                if (bibl.inventar_general.size() > 10) {
                    std::cout << "... si inca " << bibl.inventar_general.size() - 10 << " unitati in arhiva.\n";
                }
                break;
            }

            case 3: {
                std::string titlu;
                std::cout << "Introdu titlul cautat: ";
                std::getline(std::cin, titlu);
                
                auto rezultate = bibl.cautaDupaTitlu(titlu);
                if (rezultate.empty()) {
                    std::cout << "Nu am gasit niciun exemplar pentru: " << titlu << "\n";
                } else {
                    for (auto c : rezultate) {
                        std::cout << "-------------------------------------------\n";
                        std::cout << "TITLU: " << c->titlu << " (" << c->an_aparitie << ")\n";
                        std::cout << "LOCATIE: " << c->adresa_raft.nume_filiala 
                                  << " | Etaj: " << c->adresa_raft.etaj 
                                  << " | Raft: " << c->adresa_raft.raft << "\n";
                    }
                }
                break;
            }

            case 4: {
                std::string autor;
                std::cout << "Nume Autor: ";
                std::getline(std::cin, autor);
                bibl.cautaInfoAutor(autor);
                break;
            }

            case 5: {
                float rMin;
                std::cout << "Rating minim (1.0 - 5.0): ";
                if(std::cin >> rMin) {
                    bibl.filtreazaDupaRating(rMin);
                }
                break;
            }

            case 6: {
                std::cout << "\n=== RAPORT RESURSE LOGISTICE ===\n";
                for (auto& corp : bibl.lista_filiale) {
                    corp.afiseazaRaportResurse();
                }
                break;
            }

            case 7: {
                int idU;
                std::cout << "ID Utilizator: ";
                std::cin >> idU;
                bool gasit = false;
                for(auto u : bibl.lista_utilizatori) {
                    if(u->id_utilizator == idU) {
                        u->afiseazaTip();
                        gasit = true;
                        break;
                    }
                }
                if(!gasit) std::cout << "Utilizatorul nu a fost gasit in baza de date.\n";
                break;
            }

            case 9: {
                std::cout << "\n=== STAT DE PLATA SI ACCES PERSONAL ===\n";
                for (auto a : bibl.lista_angajati) {
                    std::cout << "ANGAJAT: " << a->nume << " " << a->prenume << "\n";
                    std::cout << "SALARIU TOTAL: " << a->calculeazaVenitTotal() << " RON\n";
                    a->afiseazaInterfataSpecifica();
                    std::cout << "-------------------------------------------\n";
                }
                break;
            }

            case 10: {
                int idS; char tip;
                std::cout << "Stergere (A)ngajat / (V)oluntar / (C)lient: ";
                std::cin >> tip;
                std::cout << "ID pentru eliminare definitiva: ";
                std::cin >> idS;
                
                if (tip == 'A' || tip == 'a') bibl.stergeAngajat(idS);
                else if (tip == 'V' || tip == 'v') bibl.stergeVoluntar(idS);
                else bibl.stergeClient(idS);
                break;
            }

            case 0:
                std::cout << "Inchidere securizata. Toate modificarile au fost salvate in CSV.\n";
                break;

            default:
                std::cout << "Optiune invalida.\n";
        }
    } while (optiune != 0);

    return 0;
}