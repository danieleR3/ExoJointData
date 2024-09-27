/*
 *	exoJointData_global.h
 *	
 *	@description	header che raccoglie le macro e le variabili globali
 *					utilizzate nei sorgenti del progetto ExoJointData
 *
 *  @author			Daniele Ronzani - Studente del Corso Magistrale in Ingegneria
 *										Meccatronica			
 *	@date			09/01/2015
 *  @version		1.0	
 *
 */


#ifndef __EJD_GLOBAL__
#define __EJD_GLOBAL__

/* PORT: nome della porta a cui ci si vuole connettere */
#define PORT "3490"

/* HOSTNAME: indirizzo a cui ci si vuole connettere */
#define HOSTNAME "127.0.0.1"

/* MICROSECONDS: valore di conversione da secondi a microsecondi */
#define MICROSECONDS 1000000

/* MAXDATASIZE: massimo numero di bytes concessi ad un pacchetto contenente i dati del giunto SEC NANOSEC VALUE	*/
#define MAXDATASIZE 100  


/* DEBUG: Flag per funzioni di debug impostato in fase di compilazione con cmake */
#ifdef DEBUG
#define DEBUG 1
#else
#define NDEBUG
#define DEBUG 0
#endif


/* stop: variabile globale per notificare a tutte le thread di fermarsi (1 = continua, 0 = fermati)*/
extern int stop;

 
/* mtxStop: pthread mutex per garantire la mutua esclusione sulla variabile globale stop*/												 
extern pthread_mutex_t mtxStop;


#endif
