#include <iostream>
#include "Biblioteca.h"

int main() {
    
    Biblioteca a;

    CarteFictiune c1(1, " Doua coroane inlantuite ", " Rachel Gilling ", "Storia Books", "978-630-6525-52-2", "Ro", "2024", 502, "Tineri","Donatie","Disponibila","Elspeth Spindle","Regele Pastor","Persoana III", 17 );
    CarteNonFictiune c2(2, "Dictionar roman-francez francez-roman", "Ionel V. Anton", "Poseidon", "978-606-93117-9-0","Ro-Fr","2012", 575, "Studenti/elevi","Achizitie directa", "Disponibila","Limbi straine", "-", "-");
    
    Utilizator u1(97,"Carp","Larisa","larisa.carp@gmail.com","0756345892","18-06-2005","F","Student","2021");

    a.adaugaCarte(c1);
    a.adaugaCarte(c2);
    a.inregistreazaUtilizator(u1);
    a.realizeazaImprumut(1, 97);
    return 0;
}