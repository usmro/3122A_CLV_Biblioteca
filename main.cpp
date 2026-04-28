#include "Biblioteca.h"
#include <iostream>
#include <string>
#include <iomanip>

void afiseazaMeniu() {
    std::cout << "\n" << std::setfill('=') << std::setw(45) << "" << "\n";
    std::cout << "   SISTEM DE GESTIUNE BIBLIOTECA - CORPURI A-F\n";
    std::cout << std::setfill('=') << std::setw(45) << "" << "\n";
    std::cout << "1. Afiseaza Harta Filiale (Corpuri, Sali, Resurse)\n";
    std::cout << "2. Afiseaza Tot Inventarul (Paginat - primele 10)\n";
    std::cout << "3. Cauta Carte dupa Titlu (Vezi Locatie Raft)\n";
    std::cout << "4. Cauta Info Autor (Biografie & Carti scrise)\n";
    std::cout << "5. Filtrare Avansata (Rating Minim)\n";
    std::cout << "6. Administrare: Situatie Resurse per Corp\n";
    std::cout << "0. Iesire\n";
    std::cout << "Alegerea ta: ";
}

int main() {
    std::cout << "Se incarca baza de date... Va rugam asteptati.\n";
    Biblioteca bibl; 

    int optiune;
    do {
        afiseazaMeniu();
        if (!(std::cin >> optiune)) {
            std::cout << "Te rog introdu o cifra valida!\n";
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        std::cin.ignore(); 

        switch (optiune) {
            case 1:
                std::cout << "\n--- CONFIGURATIE LOGISTICA BIBLIOTECA ---\n";
                for (auto& corp : bibl.lista_filiale) {
                    corp.afiseazaDetalii();
                }
                break;

            case 2:
                std::cout << "\n--- PRIMELES 10 CARTI DIN INVENTAR ---\n";
                for (size_t i = 0; i < 10 && i < bibl.inventar_general.size(); ++i) {
                    bibl.inventar_general[i]->afiseazaFisaCatalog();
                }
                std::cout << "\n... si inca " << bibl.inventar_general.size() - 10 << " carti in baza de date.\n";
                break;

            case 3: {
                std::string titlu;
                std::cout << "Introdu titlul (sau o parte din el): ";
                std::getline(std::cin, titlu);
                
                auto rezultate = bibl.cautaDupaTitlu(titlu);
                if (rezultate.empty()) {
                    std::cout << "Nu am gasit nicio carte cu acest titlu.\n";
                } else {
                    std::cout << "\nAm gasit " << rezultate.size() << " rezultate:\n";
                    for (auto c : rezultate) {
                        std::cout << "-------------------------------------------\n";
                        std::cout << "TITLU: " << c->titlu << "\n";
                        std::cout << "LOCATIE: " << c->adresa_raft.nume_filiala 
                                  << " | Etaj: " << c->adresa_raft.etaj 
                                  << " | Raft: " << c->adresa_raft.raft << "\n";
                    }
                }
                break;
            }

            case 4: {
                std::string autor;
                std::cout << "Introdu numele autorului: ";
                std::getline(std::cin, autor);
                bibl.cautaInfoAutor(autor);
                break;
            }

            case 5: {
                float rMin;
                std::cout << "Afiseaza carti cu rating mai mare de (ex: 4.5): ";
                std::cin >> rMin;
                bibl.filtreazaDupaRating(rMin);
                break;
            }

            case 6:
                std::cout << "\n--- RAPORT RESURSE UMANE SI LOGISTICE ---\n";
                for (auto& corp : bibl.lista_filiale) {
                    corp.afiseazaRaportResurse();
                }
                break;

            case 0:
                std::cout << "Salvare date si inchidere sistem. O zi buna!\n";
                break;

            default:
                std::cout << "Optiune inexistenta. Incearca din nou.\n";
        }
    } while (optiune != 0);

    return 0;
}