/*
 *	Project ExoJointData - Client Application
 *
 *  @author			Daniele Ronzani - Studente del Corso Magistrale in Ingegneria
 *										Meccatronica			
 *	@date			09/01/2015
 *  @version		1.0	
 *
 */


README per il progetto EXO JOINT DATA
========================================================

INDICE
1. Breve introduzione
2. Note sulla compilazione
3. Note sull'esecuzione
4. Informazioni sul copyright

======================
1. BREVE INTRODUZIONE
======================

exoJointData_client è una applicazione progettata per inviare dati ad una applicazione
server, del medesimo progetto, con il protocollo SEC NANOSEC VALUE.
La seguente applicazione simula la lettura dei dati misurati da un sensore che misura
l'angolo del giunto in un esosceletro per la riabilitazione.
L'applicazione è installata in un sistema embedded linux ed invia i dati tramite una 
connessione socket stream.

===========================
2. NOTE SULLA COMPILAZIONE
===========================
Per la compilazione, basterà digitare a terminale (nella directory build/):

$ cmake ..

e di seguito:

$ make

L'eseguibile (exoJointData_client) verrà posizionato nella directory bin/.

In fase di compilazione, è prevista un opzione che permette la visualizzazione 
di informazioni aggiuntive di debug, durante l'esecuzione del programma.

Per attivare la modalità di debug digitare a terminale (nella directory build/):

$ cmake -DCMAKE_BUILD_TYPE=Debug ..

e di seguito:

$ make

========================
3. NOTE SULL'ESECUZIONE
========================

L'applicazione viene eseguita scrivendo a terminale (nella directory bin/):

$ ./exoJointData_client [name_joint] [sampling_rate] [parabolic wave frequency] [amplitude]

Per la corretta esecuzione del programma sono necessari tutti i cinque parametri indicati.

Essendo la presente un'applicazione client di simulazione, l'indirizzo IP del server a
cui il client chiede la connessione non è indicato fra i parametri per comodità.
L'indirizzo di default è "127.0.0.1". Qualora si avesse la necessità di cambiare questo
valore bisogna modificare la macro HOSTNAME presente nell'header "exoJointData_global.h".
E' dunque necessario effettuare questa modifica in fase di compilazione.

===================================
4. INFORMAZIONI SUL COPYRIGHT
===================================
Licenza opensurce LGPL
Ultima versione: 1.00 gennaio 2015
