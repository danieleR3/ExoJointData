/*
 *	exoJointData_threadStop.c
 *	
 *	@description	thread per la gestione della gradeful degradation. Attraverso
 *					una variabile globale, notifica alle altre thread di chiudere,
 * 					deallocare le risorse e di fermarsi.
 *
 *  @author			Daniele Ronzani - Studente del Corso Magistrale in Ingegneria
 *										Meccatronica			
 *	@date			09/01/2015
 *  @version		1.0	
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "exoJointData_global.h"
#include "exoJointData_thread.h"
#include "exoJointData_auxFunction.h"

/*///////////////// FUNZIONI PER GESTIRE GRADEFUL DEGRADATION ///////////////////*/
/*
 *	void stopThreads(int sig);
 *	
 *	@description	funzione che setta la variabile globale stop al valore 1,
 *					in modo che le altre thread , controllando ciclicamente tale
 *					variabile, siano allertate di chiudersi.
 *
 *  @param			sig = identificativo del segnale
 *
 */

void stopThreads(int sig)
{
	epthread_mutex_lock(&mtxStop);
    	stop = 1;
    epthread_mutex_unlock(&mtxStop);
}


/*/////////////////////// THREAD GESTIONE SIGINT /////////////////////////////*/
void* threadStopFunc(void *arg)
{	
	sigset_t sigmask;
	sigemptyset(&sigmask); /* initialize set */
  
    if (sigprocmask(0,0,&sigmask) == -1) { /* get the current signal mask */
    	printf("error in pthread_sigmask\n");
      	exit(EXIT_FAILURE);
  	}
    sigdelset(&sigmask, SIGINT); /* clear SIGSEGV flag */ //in questo modo la tread può ricevere SIGINT

	int s;

	s = pthread_sigmask(SIG_SETMASK, &sigmask, 0);
	if ( s != 0) {
      printf("error in pthread_sigmask\n");
      exit(EXIT_FAILURE);
    }
	
	struct sigaction stop;
  	sigemptyset(&stop.sa_mask);
  	stop.sa_handler = stopThreads;
  	stop.sa_flags = 0;
 
  	if (sigaction(SIGINT, &stop, NULL) == -1)
  	{
    	printf("ERROR: sigaction\n");
    	exit(EXIT_FAILURE);
  	}
	pause();
  	pthread_exit(0); 
}
