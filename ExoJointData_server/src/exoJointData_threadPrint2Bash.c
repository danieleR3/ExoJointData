/*
 *	exoJointData_threadPrint2Bash.c
 *	
 *	@description	thread che si occupa di visualizzare i dati del giunto
 *					indicato nel file di configurazione.
 *					I dati non vengono visualizzati con la frequenza reale
 *					ma sottocampionati alla frequenza 1/WAITTIME.
 *
 *  @param			nameJoint, nome del giunto di cui visualizzare i dati
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

void* threadPrint2Bash(void *arg) 
{
	char *nameJoint = (char *)arg;
	JointData tmpJointPkg;
	int i=0, nSample = 0;
	float sumValue = 0, avgValue;
	unsigned int startSec = 0;
	unsigned long int startNanoSec = 0, deltaSec; 	

	//ciclo di visualizzazione, si interrompe solo qualora
	// venga lanciato SIGINT
	while(!readstop())
	{
		epthread_mutex_lock(&mtx2Bash);
			epthread_cond_wait(&newPkg2Bash, &mtx2Bash);
			//salvo il dato su una variabile locale per
			//rilasciare subito il mutex 
			tmpJointPkg=pkg2Bash;
		epthread_mutex_unlock(&mtx2Bash);

		//memorizzo il tempo del primo dato
		if(i==0){
			printf("\nSEC NANOSEC VALUE data from %s\n", nameJoint);
			startSec = tmpJointPkg.sec;
			startNanoSec = tmpJointPkg.nanosec;
		}

		//calcolo l'intervallo di tempo
		deltaSec = (tmpJointPkg.sec*NANOSECONDS + tmpJointPkg.nanosec) - (startSec*NANOSECONDS + startNanoSec);
		sumValue = sumValue + tmpJointPkg.value;
		nSample++;
		i++;

		//se l'intervallo supera il periodo si sottocampionamento
		//richiesto, visualizzo.
		if(deltaSec >= WAITTIME){
			avgValue = sumValue / nSample;
			printf("%u %09lu %f %s\n", tmpJointPkg.sec, tmpJointPkg.nanosec, avgValue, nameJoint);
			startSec = tmpJointPkg.sec;			//imposto questo tempo come il primo dell'intervallo
			startNanoSec = tmpJointPkg.nanosec;
			sumValue = 0;
			nSample = 0;
		}

	}
  	return NULL; 
}
