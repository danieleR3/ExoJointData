/*
 *	Project ExoJointData - 
 *
 *  @author			Daniele Ronzani - Studente del Corso Magistrale in Ingegneria
 *										Meccatronica			
 *	@date			09/01/2015
 *  @version		1.0	
 *
 */


README per il progetto EXO JOINT DATA
========================================================

======================
BREVE INTRODUZIONE
======================

exoJointData_client è una applicazione progettata per inviare dati ad una applicazione
server, del medesimo progetto, con il protocollo SEC NANOSEC VALUE.
La seguente applicazione simula la lettura dei dati misurati da un sensore che misura
l'angolo del giunto in un esosceletro per la riabilitazione.
L'applicazione è installata in un sistema embedded linux ed invia i dati tramite una 
connessione socket stream.

exoJointData_server è una applicazione progettata per ricevere dati raccolti da uno o due
sensori distinti, applicati in diversi giunti di un esosceletro per riabilitazione motoria.
Questa applicazione server riceve i dati secondo un'opportuna codifica contenente dati nel
protocollo SEC NANOSEC VALUE.
L'applicazione client che invia i dati fa parte dello stesso progetto software ed è dunque 
progettata per essere pienamente compatibile con la presente applicazione server.