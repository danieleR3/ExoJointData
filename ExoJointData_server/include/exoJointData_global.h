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

/* MAXDATASIZE: massimo numero di bytes concessi ad un pacchetto contenente i dati del giunto SEC NANOSEC VALUE	*/
#define MAXDATASIZE 100  

/* MAXLATETIME: ritardo massimo in [ms] tollerato per l'arrivo di un dato */
#define MAXLATETIME 250

/* MAXBUFSIZE: dimensione massima del buffer (corrisponde al numero di campioni in ritardo critico) */
#define MAXBUFSIZE 25 

/* PORT: nome della porta a cui ci si vuole connettere */
#define PORT "3490"  

/* BACKLOG: quante connessioni pendenti in coda può sostenere */
#define BACKLOG 10	

/* MILLISECONDS: valore di conversione da secondi a millisecondi */
#define MILLISECONDS 1000

/* MICROSECONDS: valore di conversione da secondi a microsecondi */
#define MICROSECONDS 1000000

/* NANOSECONDS: valore di conversione da secondi a nanosecondi */
#define NANOSECONDS 1000000000

/* WAITTIME: periodo di tempo con cui sottocampionare i dati per la visualizzazione */
#define WAITTIME 1000000000

/* DEBUG: Flag per funzioni di debug impostato in fase di compilazione con cmake */
#ifdef DEBUG
#define DEBUG 1
#else
#define NDEBUG
#define DEBUG 0
#endif

// mutex, variabile di condizione e variabile golable
// per la notifica e sincronizzazione della thread di salvataggio
extern pthread_mutex_t mtxSave;
extern pthread_cond_t okSave;
extern int saveStarted;

//mutex per i buffer
extern pthread_mutex_t mtxJointBuf[2];
//buffer di salvataggio dei pacchetti
extern JointData *jointBuf[2];
//variabile che indica la coda del buffer
extern int nPkgBuf[2];

//mutex e variabile condizione per sincronizzare
//l'evento di ricezione dati
extern pthread_mutex_t mtxRecv[2];
extern pthread_cond_t okRecv;


//mutex e variabile connessione per sincronizzare l'invio dei dati
//alla thread per la visualizzazione 
extern pthread_mutex_t mtx2Bash;
extern pthread_cond_t newPkg2Bash;
//variabile per la comunicazione del pacchetto alla thread di visualizzazione 
extern JointData pkg2Bash;

//mutex e variabile  per la comunicazione fra le thread di logout di un client
extern pthread_mutex_t mtxCltLogOut;
extern int cltLogOut;

/* stop: variabile globale per notificare a tutte le thread di fermarsi (1 = continua, 0 = fermati)*/
extern int stop;
 
/* mtxStop: pthread mutex per garantire la mutua esclusione sulla variabile globale stop*/												 
extern pthread_mutex_t mtxStop;

#endif
