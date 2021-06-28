#include "commands/handler.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "datastructures/job.h"
#include "core/job.h"





#define PROCHAIN_TOKEN_FICHIER	0
#define PROCHAIN_TOKEN_SYMBOLE	1
#define PROCHAIN_TOKEN_ENTREE	2
#define PROCHAIN_TOKEN_SORTIE	3







int handleCommand(char* cmd) {
	size_t len = strlen(cmd);
	
	// Variables de boucle
	char* motPtr;
	motPtr = strtok(cmd, " ");
	int prochain = PROCHAIN_TOKEN_FICHIER;
	int i = 0;

	// Variables process
	/*/ temp
	struct process p0;
	struct process p1;
	// fin temp*/

	struct process** processus = nouvProcess(32);
	int iProc = 0;

	// Variables job
	struct job j;
	j.command = "";
	j.next = NULL;
	j.notified = 0;
	int idesc = 0; // Entrée
	int odesc = 0; // Sortie
	


	while (motPtr != NULL) {


		// ----------------------------
		//	Entr�e du flux
		// ----------------------------
		if (motPtr[0] == '<') {
			printf("Entr�e : %s\n", motPtr);
			if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
				perror("Erreur syntaxe : '<' ne peut pas être placé ici.\n");
				return -1;
			}

			prochain = PROCHAIN_TOKEN_ENTREE;
		}
		
		// ----------------------------
		//	Sortie du flux
		// ----------------------------
		else if (motPtr[0] == '>') {
			printf("Sortie : %s\n", motPtr);
			if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
				perror("Erreur syntaxe : '>' ne peut pas �tre plac� ici.\n");
				return -1;
			}

			prochain = PROCHAIN_TOKEN_SORTIE;
		}

		// ----------------------------
		//	S�parateur de programmes
		// ----------------------------
		else if (motPtr[0] == '|') {
			printf("Séparateur : %s\n", motPtr);
			if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
				perror("Erreur syntaxe : '|' ne peut pas �tre plac� ici.\n");
				return -1;
			}
			prochain = PROCHAIN_TOKEN_FICHIER;
		}

		// ----------------------------
		//	Fichier
		// ----------------------------
		else {
			printf("Fichier : %s\n", motPtr);
			switch (prochain)
			{

				// Si l'entrée est attendue
				case PROCHAIN_TOKEN_ENTREE:;
					idesc = open(motPtr, O_RDONLY);
					if (idesc == -1) {
						perror("Le fichier d'entree n'a pas ete trouve.\n");
						return -1;
					}
					
					j.stdin = idesc;
					break;


				// Si la sortie est attendue
				case PROCHAIN_TOKEN_SORTIE:;
					odesc = open(motPtr, O_WRONLY | O_CREAT | O_EXCL, 0666);
					if (odesc == -1) {
						perror("Une erreur s'est produite lors de l'ouverture du flux de sortie.\n");
						return -1;
					}

					j.stdout = odesc;
					break;


				// Si un nom de ficher est attendu
				case PROCHAIN_TOKEN_FICHIER:;
					
					if (iProc == 0) {
						j.first_process = processus[iProc];
					} else {
						processus[iProc-1]->next = processus[iProc];
					}
					

					/*/temp
					struct process* pPtr;

					if (iProc == 0) {
						pPtr = &p0;
						j.first_process = pPtr;
					}
					else if (iProc == 1) {
						pPtr = &p1;
						p0.next = pPtr;
					}
					else {
						printf("Erreur temp : trop de fichiers\n");
						break;
					}
					//fin temp*/
					

					char motSlash[32] = "./";
					printf("Avant concat\n");
					strcat(motSlash, motPtr);
					printf("Après concat\n");
					char* tabMotPtr[2] = {motSlash, NULL};
					tabMotPtr[0] = motSlash;
					tabMotPtr[1] = NULL;
					printf("Après remplissage tableau\n");
					
					processus[iProc]->argv = tabMotPtr;
					processus[iProc]->completed = 0;
					processus[iProc]->stopped = 0;
					/*/temp
					pPtr->argv = tabMotPtr;
					pPtr->completed = 0;
					pPtr->stopped = 0;
					//fin temp*/
					printf("Après remplissage processus (1 iteration)\n");

					iProc++;
					
					break;

				
			default:
				break;
			}


			prochain = PROCHAIN_TOKEN_SYMBOLE;
		}
		// ----------------------------
		//	Fin boucle
		// ----------------------------

		motPtr = strtok(NULL, " ");
		i++;
	}


	// ----------------------------
	//	V�rification rien en attente
	// ----------------------------
	if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
		perror("La commande ne peut pas finir par un symbole.\n");
		return -1;
	}

	// Exécution du programme
	launch_job(&j, 1);


	return 0;
}
