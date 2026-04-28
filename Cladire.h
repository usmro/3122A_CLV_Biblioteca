#ifndef CLADIRE_H
#define CLADIRE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>

struct Eveniment {
    std::string nume;
    std::string data;
    std::string ora_inceput;           
    int durata_minute;
    std::string sala;
    int nr_participanti_estimat;
    int angajati_necesari;
};
struct DetaliiSala {
    std::string nume;
    std::string tip; 
    int etaj;
    int capacitate_locuri;
};
struct Facilitati {
    int nr_bai;
    bool are_lift;
    bool acces_dizabilitati;
};
class Cladire {
public:
    std::string nume_filiala;
    std::map<std::string, std::string> program_saptamanal;
    std::string adresa;
    std::string numar_contact; 
    std::string email_contact; 

    int numar_total_angajati;
    int angajati_per_etaj;

    int numar_sali;  
    int numar_etaje;     
    int numar_mese;
    int numar_scaune;          
    int capacitate_maxima_carti;  
    int numar_carti_actual;  
    bool este_sediu_central; 

    std::string ora_deschidere; 
    std::string ora_inchidere;

    std::vector<Eveniment> lista_evenimente;
    std::vector<DetaliiSala> inventar_sali;
    Facilitati facilitati_cladire;
   
    Cladire(std::string nume, std::string adr, int angajati, int etaje, int sali, 
            int mese, int scaune, int cap_max, bool central, std::string deschidere, std::string inchidere);
    void afiseazaDetalii();
    void afiseazaProgram ();   
    void adaugaSala(std::string nume, std::string tip, int etaj, int locuri);
    void adaugaEveniment(Eveniment ev);
    void afiseazaRaportResurse(); 
    void afiseazaEvenimenteViitoare();
    bool verificaCapacitateEveniment(int nr_oaspeti);
    bool sePoateOrganizaEveniment(int participanti, int staff_necesar, std::string ora_start);
    bool verificaSpatiuDisponibil(int carti_existente);
    int angajatiLiberi(std::string data, std::string ora);
};

#endif