Tema: Dezvoltarea unei aplicații în C++ pentru administrarea unei biblioteci și a bazei de date a utilizatorilor unei biblioteci.

Motivare: Proiectul a fost ales pentru a demonstra aplicarea conceptelor fundamentale de Programare Orientată pe Obiecte (POO). Tema permite utilizarea moștenirii pentru a diferenția tipurile de publicații și a polimorfismului pentru gestionarea uniformă a acestora. Sistemul urmărește automatizarea procesului de împrumut și asigurarea consistenței datelor între entități.

Structura Logică a Datelor

Clasa: Cărți

Câmp		Tip		Dimensiune		Descriere
Id_carte	Întreg		10			Identificator unic (necesar pentru legături) 
Titlu		Caracter	50			Titlul complet al cărții
Autor		Caracter	50			Numele autorului
Editura		Caracter	30			Casa de editură
ISBN		Caracter	20			Codul unic internațional
Limba		Caracter	20			Limba în care este scrisă
Data_pub	Caracter	12			Format 'yyyy-mm-dd' 
Nr_pagini	Întreg		5			Numărul de pagini
Destinatar	Caracter	30			Publicul țintă (Ex: Copii, Academic)
Subiect		Caracter	100			Cuvinte cheie despre subiect
Note		Caracter	200			Note generale/Observații 
Sursa		Caracter	30			Proveniența (Editură/Donator)
Status		Caracter	15			Disponibilă / Împrumutată

Clasa: Utilizator

Câmp		        Tip		        Dimensiune		    Descriere
Id_utilizator	    Întreg(Long)	10			        Identificator unic (Primary Key)
Nume		        Caracter	    30			        Numele de familie
Prenume		        Caracter	    30			        Prenumele
Email		        Caracter	    50			        Adresa de mail (Username)
//Parola	        Caracter	    64
Telefon		        Caracter	    15			        Număr de contact
Data_nasterii	    Caracter	    12			        Format 'yyyy-mm-dd'
Gen		            Caracter	    1			        M / F / N		
Tip_Utilizator	    Caracter	    15			        Student, Adult, Elev
Status		        Caracter	    15			        Activ, Penalizat, Suspendat
Data_Inscriere	    Caracter	    12			        Data creării contului	
Carti_Active	    Întreg		    2			        Numărul de cărți deținute în prezent
