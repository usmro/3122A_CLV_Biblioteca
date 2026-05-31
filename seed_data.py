import random
import sqlite3
import os

DB_PATH = "biblioteca.db"
SCHEMA_PATH = "schema.sql"

# ── Date pentru generare carti ────────────────────────────────

AUTORI_FICTIUNE = [
    "Mihai Eminescu", "Ion Creanga", "Ioan Slavici", "Liviu Rebreanu",
    "Mihail Sadoveanu", "George Calinescu", "Camil Petrescu", "Tudor Arghezi",
    "Lucian Blaga", "Marin Preda", "Nichita Stanescu", "Ana Blandiana",
    "Gabriel Garcia Marquez", "Jorge Luis Borges", "Pablo Neruda",
    "Fyodor Dostoevsky", "Leo Tolstoy", "Anton Chekhov", "Ivan Turgenev",
    "Franz Kafka", "Thomas Mann", "Hermann Hesse", "Stefan Zweig",
    "Victor Hugo", "Gustave Flaubert", "Emile Zola", "Alexandre Dumas",
    "Charles Dickens", "Jane Austen", "Oscar Wilde", "Arthur Conan Doyle",
    "Ernest Hemingway", "F. Scott Fitzgerald", "William Faulkner",
    "George Orwell", "Aldous Huxley", "J.R.R. Tolkien", "C.S. Lewis",
    "Agatha Christie", "Virginia Woolf", "James Joyce"
]

AUTORI_NONFICTIUNE = [
    "Yuval Noah Harari", "Stephen Hawking", "Carl Sagan", "Richard Dawkins",
    "Malcolm Gladwell", "Daniel Kahneman", "Nassim Nicholas Taleb",
    "Jordan Peterson", "Michio Kaku", "Neil deGrasse Tyson",
    "Sigmund Freud", "Carl Jung", "Abraham Maslow", "Viktor Frankl",
    "Adam Smith", "John Maynard Keynes", "Milton Friedman",
    "Platon", "Aristotel", "Immanuel Kant", "Friedrich Nietzsche",
    "Albert Camus", "Jean-Paul Sartre", "Simone de Beauvoir",
    "Nicolae Iorga", "Vasile Parvan", "Constantin Giurescu"
]

TITLURI_FICTIUNE = [
    "Misterul_Noptii", "Umbra_Timpului", "Codrul_Fermecat", "Povestea_Uitata",
    "Inima_de_Piatra", "Lumina_din_Intuneric", "Visul_Pierdut", "Calatoria_Secreta",
    "Cetatea_de_Aur", "Razbunarea_Destinului", "Sufletul_Ratacit", "Tainele_Padurii",
    "Regele_Umbrei", "Printesa_Vantului", "Dragostea_Eterna", "Spiritul_Liber",
    "Noaptea_Vesnica", "Podul_dintre_Lumi", "Secretul_Familiei", "Orasul_Pierdut",
    "Masca_Adevarului", "Flacara_Sperantei", "Labirintul_Memoriei", "Comoara_Ascunsa",
    "Drumul_spre_Stele", "Glasul_Marii", "Legaturi_de_Sange", "Cercul_Magic",
    "Orizontul_Albastru", "Paznicul_Portilor", "Raul_fara_Nume", "Ultimul_Martor",
    "Poarta_Intunericului", "Chipul_din_Oglinda", "Pelerinul_Timpului", "Izvorul_Vietii",
    "Mantia_Regelui", "Sabia_Destinului", "Coroana_de_Foc", "Tronul_de_Gheata"
]

TITLURI_NONFICTIUNE = [
    "Istoria_Romaniei", "Fizica_Moderna", "Chimie_Avansata", "Biologie_Celulara",
    "Economie_Globala", "Filozofia_Antica", "Psihologia_Maselor", "Matematica_Superioara",
    "Geografie_Universala", "Drept_Constitutional", "Medicina_Interna", "Astronomie_Practica",
    "Lingvistica_Generala", "Sociologie_Urbana", "Arheologie_Clasica", "Informatica_Aplicata",
    "Ecologie_si_Mediu", "Nutritie_si_Sanatate", "Pedagogie_Moderna", "Istoria_Artei",
    "Management_Strategic", "Marketing_Digital", "Contabilitate_Moderna", "Statistica_Aplicata",
    "Filozofia_Mintii", "Neurologie_Clinica", "Genetica_Moderna", "Fizica_Cuantica",
    "Istoria_Universala", "Drept_Penal", "Economie_Comportamentala", "Antropologie_Culturala"
]

TITLURI_PERIODICE = [
    "Revista_Stiintifica_Romana", "Jurnal_de_Medicina", "Buletin_Tehnic",
    "Anale_de_Istorie", "Revista_de_Filozofie", "Jurnal_de_Chimie",
    "Revista_de_Drept", "Buletin_Economic", "Jurnal_de_Psihologie",
    "Revista_de_Geografie", "Anale_Universitare", "Revista_Tehnica"
]

GENURI_FICTIUNE = ["Roman", "Nuvela", "Poezie", "Drama", "SF", "Fantasy",
                   "Thriller", "Mister", "Romantic", "Istoric", "Copii"]
DOMENII_NONFICTIUNE = ["Stiinta", "Istorie", "Filozofie", "Economie", "Drept",
                        "Medicina", "Tehnologie", "Psihologie", "Geografie", "Arte"]
LIMBI = ["Romana", "Engleza", "Franceza", "Germana", "Spaniola", "Italiana", "Rusa"]
TARI = ["Romania", "Franta", "Germania", "Anglia", "SUA", "Rusia", "Spania", "Italia"]
EDITURI = ["Humanitas", "Polirom", "Corint", "Litera", "Nemira", "RAO",
           "Penguin", "Oxford", "Cambridge", "Gallimard", "Fischer"]

# ── Structura cladiri ─────────────────────────────────────────

CLADIRI = [
    {
        "nume": "Corpul A",
        "adresa": "Str. Principala 1",
        "contact": "021-001-0001",
        "email": "corp.a@biblioteca.ro",
        "etaje": 3,
        "angajati": 30,
        "capacitate": 120000,
        "central": 1,
        "deschidere": "08:00",
        "inchidere": "20:00",
        "descriere": "Sediul central administrativ si cultural",
        "sali": [
            # Parter (etaj 0)
            (0, "Cafenea Literara", "Cafenea", 60, 0, 0),
            (0, "Receptie si Info Point", "Receptie", 10, 0, 0),
            (0, "Lounge Lectura Rapida", "Lectura", 30, 0, 0),
            # Etaj 1
            (1, "Sala Open Shelf", "Lectura", 80, 0, 0),
            (1, "Aula Magna", "Eveniment", 150, 0, 1),
            (1, "Foaier Expozitional", "Expozitie", 50, 0, 0),
            # Etaj 2
            (2, "Grand Hall", "Lectura", 200, 0, 1),
            (2, "Birou Director", "Birou", 5, 0, 0),
            (2, "Secretariat", "Birou", 5, 0, 0),
            (2, "Birou HR", "Birou", 5, 0, 0),
            (2, "Sala Protocol", "Birou", 15, 0, 1),
            # Etaj 3
            (3, "Contabilitate", "Birou", 5, 0, 0),
            (3, "Server Room", "Tehnic", 2, 1, 0),
            (3, "Arhiva Centrala", "Depozit", 0, 0, 0),
        ]
    },
    {
        "nume": "Corpul B",
        "adresa": "Str. Principala 2",
        "contact": "021-001-0002",
        "email": "corp.b@biblioteca.ro",
        "etaje": 1,
        "angajati": 15,
        "capacitate": 60000,
        "central": 0,
        "deschidere": "08:00",
        "inchidere": "20:00",
        "descriere": "Studiu si non-fictiune",
        "sali": [
            (0, "Info Point B", "Receptie", 5, 0, 0),
            (0, "Sala Lectura B", "Lectura", 60, 0, 0),
            (0, "Depozit Acces Liber", "Depozit", 0, 0, 0),
            (1, "Sala Lectura B1", "Lectura", 50, 0, 0),
            (1, "Sala Studiu Individual 1", "Studiu", 1, 1, 0),
            (1, "Sala Studiu Individual 2", "Studiu", 1, 1, 0),
            (1, "Salon Literar", "Lectura", 20, 0, 1),
        ]
    },
    {
        "nume": "Corpul C",
        "adresa": "Str. Principala 3",
        "contact": "021-001-0003",
        "email": "corp.c@biblioteca.ro",
        "etaje": 0,
        "angajati": 8,
        "capacitate": 20000,
        "central": 0,
        "deschidere": "09:00",
        "inchidere": "22:00",
        "descriere": "Hub digital si mediateca",
        "sali": [
            (0, "Open IT Hub", "Mediateca", 40, 1, 1),
            (0, "Mediateca", "Mediateca", 30, 1, 0),
            (0, "Arhiva Speciala", "Depozit", 0, 0, 0),
            (0, "Laborator Tehnic", "Tehnic", 10, 1, 1),
        ]
    },
    {
        "nume": "Corpul D",
        "adresa": "Str. Principala 4",
        "contact": "021-001-0004",
        "email": "corp.d@biblioteca.ro",
        "etaje": 0,
        "angajati": 6,
        "capacitate": 15000,
        "central": 0,
        "deschidere": "09:00",
        "inchidere": "20:00",
        "descriere": "Fictiune si dezvoltare personala",
        "sali": [
            (0, "Info Point D", "Receptie", 3, 0, 0),
            (0, "Sala Studiu Individual D1", "Studiu", 1, 1, 0),
            (0, "Sala Studiu Individual D2", "Studiu", 1, 1, 0),
            (0, "Sala Studiu Individual D3", "Studiu", 1, 1, 0),
            (0, "Common Space", "Lectura", 40, 0, 0),
        ]
    },
    {
        "nume": "Corpul E",
        "adresa": "Str. Principala 5",
        "contact": "021-001-0005",
        "email": "corp.e@biblioteca.ro",
        "etaje": 1,
        "angajati": 12,
        "capacitate": 40000,
        "central": 0,
        "deschidere": "08:00",
        "inchidere": "18:00",
        "descriere": "Sectia stiintifica si cercetare",
        "sali": [
            (0, "Receptie Cercetare", "Receptie", 5, 0, 0),
            (0, "Sala Jurnale Stiintifice", "Lectura", 40, 0, 0),
            (0, "Birou Consiliu Stiintific", "Birou", 10, 0, 1),
            (1, "Grand Hall Stiintific", "Lectura", 100, 0, 1),
            (1, "Depozit Arhivistic", "Depozit", 0, 0, 0),
            (1, "Laborator Conservare", "Tehnic", 5, 0, 0),
        ]
    },
    {
        "nume": "Corpul F",
        "adresa": "Str. Principala 6",
        "contact": "021-001-0006",
        "email": "corp.f@biblioteca.ro",
        "etaje": 1,
        "angajati": 10,
        "capacitate": 10000,
        "central": 0,
        "deschidere": "09:00",
        "inchidere": "19:00",
        "descriere": "Sectia pentru copii si evenimente",
        "sali": [
            (0, "Ludoteca", "Ludoteca", 30, 0, 0),
            (0, "Sala Ora Povestirilor", "Eveniment", 25, 0, 0),
            (0, "Sala Mare Evenimente", "Eveniment", 100, 0, 1),
            (0, "Receptie si Garderoba", "Receptie", 5, 0, 0),
            (1, "Atelier Creatie", "Atelier", 20, 0, 0),
            (1, "Biblioteca Parintilor", "Lectura", 30, 0, 0),
        ]
    },
]

ZILE = ["Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata", "Duminica"]

# ── Distributia cartilor pe corpuri ──────────────────────────

# (tip, suport, corp_index)  corp_index 0=A, 1=B, 2=C, 3=D, 4=E, 5=F
DISTRIBUTIE = [
    # Corp A - putin din toate
    ("Fictiune",    "FIZIC",       0, 120),
    ("NonFictiune", "FIZIC",       0, 100),
    ("Periodica",   "FIZIC",       0,  50),
    ("Fictiune",    "DIGITAL_PDF", 0,  30),
    ("NonFictiune", "DIGITAL_PDF", 0,  30),
    ("Fictiune",    "AUDIOBOOK",   0,  20),
    ("Fictiune",    "VIDEO",       0,  20),
    # Corp B - non-fictiune si clasica
    ("NonFictiune", "FIZIC",       1, 180),
    ("Fictiune",    "FIZIC",       1,  80),
    ("Periodica",   "FIZIC",       1,  40),
    # Corp C - digital
    ("Fictiune",    "DIGITAL_PDF", 2,  80),
    ("NonFictiune", "DIGITAL_PDF", 2,  80),
    ("Fictiune",    "AUDIOBOOK",   2,  60),
    ("Fictiune",    "VIDEO",       2,  60),
    ("NonFictiune", "AUDIOBOOK",   2,  30),
    # Corp D - fictiune si dezvoltare
    ("Fictiune",    "FIZIC",       3, 150),
    ("NonFictiune", "FIZIC",       3,  80),
    ("Fictiune",    "DIGITAL_PDF", 3,  20),
    # Corp E - stiintific
    ("NonFictiune", "FIZIC",       4, 180),
    ("Periodica",   "FIZIC",       4,  80),
    ("NonFictiune", "DIGITAL_PDF", 4,  50),
    # Corp F - copii si parinti
    ("Fictiune",    "FIZIC",       5, 100),
    ("NonFictiune", "FIZIC",       5,  50),
]

# ─────────────────────────────────────────────────────────────

def gen_carte(tip, suport, id_cladire, id_sala, id_raft, serie_idx):
    an = random.randint(1950, 2024)
    stoc = random.randint(1, 5) if suport == "FIZIC" else 999
    patrimoniu = 1 if (an < 1960 and random.random() < 0.1) else 0
    rating = round(random.uniform(3.0, 5.0), 1)
    pret = round(random.uniform(15.0, 150.0), 2)

    if tip == "Fictiune":
        titlu = random.choice(TITLURI_FICTIUNE) + "_" + str(random.randint(1, 999))
        autor = random.choice(AUTORI_FICTIUNE)
        gen = random.choice(GENURI_FICTIUNE)
        pagini = random.randint(100, 600) if suport != "AUDIOBOOK" else 0
        minute = random.randint(120, 900) if suport == "AUDIOBOOK" else 0
        val = minute if suport == "AUDIOBOOK" else pagini
        mb = round(random.uniform(1.0, 50.0), 1) if suport != "FIZIC" else 0.0
        return (titlu, autor, random.choice(EDITURI), random.choice(LIMBI),
                f"{an}-01-01", an, pagini, random.choice(TARI), tip, suport,
                "Disponibila", "Achizitie", "Biblioteca",
                f"SC-F-{serie_idx:06d}", stoc, patrimoniu, pret,
                "Rezumat in curs de adaugare.", rating,
                id_raft, id_sala, id_cladire, f"COD-{serie_idx:06d}",
                val, mb,
                f"ISBN-{random.randint(1000000000,9999999999)}", gen,
                "Protagonist", "", "Naratiune",
                random.randint(6, 18), "Standard", round(random.uniform(0.2, 1.5), 2),
                random.choice(["Cartonata", "Brosata"]),
                None, None, None, None, 0, None)
    elif tip == "NonFictiune":
        titlu = random.choice(TITLURI_NONFICTIUNE) + "_" + str(random.randint(1, 999))
        autor = random.choice(AUTORI_NONFICTIUNE)
        domeniu = random.choice(DOMENII_NONFICTIUNE)
        pagini = random.randint(150, 800)
        mb = round(random.uniform(2.0, 80.0), 1) if suport != "FIZIC" else 0.0
        return (titlu, autor, random.choice(EDITURI), random.choice(LIMBI),
                f"{an}-01-01", an, pagini, random.choice(TARI), tip, suport,
                "Disponibila", "Achizitie", "Biblioteca",
                f"SC-N-{serie_idx:06d}", stoc, patrimoniu, pret,
                "Rezumat in curs de adaugare.", rating,
                id_raft, id_sala, id_cladire, f"COD-{serie_idx:06d}",
                pagini, mb,
                f"ISBN-{random.randint(1000000000,9999999999)}", None,
None, None, None, 0, None, 0.0, None,
domeniu, "Institutie", "v1.0", None, 0, None)
    else:  # Periodica
        titlu = random.choice(TITLURI_PERIODICE) + "_" + str(random.randint(1, 99))
        autor = "Redactia"
        pagini = random.randint(50, 200)
        return (titlu, autor, random.choice(EDITURI), random.choice(LIMBI),
                f"{an}-01-01", an, pagini, random.choice(TARI), tip, suport,
                "Disponibila", "Abonament", "Biblioteca",
                f"SC-P-{serie_idx:06d}", stoc, 0, pret,
                "Publicatie periodica.", rating,
                id_raft, id_sala, id_cladire, f"COD-{serie_idx:06d}",
                pagini, 0.0,
                None, None, None, None, None, 0, None, 0.0, None,
                None, None, None,
                f"ISSN-{random.randint(10000000,99999999)}",
                random.randint(1, 200),
                random.choice(["Lunar", "Saptamanal", "Anual", "Trimestrial"]))


def main():
    # Sterge DB vechi
    if os.path.exists(DB_PATH):
        os.remove(DB_PATH)

    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()

    # Incarca schema
    with open(SCHEMA_PATH, "r", encoding="utf-8") as f:
        cur.executescript(f.read())
    print("[1/4] Schema creata.")

    # ── Insereaza cladiri, etaje, sali, rafturi ───────────────
    cladire_ids = []
    sala_map = {}   # (corp_idx, etaj, nume_sala) → id_sala
    raft_map = {}   # corp_idx → lista id_rafturi

    for corp_idx, corp in enumerate(CLADIRI):
        cur.execute("""INSERT INTO cladiri
            (nume, adresa, numar_contact, email_contact, numar_etaje,
             numar_total_angajati, capacitate_maxima_carti, este_sediu_central,
             ora_deschidere, ora_inchidere, descriere)
            VALUES (?,?,?,?,?,?,?,?,?,?,?)""",
            (corp["nume"], corp["adresa"], corp["contact"], corp["email"],
             corp["etaje"], corp["angajati"], corp["capacitate"], corp["central"],
             corp["deschidere"], corp["inchidere"], corp["descriere"]))
        id_cladire = cur.lastrowid
        cladire_ids.append(id_cladire)

        # Program
        for zi in ZILE:
            if zi == "Duminica":
                interval = "Inchis"
            elif zi == "Sambata":
                interval = "09:00 - 14:00"
            else:
                interval = f"{corp['deschidere']} - {corp['inchidere']}"
            cur.execute("INSERT INTO program_cladire (id_cladire, zi, interval) VALUES (?,?,?)",
                        (id_cladire, zi, interval))

        # Etaje
        etaj_ids = {}
        for nr_etaj in range(corp["etaje"] + 1):
            den = "Parter" if nr_etaj == 0 else f"Etaj {nr_etaj}"
            cur.execute("INSERT INTO etaje (id_cladire, numar_etaj, denumire) VALUES (?,?,?)",
                        (id_cladire, nr_etaj, den))
            etaj_ids[nr_etaj] = cur.lastrowid

        # Sali + rafturi
        raft_map[corp_idx] = []
        for (nr_etaj, nume_sala, tip_sala, cap, calc, proiector) in corp["sali"]:
            id_etaj = etaj_ids.get(nr_etaj, list(etaj_ids.values())[0])
            cur.execute("""INSERT INTO sali
                (id_etaj, id_cladire, nume, tip, capacitate, are_calculator, are_proiector)
                VALUES (?,?,?,?,?,?,?)""",
                (id_etaj, id_cladire, nume_sala, tip_sala, cap, calc, proiector))
            id_sala = cur.lastrowid
            sala_map[(corp_idx, nr_etaj, nume_sala)] = id_sala

            # Rafturi doar pentru salile de lectura/depozit
            if tip_sala in ("Lectura", "Depozit", "Studiu", "Ludoteca", "Mediateca"):
                for r in range(1, 6):
                    cod = f"{corp['nume'][7]}{nr_etaj}-R{r:02d}"
                    cur.execute("INSERT INTO rafturi (id_sala, cod_raft) VALUES (?,?)",
                                (id_sala, cod))
                    raft_map[corp_idx].append((cur.lastrowid, id_sala))

    print("[2/4] Cladiri, etaje, sali si rafturi inserate.")

    # ── Insereaza carti ───────────────────────────────────────
    INSERT_CARTE = """INSERT INTO carti
        (titlu, autor, editura, limba, data_pub, an_aparitie, nr_pagini,
         tara_provenienta, tip, suport, status, sursa, destinatar,
         serie_contabila, exemplare_disponibile, este_patrimoniu, pret_intrare,
         rezumat, rating, id_raft, id_sala, id_cladire, cod_identificare,
         valoare_masurabila, marime_mb,
         isbn, gen_specific, personaj_principal, serie, tip_naratiune,
         varsta_recomandata, dimensiuni, greutate, tip_coperta,
         domeniu, institutie_sursa, editie_revizuita,
         issn, numar_editie, frecventa)
        VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,
                ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"""

    serie_idx = 1
    total_carti = 0
    for (tip, suport, corp_idx, nr) in DISTRIBUTIE:
        id_cladire = cladire_ids[corp_idx]
        rafturi = raft_map.get(corp_idx, [])

        for _ in range(nr):
            if rafturi:
                id_raft, id_sala = random.choice(rafturi)
            else:
                id_raft, id_sala = None, None

            date = gen_carte(tip, suport, id_cladire, id_sala, id_raft, serie_idx)
            cur.execute(INSERT_CARTE, date)
            serie_idx += 1
            total_carti += 1

    print(f"[3/4] {total_carti} carti inserate.")

    # ── Autori din carti ──────────────────────────────────────
    cur.execute("SELECT DISTINCT autor FROM carti")
    autori = cur.fetchall()
    for (nume,) in autori:
        cur.execute("INSERT INTO autori (nume_complet, biografie) VALUES (?,?)",
                    (nume, "Biografie in curs de actualizare."))
        id_autor = cur.lastrowid
        cur.execute("UPDATE carti SET id_autor=? WHERE autor=?", (id_autor, nume))

    print(f"[4/4] {len(autori)} autori inserati si corelati.")

    conn.commit()
    conn.close()
    print(f"\nDone! Baza de date '{DB_PATH}' creata cu {total_carti} carti si {len(autori)} autori.")
    print("Poti mari numarul de carti modificand valorile din DISTRIBUTIE.")


if __name__ == "__main__":
    main()
