#include "./utils/stringUtils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
	Divise une commande (cmd) en un tableau d'arguments (args, de taille definie a l'avance)
	Renvoie 0 si succes et -1 si syntaxe incorrecte
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

				if (mot[strlen(mot) - 1] == '"') { // Si le mot finit deja par un guillemet
					mot[strlen(mot) - 1] = '\0';
					args[i] = malloc(sizeof(char) * strlen(mot));
					strcpy(args[i], mot);
					i++;
				}
				else {
					strcpy(argTemp, mot);
					isQuoteOpen = 1;
				}
				mot--;

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

	len[0] = i;
	free(mot);

	if (isQuoteOpen) {
		printf("L'argument %s ne se termine jamais (par un guillemet).\n", argTemp);
		return -1;
	}
	return 0;
}