#include "commands/handler.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "datastructures/job.h"
#include "core/job.h"





#define PROCHAIN_TOKEN_FICHIER		0
#define PROCHAIN_TOKEN_SYMBOLE		1
#define PROCHAIN_TOKEN_ENTREE		2
#define PROCHAIN_TOKEN_SORTIE		3
#define PROCHAIN_TOKEN_SYMBouARG	4





/*
	Divise une commande (cmd) en un tableau d'arguments (args, de taille definie a l'avance)
*/

int splitStringCmd(char* cmd, char** args, int* len) {
	char* mot = malloc(sizeof(char) * 2048);
	char argTemp[4096];
	int i = 0;

	int isQuoteOpen = 0;
	mot = strtok(cmd, " ");

	while (mot != NULL) {
		if (!isQuoteOpen) {
			if (mot[0] == '"') { // Si l'argument commence par un guillemet, on va laisser les mots s'accumuler
				mot++; // Suppression du premier char (le guillemet)
				strcpy(argTemp, mot);
				mot--;
				isQuoteOpen = 1;
			}
			else { // Si l'argument ne commence pas par un guillemet, alors on le renvoie directement
				args[i] = malloc(sizeof(char) * strlen(mot));
				strcpy(args[i], mot);
				i++;
			}
		}

		// Si on n'a pas renvoyé, on vérifie qu'on n'a pas un guillemet de fin et on ajoute
		else {
			strcat(argTemp, " ");
			strcat(argTemp, mot);

			if (mot[strlen(mot) - 1] == '"') {
				argTemp[strlen(argTemp) - 1] = '\0';
				args[i] = malloc(sizeof(char) * strlen(argTemp));
				strcpy(args[i], argTemp);
				isQuoteOpen = 0;
				i++;
			}
		}

		mot = strtok(NULL, " ");
	}

	i++;
	len[0] = i;
	free(mot);

	if (isQuoteOpen) {
		printf("L'argument %s ne se termine jamais (par un guillemet).\n", argTemp);
		return -1;
	}
	return 0;
}





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
	int idesc = STDIN_FILENO; // Entrée
	int odesc = STDOUT_FILENO; // Sortie
	j.stdin = idesc;
	j.stdout = odesc;
	


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
					prochain = PROCHAIN_TOKEN_SYMBOLE;
					break;


				// Si la sortie est attendue
				case PROCHAIN_TOKEN_SORTIE:;
					odesc = open(motPtr, O_WRONLY | O_CREAT | O_EXCL, 0666);
					if (odesc == -1) {
						perror("Une erreur s'est produite lors de l'ouverture du flux de sortie.\n");
						return -1;
					}

					j.stdout = odesc;
					prochain = PROCHAIN_TOKEN_SYMBOLE;
					break;


				// Si un nom de ficher est attendu
				case PROCHAIN_TOKEN_FICHIER:;
					
					if (iProc == 0) {
						j.first_process = processus[iProc];
					} else {
						processus[iProc-1]->next = processus[iProc];
					}

					char motSlash[32] = "./";
					printf("Avant concat\n");
					strcat(motSlash, motPtr);
					printf("Après concat\n");
					char* tabMotPtr[16];
					tabMotPtr[0] = motSlash;
					tabMotPtr[1] = "moi";
					tabMotPtr[2] = NULL;
					printf("Après remplissage tableau\n");
					
					processus[iProc]->argv = tabMotPtr;
					processus[iProc]->completed = 0;
					processus[iProc]->stopped = 0;
					printf("Après remplissage processus (1 iteration)\n");

					iProc++;
					prochain = PROCHAIN_TOKEN_SYMBOLE; //TODO Remplacer
					
					break;


				
				// Si cet argument n'est pas un symbole reconnu, on le considère comme argument de la dernière commande
				case PROCHAIN_TOKEN_SYMBouARG:
					break;



				
			default:
				perror("Un fichier n'était pas attendu ici.\n");
				break;
			}
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
