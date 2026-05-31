#ifndef VOLUNTAR_H
#define VOLUNTAR_H

#include "../Clienti/Client.h" 
#include "../../Cladiri/Cladire.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>

class Carte;
struct Feedback {
    std::string nume_client;
    std::string data;
    int stelute;             
    std::string comentariu;
};

class Voluntar : public Client {
protected:
    // --- Date Administrative ---
    std::string cale_contract;          
    std::string data_start_contract;
    std::string data_end_contract;

    // --- Management Activitate ---
    int ore_lucrate;
    std::string link_idei_eveniment;    
    std::string link_inscriere_org;     
    std::string link_program_colegi;
    std::string link_program_personal;
    std::vector<std::string> notificari_sedinte;
    std::vector<Eveniment> calendar;
  
    // ---Feedback---
    std::vector<Feedback> lista_feedback;
    double rating_mediu;
    std::string observatii_activitate;          // note lasate de coordonator
    std::map<std::string, std::string> program_saptamanal_personal;

    public:
    Voluntar(int id, std::string n, std::string p, std::string e, std::string tel,
         std::string d_n, std::string g, std::string pass, std::string poza,
         std::string c_contract, std::string d_start, std::string d_end,
         std::string l_idei, std::string l_inscriere, std::string l_prog, 
         std::string l_pers, int ore);

    virtual ~Voluntar() {}

    void adaugaNotificare(const std::string& mesaj);
    void afiseazaCalendar();
    void adaugaOre(int nr_ore);
    int getOre() const { return ore_lucrate; }
     // Program personal
    void seteazaZiLucru(std::string zi, std::string interval);
    void afiseazaProgramPersonal() const;

    // Observatii
    void adaugaObservatie(const std::string& obs);
    void afiseazaObservatii() const;

    void primesteAnuntSedinta(const std::string& data, const std::string& titlu);
    std::string getLinkIdei() const { return link_idei_eveniment; }
    std::string getLinkInscriere() const { return link_inscriere_org; }

    void adaugaEveniment(std::string data, std::string ora, std::string titlu, std::string desc, bool sedinta);
    bool stergeEveniment(std::string titlu);
    std::vector<Eveniment> getEvenimentePeData(std::string data_cautata);
    
    void primesteFeedback(std::string client, std::string data, int nota, std::string obs);
    void afiseazaFeedback() const;
    double getRating() const { return rating_mediu; }

    void setStatus(std::string noulStatus);
    std::string getStatus() const { return status; }

    std::string exportaInCSV() const override;
};  

#endif