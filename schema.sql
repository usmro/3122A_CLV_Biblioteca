-- ============================================================
-- SCHEMA COMPLETA - BIBLIOTECA
-- ============================================================

PRAGMA foreign_keys = ON;

-- ============================================================
-- 1. INFRASTRUCTURA (Cladiri → Etaje → Sali → Rafturi)
-- ============================================================

CREATE TABLE IF NOT EXISTS cladiri (
    id                      INTEGER PRIMARY KEY AUTOINCREMENT,
    nume                    TEXT NOT NULL,          -- "Corpul A", "Corpul B" etc.
    adresa                  TEXT,
    numar_contact           TEXT,
    email_contact           TEXT,
    numar_etaje             INTEGER DEFAULT 0,
    numar_total_angajati    INTEGER DEFAULT 0,
    capacitate_maxima_carti INTEGER DEFAULT 0,
    este_sediu_central      INTEGER DEFAULT 0,      -- 0/1
    ora_deschidere          TEXT DEFAULT '08:00',
    ora_inchidere           TEXT DEFAULT '20:00',
    descriere               TEXT
);

CREATE TABLE IF NOT EXISTS etaje (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    id_cladire  INTEGER NOT NULL REFERENCES cladiri(id) ON DELETE CASCADE,
    numar_etaj  INTEGER NOT NULL,                   -- 0 = parter
    denumire    TEXT                                -- "Parter", "Etaj 1" etc.
);

CREATE TABLE IF NOT EXISTS sali (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_etaj         INTEGER NOT NULL REFERENCES etaje(id) ON DELETE CASCADE,
    id_cladire      INTEGER NOT NULL REFERENCES cladiri(id) ON DELETE CASCADE,
    nume            TEXT NOT NULL,
    tip             TEXT NOT NULL,  -- Lectura/Studiu/Eveniment/Birou/Tehnic/Depozit/Cafenea/Ludoteca/Mediateca/Expozitie
    capacitate      INTEGER DEFAULT 0,
    are_calculator  INTEGER DEFAULT 0,
    are_proiector   INTEGER DEFAULT 0,
    descriere       TEXT
);

CREATE TABLE IF NOT EXISTS rafturi (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    id_sala     INTEGER NOT NULL REFERENCES sali(id) ON DELETE CASCADE,
    cod_raft    TEXT NOT NULL,      -- ex: "A2-R01"
    tip_colectie TEXT              -- Fictiune/NonFictiune/Periodice/Copii/Stiintific
);

CREATE TABLE IF NOT EXISTS program_cladire (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    id_cladire  INTEGER NOT NULL REFERENCES cladiri(id) ON DELETE CASCADE,
    zi          TEXT NOT NULL,      -- Luni, Marti etc.
    interval    TEXT NOT NULL       -- "08:00 - 20:00" sau "Inchis"
);

-- ============================================================
-- 2. CARTI SI AUTORI
-- ============================================================

CREATE TABLE IF NOT EXISTS autori (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    nume_complet    TEXT NOT NULL,
    biografie       TEXT,
    cale_poza       TEXT
);

CREATE TABLE IF NOT EXISTS carti (
    id                      INTEGER PRIMARY KEY AUTOINCREMENT,
    titlu                   TEXT NOT NULL,
    autor                   TEXT NOT NULL,          -- nume direct (+ FK optional)
    id_autor                INTEGER REFERENCES autori(id) ON DELETE SET NULL,
    editura                 TEXT,
    limba                   TEXT DEFAULT 'Romana',
    data_pub                TEXT,
    an_aparitie             INTEGER,
    nr_pagini               INTEGER DEFAULT 0,
    tara_provenienta        TEXT,
    tip                     TEXT NOT NULL,          -- Fictiune/NonFictiune/Periodica
    suport                  TEXT NOT NULL,          -- FIZIC/DIGITAL_PDF/AUDIOBOOK/VIDEO
    status                  TEXT DEFAULT 'Disponibila',
    sursa                   TEXT,                   -- Achizitie/Donatie/Abonament
    destinatar              TEXT,
    serie_contabila         TEXT,
    exemplare_disponibile   INTEGER DEFAULT 1,
    este_patrimoniu         INTEGER DEFAULT 0,
    pret_intrare            REAL DEFAULT 0.0,
    rating                  REAL DEFAULT 0.0,
    rezumat                 TEXT,
    cale_poza               TEXT,
    valoare_masurabila      INTEGER DEFAULT 0,      -- nr pagini sau minute
    marime_mb               REAL DEFAULT 0.0,
    -- Locatie fizica
    id_raft                 INTEGER REFERENCES rafturi(id) ON DELETE SET NULL,
    id_sala                 INTEGER REFERENCES sali(id) ON DELETE SET NULL,
    id_cladire              INTEGER REFERENCES cladiri(id) ON DELETE SET NULL,
    cod_identificare        TEXT,
    -- Fictiune
    isbn                    TEXT,
    gen_specific            TEXT,
    personaj_principal      TEXT,
    serie                   TEXT,
    tip_naratiune           TEXT,
    varsta_recomandata      INTEGER DEFAULT 0,
    dimensiuni              TEXT,
    greutate                REAL DEFAULT 0.0,
    tip_coperta             TEXT,
    -- NonFictiune
    subcategorie            TEXT,
    domeniu                 TEXT,
    institutie_sursa        TEXT,
    editie_revizuita        TEXT,
    -- Periodice
    issn                    TEXT,
    numar_editie            INTEGER DEFAULT 0,
    frecventa               TEXT
);

CREATE TABLE IF NOT EXISTS defecte_carti (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    id_carte    INTEGER NOT NULL REFERENCES carti(id) ON DELETE CASCADE,
    descriere   TEXT NOT NULL,
    data        TEXT
);

CREATE TABLE IF NOT EXISTS recenzii (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_carte        INTEGER NOT NULL REFERENCES carti(id) ON DELETE CASCADE,
    id_utilizator   INTEGER NOT NULL,
    username        TEXT,
    nota            INTEGER CHECK(nota BETWEEN 1 AND 5),
    comentariu      TEXT,
    data            TEXT
);

-- ============================================================
-- 3. UTILIZATORI (ierarhie: utilizatori → clienti → voluntari → angajati)
-- ============================================================

CREATE TABLE IF NOT EXISTS utilizatori (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    nume            TEXT NOT NULL,
    prenume         TEXT NOT NULL,
    email           TEXT UNIQUE,
    telefon         TEXT,
    data_nasterii   TEXT,
    gen             TEXT,
    parola_criptata TEXT NOT NULL,
    data_inscriere  TEXT,
    status          TEXT DEFAULT 'Activ',
    cale_poza       TEXT,
    tip             TEXT NOT NULL   -- Client/Voluntar/Angajat
);

CREATE TABLE IF NOT EXISTS clienti (
    id_utilizator           INTEGER PRIMARY KEY REFERENCES utilizatori(id) ON DELETE CASCADE,
    username                TEXT UNIQUE NOT NULL,
    varsta                  INTEGER,
    rank                    TEXT DEFAULT 'Novice',
    total_imprumuturi       INTEGER DEFAULT 0,
    data_inscriere_client   TEXT
);

CREATE TABLE IF NOT EXISTS voluntari (
    id_utilizator           INTEGER PRIMARY KEY REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    cale_contract           TEXT,
    data_start_contract     TEXT,
    data_end_contract       TEXT,
    ore_lucrate             INTEGER DEFAULT 0,
    rating_mediu            REAL DEFAULT 5.0,
    observatii_activitate   TEXT,
    link_idei_eveniment     TEXT,
    link_inscriere_org      TEXT,
    link_program_colegi     TEXT,
    link_program_personal   TEXT,
    -- Corp unde activeaza de obicei (poate fi mutat)
    id_cladire_activa       INTEGER REFERENCES cladiri(id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS angajati (
    id_utilizator           INTEGER PRIMARY KEY REFERENCES voluntari(id_utilizator) ON DELETE CASCADE,
    salariu_baza            REAL DEFAULT 0.0,
    zile_concediu_ramase    INTEGER DEFAULT 21,
    id_superior             INTEGER REFERENCES utilizatori(id) ON DELETE SET NULL,
    parola_resursa          TEXT,
    tip_angajat             TEXT,   -- Conducere/Specialist/Creativ/Suport
    -- Corp fix al angajatului
    id_cladire_post         INTEGER REFERENCES cladiri(id) ON DELETE SET NULL,
    id_sala_post            INTEGER REFERENCES sali(id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS istoric_pozitii (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_angajat      INTEGER NOT NULL REFERENCES angajati(id_utilizator) ON DELETE CASCADE,
    pozitie         TEXT NOT NULL,
    data_start      TEXT,
    data_end        TEXT
);

CREATE TABLE IF NOT EXISTS program_personal_voluntar (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_voluntar     INTEGER NOT NULL REFERENCES voluntari(id_utilizator) ON DELETE CASCADE,
    zi              TEXT NOT NULL,
    interval        TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS feedback_voluntari (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_voluntar     INTEGER NOT NULL REFERENCES voluntari(id_utilizator) ON DELETE CASCADE,
    nume_client     TEXT,
    data            TEXT,
    stelute         INTEGER CHECK(stelute BETWEEN 1 AND 5),
    comentariu      TEXT
);

-- ============================================================
-- 4. IMPRUMUTURI SI REZERVARI
-- ============================================================

CREATE TABLE IF NOT EXISTS imprumuturi (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_client       INTEGER NOT NULL REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    id_carte        INTEGER NOT NULL REFERENCES carti(id) ON DELETE CASCADE,
    data_imprumut   TEXT NOT NULL,
    data_limita     TEXT NOT NULL,
    data_retur_real TEXT,
    returnata       INTEGER DEFAULT 0,  -- 0/1
    intarziata      INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS rezervari (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_client       INTEGER NOT NULL REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    id_carte        INTEGER NOT NULL REFERENCES carti(id) ON DELETE CASCADE,
    data_rezervare  TEXT NOT NULL,
    data_expirare   TEXT NOT NULL,      -- rezervarea expira in 48h
    ridicata        INTEGER DEFAULT 0,
    anulata         INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS progres_lectura (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_client       INTEGER NOT NULL REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    id_carte        INTEGER NOT NULL REFERENCES carti(id) ON DELETE CASCADE,
    pagini_citite   INTEGER DEFAULT 0,
    stelute         INTEGER DEFAULT 0,
    observatii      TEXT,
    status          TEXT DEFAULT 'DORITA', -- DORITA/IN_PROGRES/CITITA/REZERVATA
    data_finalizare TEXT
);

CREATE TABLE IF NOT EXISTS wishlist (
    id_client   INTEGER NOT NULL REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    id_carte    INTEGER NOT NULL REFERENCES carti(id) ON DELETE CASCADE,
    PRIMARY KEY (id_client, id_carte)
);

CREATE TABLE IF NOT EXISTS prietenii (
    id_client1  INTEGER NOT NULL REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    id_client2  INTEGER NOT NULL REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    data        TEXT,
    PRIMARY KEY (id_client1, id_client2)
);

-- ============================================================
-- 5. EVENIMENTE
-- ============================================================

CREATE TABLE IF NOT EXISTS evenimente (
    id                      INTEGER PRIMARY KEY AUTOINCREMENT,
    id_cladire              INTEGER REFERENCES cladiri(id) ON DELETE SET NULL,
    id_sala                 INTEGER REFERENCES sali(id) ON DELETE SET NULL,
    id_autor_invitat        INTEGER REFERENCES autori(id) ON DELETE SET NULL,
    nume                    TEXT NOT NULL,
    data                    TEXT NOT NULL,
    ora_inceput             TEXT,
    durata_minute           INTEGER DEFAULT 60,
    nr_participanti_estimat INTEGER DEFAULT 0,
    angajati_necesari       INTEGER DEFAULT 0,
    descriere               TEXT,
    este_sedinta            INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS participanti_evenimente (
    id_eveniment    INTEGER NOT NULL REFERENCES evenimente(id) ON DELETE CASCADE,
    id_client       INTEGER NOT NULL REFERENCES clienti(id_utilizator) ON DELETE CASCADE,
    a_participat    INTEGER DEFAULT 0,
    PRIMARY KEY (id_eveniment, id_client)
);

CREATE TABLE IF NOT EXISTS calendar_voluntari (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_voluntar     INTEGER NOT NULL REFERENCES voluntari(id_utilizator) ON DELETE CASCADE,
    id_eveniment    INTEGER REFERENCES evenimente(id) ON DELETE SET NULL,
    titlu           TEXT,
    data            TEXT,
    ora_inceput     TEXT,
    durata_minute   INTEGER DEFAULT 60,
    descriere       TEXT,
    este_sedinta    INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS notificari (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_voluntar     INTEGER NOT NULL REFERENCES voluntari(id_utilizator) ON DELETE CASCADE,
    mesaj           TEXT NOT NULL,
    data            TEXT,
    citita          INTEGER DEFAULT 0
);

-- ============================================================
-- 6. INVENTAR
-- ============================================================

CREATE TABLE IF NOT EXISTS rapoarte_inventar (
    id                  INTEGER PRIMARY KEY AUTOINCREMENT,
    data_efectuare      TEXT NOT NULL,
    id_angajat          INTEGER REFERENCES angajati(id_utilizator) ON DELETE SET NULL,
    nume_responsabil    TEXT,
    carti_gasite        INTEGER DEFAULT 0,
    carti_lipsa         INTEGER DEFAULT 0,
    observatii          TEXT
);

-- ============================================================
-- 7. CAFENEA (pregatita pentru viitor, nu e folosita initial)
-- ============================================================

CREATE TABLE IF NOT EXISTS cafenea_produse (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    nume            TEXT NOT NULL,
    categorie       TEXT,           -- Bautura/Mancare/Desert
    pret            REAL DEFAULT 0.0,
    alergeni        TEXT,
    provenienta     TEXT,
    data_expirare   TEXT,
    disponibil      INTEGER DEFAULT 1
);

CREATE TABLE IF NOT EXISTS cafenea_rezervari (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    id_client       INTEGER REFERENCES clienti(id_utilizator) ON DELETE SET NULL,
    data            TEXT NOT NULL,
    ora_inceput     TEXT,
    ora_sfarsit     TEXT,
    nr_persoane     INTEGER DEFAULT 1,
    tip             TEXT DEFAULT 'Masa',    -- Masa/Eveniment
    status          TEXT DEFAULT 'Activa'   -- Activa/Anulata/Finalizata
);

CREATE TABLE IF NOT EXISTS cafenea_curatenie (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    data        TEXT NOT NULL,
    ora         TEXT,
    id_angajat  INTEGER REFERENCES angajati(id_utilizator) ON DELETE SET NULL,
    zona        TEXT,
    observatii  TEXT
);

-- ============================================================
-- INDECSI pentru performanta la cautare
-- ============================================================

CREATE INDEX IF NOT EXISTS idx_carti_titlu     ON carti(titlu);
CREATE INDEX IF NOT EXISTS idx_carti_autor     ON carti(autor);
CREATE INDEX IF NOT EXISTS idx_carti_tip       ON carti(tip);
CREATE INDEX IF NOT EXISTS idx_carti_suport    ON carti(suport);
CREATE INDEX IF NOT EXISTS idx_carti_cladire   ON carti(id_cladire);
CREATE INDEX IF NOT EXISTS idx_imprumuturi_client ON imprumuturi(id_client);
CREATE INDEX IF NOT EXISTS idx_imprumuturi_carte  ON imprumuturi(id_carte);
CREATE INDEX IF NOT EXISTS idx_rezervari_client   ON rezervari(id_client);
