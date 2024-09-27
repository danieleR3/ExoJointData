/*
 *	exoJointData_auxFunction.c
 *	
 *	@description	sorgente che raccoglie le funzioni ausiliarie in uso nel 
 *					progetto ExoJointData e condivise con più sorgenti
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
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "exoJointData_auxFunction.h"
#include "exoJointData_global.h"


/************** FUNZIONI PER LA LETTURA DEL FILE DI CONFIGURAZIONE **************/

int blanckSpace(FILE *fin, int c)
{
   int bs;
   bs = ( c=='\t'  || c==' ' || endOfLine(fin, c));  //se c è uno spazio bianco o un carattere di fine linea bs = 1 altrimenti bs = 0
   return bs;
}

int endOfLine(FILE *fin, int c)
{
   int eol;
   eol = (c=='\r' || c=='\n');
   if (c == '\r')
   {
      c = getc(fin);
      if (c != '\n' && c != EOF)
         ungetc(c, fin);
   }
   return eol; //eol = 1 se c è un carattere di fine linea, altrimenti eol = 0
}

Configuration createConf(unsigned char *joint1, unsigned char *joint2, unsigned int freqInterp, unsigned char *nameLogFile, unsigned char *jointToBash)
{
    Configuration conf;
    if( &joint1 == NULL)
    {
        conf.joint1 = NULL;
        conf.joint2 = NULL;
        conf.freqInterp = 0;
	conf.nameLogFile = NULL;
	conf.jointToBash= NULL;
    }
    else
    {
        conf.joint1 = joint1;
        conf.joint2 = joint2;
        conf.freqInterp = freqInterp;
	conf.nameLogFile = nameLogFile;
	conf.jointToBash= jointToBash;
    }
    return conf;
}

void releaseConf(Configuration conf)
{
        if(conf.joint1 != NULL)
            free(conf.joint1);
	if(conf.joint2 != NULL)
            free(conf.joint2);
	if(conf.nameLogFile != NULL)
            free(conf.nameLogFile);
	if(conf.jointToBash != NULL)
            free(conf.jointToBash);
}

Configuration loadConf(char *filename)
{
    FILE *f;
    int conf_param = JOINT1, freqInterp=0, n;
    char c, *joint1=emalloc(6), *joint2=emalloc(6), *nameLogFile = emalloc(30), *jointToBash =emalloc(6);
    //joint2 = NULL;
	//jointToBash = NULL;

	Configuration conf;
    if(NULL == (f = fopen(filename,"r")))
    {
       fprintf(stderr,"fopen() failed!\n\n");
       exit(EXIT_FAILURE);
    }
    
    while((c=getc(f))!=EOF)
    {
		if(endOfLine(f, c) && conf_param == JOINT2){
			joint2=NULL;
			conf_param++;
		}
        else if(blanckSpace(f,c));          //se incontri uno "spazio bianco" prosegui
        else                                //se non c'è un commento analizza i dati
        {
            ungetc(c,f);
            switch (conf_param)				//conf_param indica l'informazione da analizzare
            {
                case JOINT1:            
                    if((n=fscanf(f,"%s", joint1))==1 && (strcmp(joint1,"knee")==0 || strcmp(joint1,"ankle")==0))                 
                        conf_param++;
                    else
                    {
                        fprintf(stderr,"File configurazione non conforme, JOINT1 incorretto.\n\n");
                        return createConf(NULL, NULL, 0, NULL, NULL);
                    }
                    break;
				case JOINT2:         
                    if((n=fscanf(f,"%s", joint2))==1 && (strcmp(joint2,"knee")==0 || strcmp(joint2,"ankle")==0) && strcmp(joint2, joint1)!=0)
						conf_param++;
                    else
                    {
                        fprintf(stderr,"File configurazione non conforme, JOINT2 incorretto.\n\n");
                        return createConf(NULL, NULL, 0, NULL, NULL);
                    }
                    break;
                case FREQ_INTERP:       
                    if((n=fscanf(f, "%d", &freqInterp))==1 && freqInterp > 0)
						conf_param++;
                    else{                    
                        fprintf(stderr,"File pgm non conforme, FREQ_INTERP incorretta.\n\n");
                        return createConf(NULL, NULL, 0, NULL, NULL);
                    } 
                    break;
                case NAME_LOG_FILE:                   
                    if((n=fscanf(f,"%s", nameLogFile))==1)
						conf_param++;
		    		else{
                        fprintf(stderr,"File configurazione non conforme, NAME_LOG_FILE incorretto.\n\n");
                        return createConf(NULL, NULL, 0, NULL, NULL);
                    } 
                    break;
                case JOINT2VIDEO:                   
                    if((n=fscanf(f,"%s", jointToBash))==1 && (strcmp(jointToBash,joint1)==0 || (joint2 != NULL && strcmp(jointToBash,joint2)==0)))
						conf_param++;
                    else
                    {
                        fprintf(stderr,"File configurazione non conforme, JOINT_TO_VIDEO incorretto.\n\n");
                        return createConf(NULL, NULL, 0, NULL, NULL);
                    }
                    break;
            }
        }
    }
	if(conf_param <= JOINT2VIDEO){
    	fprintf(stderr,"File configurazione non conforme.\n\n");
    	return createConf(NULL, NULL, 0, NULL, NULL);
    }
    conf = createConf(joint1, joint2, freqInterp, nameLogFile, jointToBash);
    fclose(f);  
    return conf; 
}

/***************** FUNZIONI PER GESTIRE GRADEFUL DEGRADATION ******************/

int readstop()
{
	int res;
	epthread_mutex_lock(&mtxStop);
    	res = stop;
    epthread_mutex_unlock(&mtxStop);
	return res;
}

void emptyPkgBuf(char *nameLogFile)
{

	int headBuf[2], offsetBuf, i, buf_id;
	FILE *f;

	//apro il file log in scrittura
	if(NULL == (f = fopen(nameLogFile,"a")))
    {
        fprintf(stderr,"fopen() failed!\n\n");
        exit(EXIT_FAILURE);
    }
	//setto le variabili che indicano il primo elemento
	//del buffer non ancora salvato
	headBuf[ANKLE]=0;
	headBuf[KNEE]=0;

	if(nPkgBuf[ANKLE]!=0 && nPkgBuf[KNEE]!=0){

//		//mutua esclusione sul buffer ANKLE
		epthread_mutex_lock(&mtxJointBuf[ANKLE]);
	//		//mutua esclusione sul buffer ANKLE
			epthread_mutex_lock(&mtxJointBuf[KNEE]);

				//se uno dei buffer è vuoto e l'altro pieno significa che ho superato 
				//il ritardo massimo per la ricezione dei dati da uno dei giunti
				if(nPkgBuf[ANKLE]==0 || nPkgBuf[KNEE]==0){
					printf("error: client not respond\n");
		  			exit(EXIT_FAILURE);		
				}
				//qui si sfrutta l'ipotesi che i dati arrivano con l'ordine di invio le code sono dunque
				//già ordinate temporalmente, il dato più vecchio è sempre nella testa del buffer headBuf[i] 
				//questo ciclo confronta le teste dei buffer e scrive la più vecchia
				//il dato scritto nel file viene "eliminato" facendo scorrere l'indice headbuf[]
				//il ciclo si ferma quando uno dei due buffer è stato completamente scritto su file headBuf[i] == nPkgBuf[i] 
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
	//
		epthread_mutex_unlock(&mtxJointBuf[ANKLE]);
//
	}
	if(nPkgBuf[ANKLE] == 0)
		for(i = 0; nPkgBuf[KNEE] > 0; i++){
			fprintf(f, "%u %09lu %f knee\n", jointBuf[KNEE][i].sec, jointBuf[KNEE][i].nanosec, jointBuf[KNEE][i].value);
			nPkgBuf[KNEE]--;
		}
	else
		for(i = 0; nPkgBuf[ANKLE] > 0; i++){
			fprintf(f, "%u %09lu %f ankle\n", jointBuf[ANKLE][i].sec, jointBuf[ANKLE][i].nanosec, jointBuf[ANKLE][i].value);
			nPkgBuf[ANKLE]--;
		}
	fclose(f); 
}

/****************** FUNZIONI PER CONNESSIONE SERVER TO CLIENT *******************/

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int socketCreation(char *port)
{
	int sockfd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	int yes=1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	
	return sockfd;
}

/****************** FUNZIONI PER RICEVERE I PACCHETTI DI DATI *******************/

int recvJointPkg(int sockfd, char* jointPkg)
{
	int endPkg=0, numbytes;
	char buf[1];
	strcpy(jointPkg, "");
	if ((numbytes = recv(sockfd, buf, 1, MSG_WAITALL)) == -1) {
  		perror("recv");
  		exit(1);
	}

	if(strcmp(buf,"#")!=0 && numbytes == 0)
   		return 0;	
	else if(strcmp(buf,"#")!=0)
   		return -1;

	while(endPkg!=1)
	{
		if ((numbytes = recv(sockfd, buf, 1, MSG_WAITALL)) == -1) {
 			perror("recv");
  			exit(1);
		}
		if(strcmp(buf,"\n")==0)
	    	endPkg = 1;
		else
			strcat(jointPkg, buf);
	}
	return strlen(jointPkg);
}

/******************** FUNZIONI DI GESTIONE DELL'ERRORE *********************/

void *emalloc( size_t n )
{
   void *p = malloc(n);
   if( p == NULL )
   {
      fprintf(stderr,"malloc(%lu) failed! Exit\n",n);
      exit(EXIT_FAILURE);
   }
   return p;
}

void epthread_mutex_lock( pthread_mutex_t *mutex )
{
   	if (pthread_mutex_lock(mutex) != 0) {
   		printf("error in pthread_mutex_lock\n");
     	exit(EXIT_FAILURE);
   	}
}

void epthread_mutex_unlock( pthread_mutex_t *mutex )
{
	if (pthread_mutex_unlock(mutex) != 0) {
		printf("error in pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
   }
}

void epthread_cond_signal(pthread_cond_t *cond)
{
	if ( pthread_cond_signal(cond) != 0) {
		printf("error in pthread_cond_signal\n");
		exit(EXIT_FAILURE);
   }
}

void epthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	if ( pthread_cond_wait(cond, mutex) != 0) {
		printf("error in pthread_cond_wait\n");
		exit(EXIT_FAILURE);
   }
}
