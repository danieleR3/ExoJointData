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

#include "exoJointData_global.h"
#include "exoJointData_thread.h"
#include "exoJointData_auxFunction.h"

/*////////////////////////// FUNZIONI PER GESTIRE GRADEFUL DEGRADATION //////////////////////////*/

int readstop()
{
	int res;
	epthread_mutex_lock(&mtxStop);
    	res = stop;
    epthread_mutex_unlock(&mtxStop);
	return res;
}

/*////////////////////////// FUNZIONI PER CONNESSIONE CLIENT TO SERVER //////////////////////////*/

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int clientConnection(char *hostname, char *port)
{
	int sockfd, new_fd;  
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char server[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			server, sizeof server);

	printf("client: connecting to %s\n", server);

	freeaddrinfo(servinfo); // all done with this structure

	return sockfd;

}

/*///////////////////// FUNZIONI DI GESTIONE DELL'ERRORE ////////////////////////*/

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






