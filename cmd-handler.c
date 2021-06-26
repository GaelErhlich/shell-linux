#include "commands/handler.h"

#include <string.h>
#include <stdio.h>

#include "datastructures/job.h"





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


	while (motPtr != NULL) {
		//printf("'%s'\n", motPtr);


		// ----------------------------
		//	Entr�e du flux
		// ----------------------------
		if (motPtr[0] == '<') {
			printf("Entr�e : %s\n", motPtr);
			if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
				perror("Erreur syntaxe : '<' ne peut pas �tre plac� ici.\n");
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
			printf("S�parateur : %s\n", motPtr);
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
			if (prochain == PROCHAIN_TOKEN_SYMBOLE) {
				perror("Erreur syntaxe : un symbole �tait attendu ici ('<', '>', '|').\n");
				return -1;
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



	for (int i = 0; i < len; i++) {
		printf("- %c\n", cmd[i]);
	}

	return 0;
}
