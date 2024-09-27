/*
 *	exoJointData_auxFunction.h
 *	
 *	@description	header del sorgente exoJointData_auxFunction che raccoglie 
 *					la definizione delle stutture, enumerazioni e funzioni
 *					ausiliarie in uso nel progetto ExoJointData 
 *
 *  @author			Daniele Ronzani - Studente del Corso Magistrale in Ingegneria
 *										Meccatronica			
 *	@date			09/01/2015
 *  @version		1.0	
 *
 */

#ifndef __EJD_AUX__
#define __EJD_AUX__

/************************** STRUTTURE ED ENUMERAZIONI ***************************/

/*
 *	joint: 		Enumerazione che indica a quale giunto si fa riferimento
 *	@state  	ANKLE = si fa riferimento al giunto della caviglia
 *	@state  	KNEE  =	si fa riferimento al giunto del ginocchio
 */
enum joint{ ANKLE=0, KNEE};

/*
 *	client_check: 	Enumerazione che indica le istruzioni inviate dal server in base alla configurazione.
 *	@state  		STOP =	indica che il client non corrisponde a quanto atteso e dunque deve fermarsi 
 * 	@state 			OK   =	indica che il client corrisponde al giunto atteso, può continuare 
 */
enum client_check { STOP=0, OK};

/*
 *	ClientParameter: 	Struttura che raccoglie le variabili necessarie alla thread che genera e invia
 *						i dati del giunto.
 *	@variable  			jointName = nome del giunto su cui è installato il client
 *	@variable  			sockfd = indica il file descriptor per la connessione socket client serve
 *	@variable			sampling_rate = frequenza di campionamento in Hz a cui sono generati i dati
 *	@variable			wave_freq = frequenza dell' "onda parabolica" su cui vengono raccolti 
 *									i campioni
 *	@variable			amplitude = ampiezza dell'onda
 */
typedef struct
{
	char *jointName;
	int sockfd;
	unsigned int sampling_rate;
	double wave_freq;
	double amplitude;
}ClientParameter;

/*************************** FUNZIONI PER GESTIRE GRADEFUL DEGRADATION ***************************/

/*
 * 	readstop: 		Funzione che legge in mutua esclusione una variabile globale utilizzata 
 *					per notificare lo stop a tutte le thread
 *	@return			la funzione restituisce il valore della variabile globale stop
*/
int readstop();

/*************************** FUNZIONI PER CONNESSIONE CLIENT TO SERVER ***************************/

/*
 *	get_in_addr: 			Funzione che restituisce l'indirizzo internet, IPv4 o IPv6, contenuto nella struttura passata nei parametri.
 *	@param	*sockaddr	puntatore alla struttura sockaddr 
 *	@return				la funzione ritorna un indirizzo IPv4 o IPv6.
 *	Credits: http://beej.us/guide/bgnet/examples/server.c (Beej's Guide to Network Programming)
 */
void *get_in_addr(struct sockaddr *sa);

/* 
 *	clientConnection: 	Funzione che stabilisce una connessione client to server
 *	@param	*hostname	indirizzo del server a cui ci si vuole connettere
 *	@param	*port		nome della porta a cui ci si vuole connettere
 *	@return				restituisce il socket file descriptor sockfd della connessione stabilita con il server
 *	Credits: http://beej.us/guide/bgnet/examples/client.c (Beej's Guide to Network Programming)
*/
int clientConnection(char *hostname, char *port);

/*************************** FUNZIONI DI SINTESI GESTIONE DELL'ERRORE ***************************/

/*
 * emalloc:		Funzione che alloca dinamicamente n byte di memoria e gestisce l'errore in caso di fallimento della funzione malloc.
 * @param  		n = numero di byte da allocare dinamicamente
 * @return      se il metodo malloc ha successo, restituisce un puntatore di tipo void alla memoria allocata dinamicamente
 *              se il metodo malloc fallisce, lancia l'eccezione di errore
 */
void *emalloc( size_t n );

/*
 * epthread_mutex_lock:		funzione per abbreviare ad una riga la gestione dell'errore alla chiamata
 *							della funzione pthread_mutex_lock.
 * @param  					*mutex = puntatore al mutex di cui si vuole fare lock per la mutua esclusione
 */
void epthread_mutex_lock( pthread_mutex_t *mutex );

/*
 * epthread_mutex_unlock:		funzione per abbreviare ad una riga la gestione dell'errore alla chiamata
 *							della funzione pthread_mutex_unlock.
 * @param  					*mutex = puntatore al mutex di cui si vuole fare unlock per la mutua esclusione
 */
void epthread_mutex_unlock( pthread_mutex_t *mutex );

/*
 * epthread_cond_signal:	funzione per abbreviare ad una riga la gestione dell'errore alla chiamata
 *							della funzione pthread_cond_signal.
 * @param  					*cond = variabile condizione soggetto dell'operazione di signal
 */
void epthread_cond_signal(pthread_cond_t *cond);

/*
 * epthread_cond_wait:		funzione per abbreviare ad una riga la gestione dell'errore alla chiamata
 *							della funzione pthread_cond_wait.
 * @param  					*cond = variabile condizione soggetto dell'operazione di signal
 * @param  					*mutex = puntatore al mutex in cui è protetta l'operazione di wait
 */
void epthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

#endif
