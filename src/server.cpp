#include "../lib/httplib.h"
#include "../lib/json.hpp"
#include "../src/Database/Database.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
using json = nlohmann::json;

// ── Helper: citeste fisier static ────────────────────────────
std::string citesteFisier(const std::string& cale) {
    std::ifstream f(cale, std::ios::binary);
    if (!f.is_open()) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// ── Helper: content type dupa extensie ───────────────────────
std::string contentType(const std::string& cale) {
    auto endsWith = [](const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() &&
               s.substr(s.size() - suffix.size()) == suffix;
    };
    if (endsWith(cale, ".html")) return "text/html; charset=utf-8";
    if (endsWith(cale, ".css"))  return "text/css";
    if (endsWith(cale, ".js"))   return "application/javascript";
    if (endsWith(cale, ".png"))  return "image/png";
    if (endsWith(cale, ".jpg") || endsWith(cale, ".jpeg")) return "image/jpeg";
    if (endsWith(cale, ".svg"))  return "image/svg+xml";
    return "text/plain";
}

// ── Helper: parola simpla criptare (aceeasi din Utilizator.cpp) 
std::string cripteaza(std::string pass) {
    for (size_t i = 0; i < pass.length(); i++) pass[i] += 2;
    return pass;
}

int main() {
    Database db("biblioteca.db");
    if (!db.esteDeschisDB()) {
        std::cerr << "[SERVER] Nu pot deschide baza de date!\n";
        return 1;
    }

    httplib::Server svr;

    // ── CORS headers pentru toate raspunsurile ────────────────
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // ════════════════════════════════════════════════════════
    // FISIERE STATICE — serveste folderul web/
    // ════════════════════════════════════════════════════════
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::string continut = citesteFisier("web/primapagina.html");
        if (continut.empty()) continut = "<h1>Biblioteca Aster</h1>";
        res.set_content(continut, "text/html; charset=utf-8");
    });

    svr.Get(R"(/web/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string cale = "web/" + req.matches[1].str();
        std::string continut = citesteFisier(cale);
        if (continut.empty()) {
            res.status = 404;
            res.set_content("404 Not Found", "text/plain");
            return;
        }
        res.set_content(continut, "text/html; charset=utf-8");
    });

    // ════════════════════════════════════════════════════════
    // API — AUTENTIFICARE
    // ════════════════════════════════════════════════════════

    // POST /api/login — autentificare client
    svr.Post("/api/login", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            std::string email = body["email"];
            std::string parola = body["parola"];
            std::string parola_criptata = cripteaza(parola);

            auto clienti = db.getToatiClientii();
            for (const auto& c : clienti) {
                if (c.email == email && c.parola_criptata == parola_criptata) {
                    json raspuns = {
                        {"succes", true},
                        {"id", c.id},
                        {"nume", c.nume},
                        {"prenume", c.prenume},
                        {"username", c.username},
                        {"rank", c.rank},
                        {"total_imprumuturi", c.total_imprumuturi}
                    };
                    res.set_content(raspuns.dump(), "application/json");
                    return;
                }
            }
            json err = {{"succes", false}, {"mesaj", "Email sau parolă incorectă."}};
            res.set_content(err.dump(), "application/json");
        } catch (...) {
            json err = {{"succes", false}, {"mesaj", "Eroare server."}};
            res.status = 500;
            res.set_content(err.dump(), "application/json");
        }
    
    });

    // POST /api/login-profesional — parola resursa angajat/voluntar
    svr.Post("/api/login-profesional", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            std::string tip = body["tip"];
            int id_client = body.value("id_client", -1);
            if (tip == "angajat") {
                std::string parola = body.value("parola_resursa", "");
                auto angajati = db.getToatiAngajatii();
                for (const auto& a : angajati) {
                    if (a.id == id_client && a.parola_resursa == cripteaza(parola)) {
                        json r = {{"succes",true},{"tip","angajat"},{"tip_angajat",a.tip_angajat},{"nume",a.nume},{"prenume",a.prenume},{"email",a.email}};
                        res.set_content(r.dump(), "application/json");
                        return;
                    }
                }
                json err = {{"succes",false},{"mesaj","Parola profesionala incorecta."}};
                res.set_content(err.dump(), "application/json");
            } else {
                auto voluntari = db.getToatiVoluntarii();
                for (const auto& v : voluntari) {
                    if (v.id == id_client) {
                        json r = {{"succes",true},{"tip","voluntar"},{"nume",v.nume},{"prenume",v.prenume},{"email",v.email}};
                        res.set_content(r.dump(), "application/json");
                        return;
                    }
                }
                json err = {{"succes",false},{"mesaj","Nu esti inregistrat ca voluntar."}};
                res.set_content(err.dump(), "application/json");
            }
        } catch (...) {
            json err = {{"succes",false},{"mesaj","Eroare server."}};
            res.status = 500;
            res.set_content(err.dump(), "application/json");
        }
    });
    // POST /api/inregistrare — cont client nou
    svr.Post("/api/inregistrare", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            ClientDB c;
            c.nume       = body["nume"];
            c.prenume    = body["prenume"];
            c.username   = body["username"];
            c.email      = body["email"];
            c.parola_criptata = cripteaza(body["parola"]);
            c.varsta     = body.value("varsta", 18);
            c.rank       = "Novice";
            c.data_inscriere = "2026-01-01";
            c.data_inscriere_client = "2026-01-01";
            c.total_imprumuturi = 0;
            c.status     = "Activ";
            c.cale_poza  = "";

            if (db.adaugaClient(c)) {
                json r = {{"succes", true}};
                res.set_content(r.dump(), "application/json");
            } else {
                json err = {{"succes", false}, {"mesaj", "Emailul este deja folosit."}};
                res.set_content(err.dump(), "application/json");
            }
        } catch (...) {
            json err = {{"succes", false}, {"mesaj", "Eroare server."}};
            res.status = 500;
            res.set_content(err.dump(), "application/json");
        }
    });

    // ════════════════════════════════════════════════════════
    // API — CARTI
    // ════════════════════════════════════════════════════════

    // GET /api/carti — toate cartile (primele 50)
    svr.Get("/api/carti", [&db](const httplib::Request&, httplib::Response& res) {
        auto carti = db.getToateCartile();
        json arr = json::array();
        int limita = std::min((int)carti.size(), 50);
        for (int i = 0; i < limita; i++) {
            const auto& c = carti[i];
            arr.push_back({
                {"id", c.id}, {"titlu", c.titlu}, {"autor", c.autor},
                {"editura", c.editura}, {"limba", c.limba},
                {"an_aparitie", c.an_aparitie}, {"nr_pagini", c.nr_pagini},
                {"tip", c.tip}, {"suport", c.suport}, {"rating", c.rating},
                {"exemplare_disponibile", c.exemplare_disponibile},
                {"este_patrimoniu", c.este_patrimoniu},
                {"rezumat", c.rezumat}, {"cale_poza", c.cale_poza},
                {"id_cladire", c.id_cladire}, {"id_sala", c.id_sala}, {"id_raft", c.id_raft},
                {"serie_contabila", c.serie_contabila}, {"cod_identificare", c.cod_identificare},
                {"gen_specific", c.gen_specific}, {"domeniu", c.domeniu},
                {"isbn", c.isbn}, {"issn", c.issn},
                {"tip_coperta", c.tip_coperta}, {"serie", c.serie},
                {"varsta_recomandata", c.varsta_recomandata},
                {"subcategorie", c.subcategorie}, {"frecventa", c.frecventa},
                {"numar_editie", c.numar_editie}, {"pret_intrare", c.pret_intrare},
                {"tara_provenienta", c.tara_provenienta}
            });
        }
        res.set_content(arr.dump(), "application/json");
    });

    // GET /api/carti/cauta?q=titlu — cautare carti
    svr.Get("/api/carti/cauta", [&db](const httplib::Request& req, httplib::Response& res) {
        std::string q = req.has_param("q") ? req.get_param_value("q") : "";
        std::string tip = req.has_param("tip") ? req.get_param_value("tip") : "titlu";

        std::vector<CarteDB> carti;
        if (tip == "autor") carti = db.cautaDupaAutor(q);
        else if (q.empty()) carti = db.getToateCartile();
        else carti = db.cautaDupaTitlu(q);

        // Filtrare suplimentara
        std::string gen = req.has_param("gen") ? req.get_param_value("gen") : "";
        std::string suport = req.has_param("suport") ? req.get_param_value("suport") : "";
        std::string rating_str = req.has_param("rating") ? req.get_param_value("rating") : "";
        std::string an_min_str = req.has_param("an_min") ? req.get_param_value("an_min") : "";
        std::string an_max_str = req.has_param("an_max") ? req.get_param_value("an_max") : "";

        if (!gen.empty() || !suport.empty() || !rating_str.empty() || !an_min_str.empty() || !an_max_str.empty()) {
            float rating_min = rating_str.empty() ? 0.0f : std::stof(rating_str);
            int an_min = an_min_str.empty() ? 0 : std::stoi(an_min_str);
            int an_max = an_max_str.empty() ? 9999 : std::stoi(an_max_str);
            carti.erase(std::remove_if(carti.begin(), carti.end(), [&](const CarteDB& c) {
                if (!gen.empty() && c.gen_specific != gen) return true;
                if (!suport.empty() && c.suport != suport) return true;
                if (rating_min > 0 && c.rating < rating_min) return true;
                if (an_min > 0 && c.an_aparitie < an_min) return true;
                if (an_max < 9999 && c.an_aparitie > an_max) return true;
                return false;
            }), carti.end());
        }
        json arr = json::array();
        for (const auto& c : carti) {
           arr.push_back({
                {"id", c.id}, {"titlu", c.titlu}, {"autor", c.autor},
                {"editura", c.editura}, {"limba", c.limba},
                {"an_aparitie", c.an_aparitie}, {"nr_pagini", c.nr_pagini},
                {"tip", c.tip}, {"suport", c.suport}, {"rating", c.rating},
                {"exemplare_disponibile", c.exemplare_disponibile},
                {"este_patrimoniu", c.este_patrimoniu},
                {"rezumat", c.rezumat}, {"cale_poza", c.cale_poza},
                {"id_cladire", c.id_cladire}, {"id_sala", c.id_sala}, {"id_raft", c.id_raft},
                {"serie_contabila", c.serie_contabila}, {"cod_identificare", c.cod_identificare},
                {"gen_specific", c.gen_specific}, {"domeniu", c.domeniu},
                {"isbn", c.isbn}, {"issn", c.issn},
                {"tip_coperta", c.tip_coperta}, {"serie", c.serie},
                {"varsta_recomandata", c.varsta_recomandata},
                {"subcategorie", c.subcategorie}, {"frecventa", c.frecventa},
                {"numar_editie", c.numar_editie}, {"pret_intrare", c.pret_intrare},
                {"tara_provenienta", c.tara_provenienta}
            });
        }
        res.set_content(arr.dump(), "application/json");
    
    });
    // POST /api/carti — adauga carte noua
svr.Post("/api/carti", [&db](const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = json::parse(req.body);
        std::string titlu = body.value("titlu", "");
        std::string autor = body.value("autor", "");
        if (titlu.empty() || autor.empty()) {
            json err = {{"succes", false}, {"mesaj", "Titlul si autorul sunt obligatorii."}};
            res.set_content(err.dump(), "application/json");
            return;
        }
        std::string sql = "INSERT INTO carti (titlu, autor, gen_specific, tip, exemplare_disponibile) VALUES ('" + titlu + "','" + autor + "','" + body.value("gen_specific","") + "','" + body.value("tip","") + "'," + std::to_string(body.value("exemplare_disponibile",1)) + ")";
        sqlite3_exec(db.getDb(), sql.c_str(), nullptr, nullptr, nullptr);
        int id = sqlite3_last_insert_rowid(db.getDb());
        json r = {{"succes", true}, {"id", id}};
        res.set_content(r.dump(), "application/json");
    } catch (...) {
        json err = {{"succes", false}, {"mesaj", "Eroare server."}};
        res.status = 500;
        res.set_content(err.dump(), "application/json");
    }
});

// DELETE /api/carti/:id — sterge carte
    svr.Delete(R"(/api/carti/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            int id = std::stoi(req.matches[1].str());
            std::string sql = "DELETE FROM carti WHERE id = " + std::to_string(id);
            sqlite3_exec(db.getDb(), sql.c_str(), nullptr, nullptr, nullptr);
            json r = {{"succes", true}};
            res.set_content(r.dump(), "application/json");
        } catch (...) {
            json err = {{"succes", false}, {"mesaj", "Eroare server."}};
            res.status = 500;
            res.set_content(err.dump(), "application/json");
        }
    });
    // GET /api/carti/:id — detalii carte
    svr.Get(R"(/api/carti/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1].str());
        CarteDB c = db.getCarteDupaId(id);
        if (c.id == -1) {
            res.status = 404;
            res.set_content("{\"eroare\":\"Carte negasita\"}", "application/json");
            return;
        }
        json r = {
            {"id", c.id}, {"titlu", c.titlu}, {"autor", c.autor},
            {"editura", c.editura}, {"limba", c.limba}, {"tip", c.tip},
            {"suport", c.suport}, {"rating", c.rating}, {"rezumat", c.rezumat},
            {"nr_pagini", c.nr_pagini}, {"an_aparitie", c.an_aparitie},
            {"exemplare_disponibile", c.exemplare_disponibile},
            {"este_patrimoniu", c.este_patrimoniu},
            {"gen_specific", c.gen_specific}, {"domeniu", c.domeniu},
            {"tip_coperta", c.tip_coperta}, {"serie", c.serie},
            {"issn", c.issn}, {"isbn", c.isbn},
            {"id_cladire", c.id_cladire}, {"id_sala", c.id_sala},
            {"id_raft", c.id_raft}, {"cod_identificare", c.cod_identificare}
        };
        res.set_content(r.dump(), "application/json");
    });

    // GET /api/carti/rating?min=4.0 — filtrare dupa rating
    svr.Get("/api/carti/rating", [&db](const httplib::Request& req, httplib::Response& res) {
        float ratingMin = req.has_param("min") ? std::stof(req.get_param_value("min")) : 4.0f;
        auto carti = db.filtreazaDupaRating(ratingMin);
        json arr = json::array();
        for (const auto& c : carti) {
            arr.push_back({
                {"id", c.id}, {"titlu", c.titlu}, {"autor", c.autor},
                {"rating", c.rating}, {"tip", c.tip}, {"suport", c.suport}
            });
        }
        res.set_content(arr.dump(), "application/json");
    });

    // ════════════════════════════════════════════════════════
    // API — AUTORI
    // ════════════════════════════════════════════════════════

    // GET /api/autori — toti autorii
    svr.Get("/api/autori", [&db](const httplib::Request&, httplib::Response& res) {
        auto autori = db.getToatiAutorii();
        json arr = json::array();
        for (const auto& a : autori) {
            arr.push_back({
                {"id", a.id}, {"nume_complet", a.nume_complet},
                {"biografie", a.biografie}, {"cale_poza", a.cale_poza}
            });
        }
        res.set_content(arr.dump(), "application/json");
    });

    // GET /api/autori/cauta?q=nume
    svr.Get("/api/autori/cauta", [&db](const httplib::Request& req, httplib::Response& res) {
        std::string q = req.has_param("q") ? req.get_param_value("q") : "";
        AutorDB a = db.getAutorDupaNumele(q);
        if (a.id == -1) {
            res.set_content("[]", "application/json");
            return;
        }
        // Carti ale autorului
        auto carti = db.cautaDupaAutor(a.nume_complet);
        json carti_arr = json::array();
        for (const auto& c : carti) {
            carti_arr.push_back({{"id", c.id}, {"titlu", c.titlu}, {"an_aparitie", c.an_aparitie}});
        }
        json r = {
            {"id", a.id}, {"nume_complet", a.nume_complet},
            {"biografie", a.biografie}, {"carti", carti_arr}
        };
        res.set_content(r.dump(), "application/json");
    });

    // ════════════════════════════════════════════════════════
    // API — IMPRUMUTURI & REZERVARI
    // ════════════════════════════════════════════════════════

    // POST /api/imprumuturi — imprumuta o carte
    svr.Post("/api/imprumuturi", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int id_client = body["id_client"];
            int id_carte  = body["id_carte"];
            std::string data_azi   = body.value("data_azi", "2026-01-01");
            std::string data_limita = body.value("data_limita", "2026-01-22");

            if (db.adaugaImprumut(id_client, id_carte, data_azi, data_limita)) {
                db.actualizeazaTotalImprumuturi(id_client, 1);
                json r = {{"succes", true}};
                res.set_content(r.dump(), "application/json");
            } else {
                json err = {{"succes", false}, {"mesaj", "Nu s-a putut efectua imprumutul."}};
                res.set_content(err.dump(), "application/json");
            }
        } catch (...) {
            res.status = 500;
            res.set_content("{\"succes\":false}", "application/json");
        }
    });

    // POST /api/imprumuturi/retur — returneaza o carte
    svr.Post("/api/imprumuturi/retur", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int id_carte  = body["id_carte"];
            int id_client = body["id_client"];
            std::string data_retur = body.value("data_retur", "2026-01-01");

            if (db.returneazaCarte(id_carte, id_client, data_retur)) {
                json r = {{"succes", true}};
                res.set_content(r.dump(), "application/json");
            } else {
                json err = {{"succes", false}, {"mesaj", "Nu s-a putut efectua returul."}};
                res.set_content(err.dump(), "application/json");
            }
        } catch (...) {
            res.status = 500;
            res.set_content("{\"succes\":false}", "application/json");
        }
    });

    // GET /api/imprumuturi/:id_client — imprumuturi client
    svr.Get(R"(/api/imprumuturi/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1].str());
        auto imprumuturi = db.getImprumuturiClient(id);
        json arr = json::array();
        for (const auto& imp : imprumuturi) {
            arr.push_back({
                {"id", imp.id}, {"id_carte", imp.id_carte},
                {"data_imprumut", imp.data_imprumut},
                {"data_limita", imp.data_limita},
                {"returnata", imp.returnata}
            });
        }
        res.set_content(arr.dump(), "application/json");
    });

    // POST /api/rezervari — rezerva o carte
    svr.Post("/api/rezervari", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int id_client = body["id_client"];
            int id_carte  = body["id_carte"];
            std::string data_azi = body.value("data_azi", "2026-01-01");
            std::string data_exp = body.value("data_expirare", "2026-01-03");

            if (db.adaugaRezervare(id_client, id_carte, data_azi, data_exp)) {
                json r = {{"succes", true}};
                res.set_content(r.dump(), "application/json");
            } else {
                json err = {{"succes", false}, {"mesaj", "Nu s-a putut rezerva cartea."}};
                res.set_content(err.dump(), "application/json");
            }
        } catch (...) {
            res.status = 500;
            res.set_content("{\"succes\":false}", "application/json");
        }
    });

    // DELETE /api/rezervari — anuleaza rezervare
    svr.Delete("/api/rezervari", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int id_client = body["id_client"];
            int id_carte  = body["id_carte"];
            if (db.anuleazaRezervare(id_client, id_carte)) {
                json r = {{"succes", true}};
                res.set_content(r.dump(), "application/json");
            } else {
                json err = {{"succes", false}};
                res.set_content(err.dump(), "application/json");
            }
        } catch (...) {
            res.status = 500;
            res.set_content("{\"succes\":false}", "application/json");
        }
    });

    // ════════════════════════════════════════════════════════
    // API — CLIENTI
    // ════════════════════════════════════════════════════════

    // GET /api/clienti/:id — profil client
    svr.Get(R"(/api/clienti/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1].str());
        ClientDB c = db.getClientDupaId(id);
        if (c.id == -1) {
            res.status = 404;
            res.set_content("{\"eroare\":\"Client negasit\"}", "application/json");
            return;
        }
        json r = {
            {"id", c.id}, {"nume", c.nume}, {"prenume", c.prenume},
            {"username", c.username}, {"rank", c.rank},
            {"total_imprumuturi", c.total_imprumuturi},
            {"data_inscriere", c.data_inscriere_client}
        };
        res.set_content(r.dump(), "application/json");
    });

    // GET /api/wishlist/:id_client
    svr.Get(R"(/api/wishlist/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1].str());
        auto wishlist = db.getWishlistClient(id);
        json arr = json::array();
        for (int id_carte : wishlist) {
            CarteDB c = db.getCarteDupaId(id_carte);
            if (c.id != -1) {
                arr.push_back({{"id", c.id}, {"titlu", c.titlu}, {"autor", c.autor}});
            }
        }
        res.set_content(arr.dump(), "application/json");
    });

    // POST /api/wishlist — adauga in wishlist
    svr.Post("/api/wishlist", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            bool ok = db.adaugaInWishlist(body["id_client"], body["id_carte"]);
            res.set_content(json{{"succes", ok}}.dump(), "application/json");
        } catch (...) {
            res.status = 500;
            res.set_content("{\"succes\":false}", "application/json");
        }
    });

    // ════════════════════════════════════════════════════════
    // API — CLADIRI
    // ════════════════════════════════════════════════════════
    // POST /api/ai — proxy pentru Anthropic API
    svr.Post("/api/ai", [](const httplib::Request& req, httplib::Response& res) {
        try {
         auto body = json::parse(req.body);
        
        // Construieste cererea catre Anthropic
         httplib::Client cli("api.anthropic.com");
            cli.set_default_headers({
                {"x-api-key", "ANTHROPIC_API_KEY"},
                {"anthropic-version", "2023-06-01"},
                {"content-type", "application/json"}
            });
        
            auto result = cli.Post("/v1/messages", body.dump(), "application/json");
        
            if (result && result->status == 200) {
             res.set_content(result->body, "application/json");
            } else {
                json err = {{"error", "API error"}};
                res.status = 500;
                res.set_content(err.dump(), "application/json");
            }
        } catch (...) {
            json err = {{"error", "Server error"}};
            res.status = 500;
            res.set_content(err.dump(), "application/json");
        }
    });
    // POST /api/idei-evenimente — trimite idee eveniment
svr.Post("/api/idei-evenimente", [&db](const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = json::parse(req.body);
        int id_voluntar = body.value("id_voluntar", -1);
        std::string titlu = body.value("titlu", "");
        std::string tip = body.value("tip", "");
        std::string data_propusa = body.value("data_propusa", "");
        std::string corp = body.value("corp", "");
        std::string descriere = body.value("descriere", "");
        std::string resurse = body.value("resurse", "");
        if (titlu.empty()) {
            json err = {{"succes", false}, {"mesaj", "Titlul este obligatoriu."}};
            res.set_content(err.dump(), "application/json");
            return;
        }
        bool ok = db.adaugaIdeieEveniment(id_voluntar, titlu, tip, data_propusa, corp, descriere, resurse);
        json r = {{"succes", ok}};
        res.set_content(r.dump(), "application/json");
    } catch (...) {
        json err = {{"succes", false}, {"mesaj", "Eroare server."}};
        res.status = 500;
        res.set_content(err.dump(), "application/json");
    }
});

    // POST /api/inscrieri-organizare — trimite inscriere
    svr.Post("/api/inscrieri-organizare", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int id_voluntar = body.value("id_voluntar", -1);
            std::string eveniment = body.value("eveniment", "");
            std::string rol = body.value("rol", "");
            std::string disponibilitate = body.value("disponibilitate", "");
            std::string competente = body.value("competente", "");
            std::string mentiuni = body.value("mentiuni", "");
            if (eveniment.empty()) {
                json err = {{"succes", false}, {"mesaj", "Evenimentul este obligatoriu."}};
                res.set_content(err.dump(), "application/json");
                return;
            }
            bool ok = db.adaugaInscriere(id_voluntar, eveniment, rol, disponibilitate, competente, mentiuni);
            json r = {{"succes", ok}};
            res.set_content(r.dump(), "application/json");
        } catch (...) {
            json err = {{"succes", false}, {"mesaj", "Eroare server."}};
            res.status = 500;
            res.set_content(err.dump(), "application/json");
        }
    });
    // POST /api/candidaturi — trimite candidatura
    svr.Post("/api/candidaturi", [&db](const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = json::parse(req.body);
        std::string nume = body.value("nume", "");
        std::string email = body.value("email", "");
        std::string telefon = body.value("telefon", "");
        std::string experienta = body.value("experienta", "");
        std::string scrisoare = body.value("scrisoare", "");
        std::string cv_link = body.value("cv_link", "");
        std::string tip_oferta = body.value("tip_oferta", "");

        if (nume.empty() || email.empty() || scrisoare.empty() || tip_oferta.empty()) {
            json err = {{"succes", false}, {"mesaj", "Campuri obligatorii lipsa."}};
            res.set_content(err.dump(), "application/json");
            return;
        }

        std::string sql = "INSERT INTO candidaturi (nume, email, telefon, experienta, scrisoare, cv_link, tip_oferta) VALUES (?, ?, ?, ?, ?, ?, ?)";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db.getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, nume.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, telefon.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, experienta.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, scrisoare.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 6, cv_link.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 7, tip_oferta.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            json r = {{"succes", true}, {"mesaj", "Candidatura a fost trimisa cu succes!"}};
            res.set_content(r.dump(), "application/json");
        } else {
            json err = {{"succes", false}, {"mesaj", "Eroare la salvare."}};
            res.set_content(err.dump(), "application/json");
        }
    } catch (...) {
        json err = {{"succes", false}, {"mesaj", "Eroare server."}};
        res.status = 500;
        res.set_content(err.dump(), "application/json");
    }
    });
    // POST /api/mesaje — trimite mesaj in grup
    svr.Post("/api/mesaje", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            int id_grup = body.value("id_grup", 0);
            int id_client = body.value("id_client", -1);
            std::string username = body.value("username", "anonim");
            std::string text = body.value("text", "");
            if (text.empty() || id_grup == 0) {
                json err = {{"succes", false}, {"mesaj", "Date incomplete."}};
                res.set_content(err.dump(), "application/json");
                return;
            }
            bool ok = db.adaugaMesaj(id_grup, id_client, username, text);
            json r = {{"succes", ok}};
            res.set_content(r.dump(), "application/json");
        } catch (...) {
            json err = {{"succes", false}, {"mesaj", "Eroare server."}};
            res.status = 500;
            res.set_content(err.dump(), "application/json");
        }
    });

    // GET /api/mesaje/:id_grup — mesaje grup
    svr.Get(R"(/api/mesaje/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        int id_grup = std::stoi(req.matches[1].str());
        auto mesaje = db.getMesajeGrup(id_grup);
        json arr = json::array();
        for (const auto& m : mesaje) {
            arr.push_back({
                {"id", m.id}, {"id_grup", m.id_grup}, {"id_client", m.id_client},
                {"username", m.username}, {"text", m.text}, {"data_ora", m.data_ora}
            });
        }
        res.set_content(arr.dump(), "application/json");
    });
    // GET /api/cladiri — toate cladirile cu program
    svr.Get("/api/cladiri", [&db](const httplib::Request&, httplib::Response& res) {
        int nr = db.getNrCladiri();
        json arr = json::array();
        for (int i = 1; i <= nr; i++) {
            arr.push_back({
                {"id", i},
                {"nume", db.getNumeCladire(i)}
            });
        }
        res.set_content(arr.dump(), "application/json");
    });

    // ════════════════════════════════════════════════════════
    // PORNIRE SERVER
    // ════════════════════════════════════════════════════════
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║     BIBLIOTECA ASTER — SERVER        ║\n";
    std::cout << "║  http://localhost:8080               ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";
    std::cout << "[SERVER] Baza de date: OK\n";
    std::cout << "[SERVER] Pornit pe portul 8080\n";
    std::cout << "[SERVER] Deschide http://localhost:8080 in browser\n\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}
