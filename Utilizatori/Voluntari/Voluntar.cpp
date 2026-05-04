#include "Voluntar.h"
#include <iostream>

Voluntar::Voluntar(int id, std::string n, std::string p, std::string e, std::string tel,
                   std::string d_n, std::string g, std::string pass, std::string poza,
                   std::string c_contract, std::string d_start, std::string d_end,
                   std::string l_idei, std::string l_inscriere, std::string l_prog, 
                   std::string l_pers, int ore)
    : Client(id, n,  p, e, tel, d_n,   g,  pass,  n + "_" + p, 18,    d_n,   "Novice",poza ), 
      cale_contract(c_contract), 
      data_start_contract(d_start), 
      data_end_contract(d_end),
      ore_lucrate(ore), 
      link_idei_eveniment(l_idei), 
      link_inscriere_org(l_inscriere), 
      link_program_colegi(l_prog),
      link_program_personal(l_pers) 
{
    this->status = "Offline";
    this->rating_mediu = 5.0;
}


void Voluntar::adaugaNotificare(const std::string& mesaj) {
    notificari_sedinte.push_back(mesaj);
}

void Voluntar::afiseazaCalendar() {
    std::cout << "--- Calendarul lui " << prenume << " ---\n";
    for(const auto& ev : calendar) {
        std::cout << "[ ] " << ev.data << " " << ev.ora_inceput << ": " << ev.nume << "\n";
    }
}

void Voluntar::adaugaOre(int nr_ore) {
    if (nr_ore > 0 && nr_ore < 24) { 
        this->ore_lucrate += nr_ore;
        std::cout << "[LOG] S-au adaugat " << nr_ore << " ore. Total: " << this->ore_lucrate << "\n";
    }
}

void Voluntar::primesteAnuntSedinta(const std::string& data, const std::string& titlu) {
    std::string anunt_complet = data + " - " + titlu;
    notificari_sedinte.push_back(anunt_complet);
    
    Eveniment sed;
    sed.nume = titlu + " (Sedinta)";
    sed.data = data;
    sed.ora_inceput = "10:00";
    sed.durata_minute = 60;
    sed.este_sedinta = true;
    calendar.push_back(sed);
}

void Voluntar::adaugaEveniment(std::string data, std::string ora, std::string titlu, std::string desc, bool sedinta) {
    Eveniment nou;
    nou.nume = titlu;
    nou.data = data;
    nou.ora_inceput = ora;
    nou.durata_minute = 60; 
    nou.descriere = desc;   
    nou.este_sedinta = sedinta;
    calendar.push_back(nou);
}

bool Voluntar::stergeEveniment(std::string titlu) {
    for (auto it = calendar.begin(); it != calendar.end(); ++it) {
        if (it->nume == titlu) {
            calendar.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<Eveniment> Voluntar::getEvenimentePeData(std::string data_cautata) {
    std::vector<Eveniment> rezultate;
    for (const auto& ev : calendar) {
        if (ev.data == data_cautata) {
            rezultate.push_back(ev);
        }
    }
    return rezultate;
}

void Voluntar::primesteFeedback(std::string client, std::string data, int nota, std::string obs) {
    Feedback f = {client, data, nota, obs};
    lista_feedback.push_back(f);
    double suma = 0;
    for(const auto& r : lista_feedback) suma += r.stelute;
    rating_mediu = suma / lista_feedback.size();
}

void Voluntar::afiseazaFeedback() const {
    std::cout << "--- Feedback pentru " << prenume << " (Rating: " << rating_mediu << "/5) ---\n";
    for(const auto& f : lista_feedback) {
        std::cout << "[" << f.data << "] " << f.nume_client << ": " << f.stelute << " stele\n";
    }
}

void Voluntar::setStatus(std::string noulStatus) {
    this->status = noulStatus;
}

std::string Voluntar::exportaInCSV() const {
    std::stringstream ss;
    ss << Client::exportaInCSV() << ";" 
       << cale_contract << ";" << data_start_contract << ";" << data_end_contract << ";"
       << ore_lucrate << ";" << rating_mediu;
    return ss.str();
}