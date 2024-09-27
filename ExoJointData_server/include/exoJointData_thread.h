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
 *	threadPkgRecv
 *	@description	thread che riceve dati con il protocollo SEC NANOSEC VALUE,
 *					attraverso una connessione di tipo SOCKET STREAM. 
 *					Quando è connesso un singolo giunto i dati vengono direttamente
 *					scritti in ordine temporale su un file di log.
 *					QUando sono presenti due giunti, i dati vengono memorizzati
 *					in due buffer. Un uteriore thread si occuperà di leggere da
 *					questi e salvarli sul file di log. 
 *  @param			srvParam, struttura di tipo ServerParameter che contiene
 *					informazioni sulla configurazione del sistema.																	 
 */
void* threadPkgRecv(void *arg);

/*
 *	threadPkgSave	
 *	@description	thread che si occupa di salvare e ordinare
					i dati di due giunti, salvati su due buffer differenti 
 *  @param			nameLogFile, path e nome del file log su cui salvare i dati																	 
 */
void* threadPkgSave(void *arg);

/*
 *	threadPrint2Bash
 *	@description	thread che si occupa di visualizzare i dati del giunto
 *					indicato nel file di configurazione.
 *					I dati non vengono visualizzati con la frequenza reale
 *					ma sottocampionati alla frequenza 1/WAITTIME.
 *
 *  @param			nameJoint, nome del giunto di cui visualizzare i dati																		 
 */
void* threadPrint2Bash(void *arg);

/*
 *	threadStopFunc
 *	@description	thread per la gestione della gradeful degradation. Attraverso
 *					una variabile globale, notifica alle altre thread di chiudere,
 * 					deallocare le risorse e di fermarsi.																						 
 */
void* threadStopFunc(void *arg);

#endif
