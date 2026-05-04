#include "Cladire.h"
#include "Autor/Autor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

Cladire::Cladire(std::string nume, std::string adr, int angajati, int etaje, int sali, 
                 int mese, int scaune, int cap_max, bool central, std::string deschidere, std::string inchidere)
    : nume_filiala(nume), 
      adresa(adr), 
      numar_total_angajati(angajati), 
      numar_etaje(etaje),      
      numar_sali(sali),        
      numar_mese(mese), 
      numar_scaune(scaune),
      capacitate_maxima_carti(cap_max), 
      este_sediu_central(central), 
      ora_deschidere(deschidere), 
      ora_inchidere(inchidere)
{
    numar_carti_actual = 0; 
    angajati_per_etaj = (etaje > 0) ? angajati / etaje : angajati;

    std::vector<std::string> zile = {"Luni", "Marti", "Miercuri", "Joi", "Vineri"};
    for(const auto& zi : zile) {
        program_saptamanal[zi] = ora_deschidere + " - " + ora_inchidere;
    }
    program_saptamanal["Sambata"] = "09:00 - 14:00";
    program_saptamanal["Duminica"] = "Inchis";
}
void Cladire::afiseazaRaportResurse() {
    std::cout << "\n--- RAPORT RESURSE LOGISTICE: " << nume_filiala << " ---\n";
    std::cout << "Scaune: " << numar_scaune << " | Mese: " << numar_mese << "\n";
    std::cout << "Angajati: " << numar_total_angajati << " (aprox. " << angajati_per_etaj << "/etaj)\n";
    std::cout << "Capacitate RAMASA: " << (capacitate_maxima_carti - numar_carti_actual) << " unitati.\n";
    std::cout << "----------------------------------------------\n";
}
bool Cladire::verificaSpatiuDisponibil(int carti_noi) {
    return (numar_carti_actual + carti_noi) <= capacitate_maxima_carti;
}
void Cladire::afiseazaProgram() {
    std::cout << "\n--- ORAR FUNCTIONARE: " << nume_filiala << " ---\n";
    
    if (program_saptamanal.empty()) {
        std::cout << "Programul nu a fost inca setat pentru aceasta filiala.\n";
        return;
    }

    for (auto const& [zi, interval] : program_saptamanal) {
        
        std::cout << std::left << std::setw(10) << zi << ": " << interval << "\n";
    }
    std::cout << "------------------------------------------\n";
}

void Cladire::adaugaSala(std::string nume, std::string tip, int etaj, int locuri) {
    inventar_sali.push_back({nume, tip, etaj, locuri});
}

void Cladire::afiseazaDetalii() {
    std::cout << "\n===========================================\n";
    std::cout << "FILIALA: " << nume_filiala;
    if (este_sediu_central) std::cout << " [SEDIU CENTRAL ADMINISTRATIV]";
    std::cout << "\nADRESA: " << adresa << "\n";
    std::cout << "CONTACT: " << numar_contact << " | " << email_contact << "\n";
    std::cout << "-------------------------------------------\n";

    std::cout << "DOTARI SI SALI (" << inventar_sali.size() << " locatii):\n";
    if (inventar_sali.empty()) {
        std::cout << "   - Informatii despre sali indisponibile.\n";
    } else {
        for (const auto& sala : inventar_sali) {
            std::cout << "   * " << sala.nume << " [" << sala.tip << "] - Etaj " 
                      << sala.etaj << " (" << sala.capacitate_locuri << " locuri)\n";
        }
    }

    std::cout << "\nSTATISTICI RESURSE:\n";
    std::cout << "   - Capacitate stocare: " << capacitate_maxima_carti << " carti\n";
    std::cout << "   - Mobilier: " << numar_mese << " mese, " << numar_scaune << " scaune\n";
    std::cout << "   - Personal: " << numar_total_angajati << " angajati disponibili\n";

    std::cout << "-------------------------------------------\n";
    afiseazaProgram(); 
    std::cout << "===========================================\n";
}

bool Cladire::sePoateOrganizaEveniment(int participanti, int staff_necesar, std::string ora_start) {
    if (staff_necesar > numar_total_angajati) {
        std::cout << "[EROARE] Nu avem destui angajati (" << numar_total_angajati 
                  << ") pentru acest eveniment (necesar: " << staff_necesar << ")\n";
        return false;
    }

    if (participanti > numar_scaune) {
        std::cout << "[AVERTISMENT] Numarul de participanti depaseste numarul de scaune!\n";
    }

    if (ora_start > ora_inchidere || ora_start < ora_deschidere) {
        std::cout << "[EROARE] Evenimentul este in afara programului (" 
                  << ora_deschidere << "-" << ora_inchidere << ")\n";
        return false;
    }

    return true;
}

void Cladire::adaugaEveniment(Eveniment ev) {
    if (sePoateOrganizaEveniment(ev.nr_participanti_estimat, ev.angajati_necesari, ev.ora_inceput)) {
        lista_evenimente.push_back(ev);
        std::cout << "[LOGISTICA] Eveniment '" << ev.nume << "' programat cu succes.\n";
    }
}

void Cladire::afiseazaEvenimenteViitoare() {
    std::cout << "\n--- EVENIMENTE PROGRAMATE IN " << nume_filiala << " ---\n";
    if (lista_evenimente.empty()) {
        std::cout << "Nu exista evenimente programate momentan.\n";
        return;
    }

    for (const auto& ev : lista_evenimente) {
        if (ev.este_sedinta) {
            std::cout << "[SEDINTA] >> " << ev.nume << " <<\n";
        } else {
            std::cout << ">> " << ev.nume << " <<\n";
        }
        if (ev.autor_invitat != nullptr) {
            std::cout << "   INVITAT SPECIAL: " << ev.autor_invitat->nume_complet << "\n";
        }
        std::cout << "   Data: " << ev.data << " | Ora: " << ev.ora_inceput << "\n";
        std::cout << "   Locatie: " << ev.sala << " | Participanti: " << ev.nr_participanti_estimat << "\n";
        
        std::cout << "   Descriere: " << ev.descriere << "\n";
        
        std::cout << "   Staff alocat: " << ev.angajati_necesari << " persoane\n";
        std::cout << "------------------------------------------\n";
    } 
}
void Cladire::adaugaCarteInDepozit(Carte* c) {
        if (c != nullptr && numar_carti_actual < capacitate_maxima_carti) {
            colectie_carti.push_back(c);
            numar_carti_actual++;
        }
    }
int Cladire::angajatiLiberi(std::string data, std::string ora) {
    int angajati_ocupati = 0;
    angajati_ocupati += numar_etaje;

    for (const auto& ev : lista_evenimente) {
        if (ev.data == data && ev.ora_inceput == ora) {
            angajati_ocupati += ev.angajati_necesari;
        }
    }

    int liberi = numar_total_angajati - angajati_ocupati;
    return (liberi > 0) ? liberi : 0;
}
