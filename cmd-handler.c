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
		//	Entrée du flux
		// ----------------------------
		if (motPtr[0] == '<') {
			printf("Entrée : %s\n", motPtr);
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
				perror("Erreur syntaxe : '>' ne peut pas être placé ici.\n");
				return -1;
			}
			prochain = PROCHAIN_TOKEN_SORTIE;
		}

		// ----------------------------
		//	Séparateur de programmes
		// ----------------------------
		else if (motPtr[0] == '|') {
			printf("Séparateur : %s\n", motPtr);
			if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
				perror("Erreur syntaxe : '|' ne peut pas être placé ici.\n");
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
				perror("Erreur syntaxe : un symbole était attendu ici ('<', '>', '|').\n");
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
	//	Vérification rien en attente
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
