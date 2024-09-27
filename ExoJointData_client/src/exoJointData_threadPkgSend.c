/*
 *	exoJointData_threadPkgSend.c
 *	
 *	@description	thread che genera dati con il protocollo SEC NANOSEC VALUE,
 *					per simulare le letture da un sensore. E li invia attraverso
 *					l'uso di una SOCKET STREAM ad un server che riceve e memorizza
 *					i dati.
 *  @author			Daniele Ronzani - Studente del Corso Magistrale in Ingegneria
 *										Meccatronica			
 *	@date			09/01/2015
 *  @version		1.0	
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "exoJointData_global.h"
#include "exoJointData_thread.h"
#include "exoJointData_auxFunction.h"

/************************** FUNZIONI PER LA GENERAZIONE DEI DATI **************************/
/*
 *	struct timeval getTime();
 *	
 *	@description	funzione che restituisce l'intervallo di tempo trascorso
 *					dalla prima chiamata alla funzione
 *
 *  @return			res di tipo struct timeval
 *  Credits: real_timer.c (Michael Kerrisk-The Linux Programming Interface)   
 */
// Display the ringing times
struct timeval getTime() {

    static struct timeval start;
    struct timeval curr, res;
    static int callNum = 0;             /* Number of calls to this function */

    if (callNum == 0)                /* Initialize elapsed time meter */
        if (gettimeofday(&start, NULL) == -1) {
            fprintf(stderr, "gettimeofday");
            exit(EXIT_FAILURE);
        }
    if (gettimeofday(&curr, NULL) == -1) {
        fprintf(stderr, "gettimeofday");
        exit(EXIT_FAILURE);
    }  
    callNum++;
	res.tv_sec = ((curr.tv_sec*MICROSECONDS+curr.tv_usec) - (start.tv_sec*MICROSECONDS+start.tv_usec))/MICROSECONDS;
	res.tv_usec =((curr.tv_sec*MICROSECONDS+curr.tv_usec) - (start.tv_sec*MICROSECONDS+start.tv_usec))%MICROSECONDS;
    return res;
}

/*
 *	void sigalrmHandler(int sig);
 *	
 *	@description	funzione di handler per il timer
 *  @param			sig = identificativo del segnale
 *  @return			ritorna al chiamante
 *
 */
void sigalrmHandler(int sig) {
    return; 
}

/************************** THREAD CLIENT PER GENERAZIONE E INVIO DATI AL SERVER ***************************/
void* threadPkgSend(void *arg) 
{
	//casto il puntatore ricevuto dalla thread alla variabile desiderata
	ClientParameter cltParam = *(ClientParameter *)arg;

	//imposto la maschera in modo tale che la thread riesca a ricevere
	//il segnale SIGALARM, necessario a produrre e inviare i campioni
	//con la frequenza stabilita
	sigset_t sigmask;
	sigemptyset(&sigmask); /* initialize set */
    if (sigprocmask(0,0,&sigmask) == -1) { /* get the current signal mask */
    	printf("error in pthread_sigmask\n");
      	exit(EXIT_FAILURE);
  	}
    sigdelset(&sigmask, SIGALRM); /* clear SIGALRM flag */
	int e;
	e = pthread_sigmask(SIG_SETMASK, &sigmask, 0);
	if ( e != 0) {
      printf("error in pthread_sigmask\n");
      exit(EXIT_FAILURE);
    }

	//imposto il timer e la relativa funzione di handler
	//che invia un segnale alla frequenza desiderata s
	double s = (double)cltParam.sampling_rate;
    // set the signal handler
    struct itimerval itv;
    struct sigaction sa; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigalrmHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction");
        exit(EXIT_FAILURE);
    }
	// Set timer: ring once every 1/s
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 1./s * MICROSECONDS;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 1./s * MICROSECONDS;
    if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
        fprintf(stderr, "setitimer");
        exit(EXIT_FAILURE);
    }


    // Set the parameters for the parabolic function
    double f = cltParam.wave_freq;
    double A = cltParam.amplitude;
    long m = s/(2.0*f);
    long a = -m;
    long b = 0;
    A*=4.0/(double)(m*m);
    double y = 0;
    long i = 0;

    struct timeval sampleTime;
	char *jointPkg=(char *)emalloc(MAXDATASIZE*sizeof(char));

	//variabile flag che fa uscire la thread quando il server
	//chiude la connessione
	int server_logout = 0;

	printf("client: sending joint package SEC NANOSEC VALUE from %s to server...\n", cltParam.jointName);

	//ciclo della thread che si arresta nel caso di:
	//	-arrivo del segnale SIGINT e notifica tramite 
	//   variabile globale stop 
	//	-server chiude la connessione o errore su send
    while(!readstop() && !server_logout){
        
        if( i%m == 0 ) {a+=m; b+=m; A=-A;}
        y = A*(i-a)*(i-b);
		i++;
		//misuro il tempo del campione dall'inizio della generazione
		sampleTime = getTime();

		//genero il pacchetto da inviare tramite socket 
		//utilizzando la codifica "# SEC NANOSEC VALUE \n"
		//in modo da essere opportunamente interpretato dal server
		sprintf(jointPkg, "#%ld %09ld %6.4f\n", sampleTime.tv_sec, sampleTime.tv_usec*1000, y);
/*<DEBUG/>*/	if(DEBUG)printf("%s", jointPkg);
		// send the information through socket
		if (send(cltParam.sockfd, jointPkg, strlen(jointPkg), 0) == -1){
				printf("client: server disconnected\n");
				server_logout = 1;
				printf("client: Press CTRL+C to EXIT\n");
		}
		//aspetto il prossimo istante di campionamento
		//notificato dall'arrivo del segnale SIGALRM
		//impostato con il timer
        pause();
    }

	//prima di terminare la thread libero le risorse
	free(jointPkg);
	close(cltParam.sockfd);
  return NULL; 
}
