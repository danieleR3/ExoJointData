/*
 *	exoJointData_thread.h
 *	
 *	@description	header che raccoglie le dichiarazioni delle funzioni principali
 *					delle thread inizializzate nel progetto ExoJointData
 *
 *  @author			Daniele Ronzani - Studente del Corso Magistrale in Ingegneria
 *										Meccatronica			
 *	@date			09/01/2015
 *  @version		1.0	
 *
 */

#ifndef __EJD_THREAD__
#define __EJD_THREAD__

/*
 * 	threadPkgSend
 *
 *	@description	thread che genera dati con il protocollo SEC NANOSEC VALUE,
 *					per simulare le letture da un sensore. E li invia attraverso
 *					l'uso di una SOCKET STREAM ad un server che riceve e memorizza
 *					i dati.	
 *	 
 * 	@param	*arg   	cltParam. Dato di tipo struc ClientParameter che contiene informazioni
 *					sul nome del giunto su cui è installato il client e parametri per la
 *					generazione dei dati.													 
 */
void* threadPkgSend(void *arg);

/*
 *	threadStopFunc
 *	@description	thread per la gestione della gradeful degradation. Attraverso
 *					una variabile globale, notifica alle altre thread di chiudere,
 * 					deallocare le risorse e di fermarsi.																						 
 */
void* threadStopFunc(void *arg);

#endif
