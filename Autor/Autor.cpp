#include "Autor.h"
#include "../Carte/Carte.h" 
#include <iostream>

Autor::Autor(const std::string& nume, const std::string& bio, const std::string& poza) 
    : nume_complet(nume), biografie(bio), cale_poza(poza) {}

void Autor::afiseazaProfilComplet() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "PAGINA AUTOR: " << nume_complet << std::endl;
    std::cout << "FOTO: " << (cale_poza.empty() ? "Lipsa imagine" : cale_poza) << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "BIOGRAFIE: " << (biografie.empty() ? "Nu exista detalii." : biografie) << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "CARTI IN BIBLIOTECA:" << std::endl;
    
    if (carti_in_biblioteca.empty()) {
        std::cout << "- Nu am gasit carti inregistrate pentru acest autor." << std::endl;
    } else {
        for (Carte* c : carti_in_biblioteca) {
            if (c != nullptr) {
                std::cout << "- " << c->titlu << " (" << c->an_aparitie << ")" << std::endl;
            }
        }
    }
    std::cout << "========================================\n" << std::endl;
}

void Autor::adaugaCarteInLista(Carte* c) {
    if (c != nullptr) {
        carti_in_biblioteca.push_back(c);
        c->autor_ref = this;
    }
}