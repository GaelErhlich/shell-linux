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
	
	char* motPtr;
	motPtr = strtok(cmd, " ");
	int prochain = PROCHAIN_TOKEN_FICHIER;
	int i = 0;

	struct job j;
	int idesc = 0;
	int odesc = 0;


	while (motPtr != NULL) {
		//printf("'%s'\n", motPtr);


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

				case PROCHAIN_TOKEN_ENTREE:;
					idesc = open(motPtr, O_RDONLY);
					if (idesc == -1) {
						perror("Le fichier d'entree n'a pas ete trouve.\n");
						return -1;
					}
					
					j.stdin = idesc;
					break;


				case PROCHAIN_TOKEN_SORTIE:;
					odesc = open(motPtr, O_WRONLY | O_CREAT | O_EXCL, 0666);
					if (odesc == -1) {
						perror("Une erreur s'est produite lors de l'ouverture du flux de sortie.\n");
						return -1;
					}

					j.stdout = odesc;
					break;


				case PROCHAIN_TOKEN_FICHIER:;
					struct process* process = nouvProcess();
					j.first_process = process;
					j.command = "";
					j.next = NULL;
					
					j.notified = 0;

					printf("Avant création motSlash et tabMotPtr\n");
					char* tabMotPtr[2];
					char motSlash[32] = "./";
					printf("Avant concat\n");
					strcat(motSlash, motPtr);
					printf("Après concat\n");
					tabMotPtr[0] = motSlash;
					tabMotPtr[1] = NULL;
					printf("Après remplissage tableau\n");
					process->argv = tabMotPtr;
					process->completed = 0;
					process->stopped = 0;

					launch_job(&j, 1);
					// TODO
					
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



	return 0;
}
