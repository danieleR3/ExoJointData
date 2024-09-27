/*
 *	exoJointData_threadPkgSave.c
 *	
 *	@description	thread che si occupa di salvare e ordinare
 *					i dati di due giunti, salvati su due buffer differenti 
 *  @param			nameLogFile, path e nome del file log su cui salvare i dati
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
#include <string.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "exoJointData_auxFunction.h"
#include "exoJointData_thread.h"
#include "exoJointData_global.h"

void* threadPkgSave(void *arg) 
{
	char *nameLogFile =(char *)arg;
	int headBuf[2], offsetBuf, i;
	FILE *f;
	
	//ciclo di salvataggio, si interrompe solo qualora
	// venga lanciato SIGINT
	while(!readstop()){

		//setto le variabili che indicano il primo elemento
		//del buffer non ancora salvato
		headBuf[ANKLE]=0;
		headBuf[KNEE]=0;

		//mutex per variabile condizione sul salvataggio
		epthread_mutex_lock(&mtxSave);
			//attendo che la thread che riceve i dati riempia un buffer
			epthread_cond_wait(&okSave, &mtxSave);
		epthread_mutex_unlock(&mtxSave);

/*DEBUG*/	if(DEBUG) printf("server: save\n");

		//apro il file di log in modalità di accumulo
		if(NULL == (f = fopen(nameLogFile,"a")))
    	{
        	fprintf(stderr,"fopen() failed!\n\n");
        	exit(EXIT_FAILURE);
    	}

		//mutua esclusione sul buffer ANKLE
		epthread_mutex_lock(&mtxJointBuf[ANKLE]);
			//mutua esclusione sul buffer KNEE
			epthread_mutex_lock(&mtxJointBuf[KNEE]);
				
				//qui si sfrutta l'ipotesi che i dati arrivano con l'ordine di invio, le code sono dunque
				//già ordinate temporalmente, il dato più vecchio è sempre nella testa del buffer headBuf[i]. 
				//questo ciclo confronta le teste dei buffer e scrive la più vecchia
				//il dato scritto nel file viene "eliminato" facendo scorrere l'indice headbuf[]
				//il ciclo si ferma quando uno dei due buffer è stato completamente scritto su file, headBuf[i] == nPkgBuf[i] 
				while(headBuf[ANKLE]<nPkgBuf[ANKLE] && headBuf[KNEE]<nPkgBuf[KNEE]){ 
					if((jointBuf[ANKLE][headBuf[ANKLE]].sec < jointBuf[KNEE][headBuf[KNEE]].sec) || 
							((jointBuf[ANKLE][headBuf[ANKLE]].sec == jointBuf[KNEE][headBuf[KNEE]].sec) && 
								(jointBuf[ANKLE][headBuf[ANKLE]].nanosec <= jointBuf[KNEE][headBuf[KNEE]].nanosec))){

						fprintf(f, "%u %09lu %f ankle\n", jointBuf[ANKLE][headBuf[ANKLE]].sec, 
								jointBuf[ANKLE][headBuf[ANKLE]].nanosec, jointBuf[ANKLE][headBuf[ANKLE]].value );
						headBuf[ANKLE]++;
					}
					else{
						fprintf(f, "%u %09lu %f knee\n", jointBuf[KNEE][headBuf[KNEE]].sec, 
								jointBuf[KNEE][headBuf[KNEE]].nanosec, jointBuf[KNEE][headBuf[KNEE]].value );
						headBuf[KNEE]++;								
					}
					
				}
				
				//individuo il buffer che non è stato svuotato
				if(headBuf[ANKLE] == nPkgBuf[ANKLE])	
					offsetBuf = KNEE;
				else
					offsetBuf = ANKLE;
				i = headBuf[offsetBuf];

				//faccio scorrere gli elementi non ancora scritti all'inizio del buffer (sono i più vecchi)
				//la thread di ricezione potrà dunque scrivere dopo questi elementi mantenendo così il buffer
				//ordinato temporalmente
				while(i<nPkgBuf[offsetBuf]){
					jointBuf[offsetBuf][i-headBuf[offsetBuf]] =	jointBuf[offsetBuf][i];
					i++;
				}
				nPkgBuf[ANKLE] = nPkgBuf[ANKLE] - headBuf[ANKLE];
				nPkgBuf[KNEE] = nPkgBuf[KNEE] - headBuf[KNEE];

			epthread_mutex_unlock(&mtxJointBuf[KNEE]);
		epthread_mutex_unlock(&mtxJointBuf[ANKLE]);

		for(i=0;i<2;i++){
			//mutex per variabile condizione per continuare a ricevere dopo il salvataggio
			epthread_mutex_lock(&mtxRecv[i]);

				epthread_cond_signal(&okRecv);

				saveStarted--;
			epthread_mutex_unlock(&mtxRecv[i]);
		}
	fclose(f);
	}

/*DEBUG*/	if(DEBUG) printf("server: save end\n");

/*DEBUG*/	if(DEBUG) printf("THREAD SAVE RETURNED\n");
  	return NULL; 
}
