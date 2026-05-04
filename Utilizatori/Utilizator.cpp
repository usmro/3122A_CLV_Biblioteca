#include "Utilizator.h"
#include<iostream>

std::string cripteazaParola(std::string pass) {
    for(size_t i = 0; i < pass.length(); i++) 
        pass[i] += 2; 
    return pass;
}


Utilizator::Utilizator(int id, std::string n, std::string p, std::string e, std::string tel, 
                       std::string d_n, std::string g, std::string pass, std::string d_i, std::string poza) 
    : id_utilizator(id), nume(n), prenume(p), email(e), telefon(tel), 
      data_nasterii(d_n), gen(g), data_inscriere(d_i), status("Activ"), cale_poza(poza)
{
    parola_criptata = cripteazaParola(pass); 
}
void Utilizator::afiseazaTip() {
    std::cout << "Acesta este un utilizator generic.\n";
}







