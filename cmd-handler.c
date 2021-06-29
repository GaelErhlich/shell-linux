#include "commands/handler.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "core/job.h"
#include "datastructures/job.h"

#include "utils/stringUtils.h"

#define PROCHAIN_TOKEN_FICHIER 0
#define PROCHAIN_TOKEN_SYMBOLE 1
#define PROCHAIN_TOKEN_ENTREE 2
#define PROCHAIN_TOKEN_SORTIE 3

#define MAX_ARGS_CMD 64

int handleCommand(char *cmd) {

  // Variables de boucle
  char *args[MAX_ARGS_CMD];
  int nbArgs;
  int result = splitStringCmd(cmd, args, &nbArgs);
  if (result == -1) {
    return -1;
  }
  int prochain = PROCHAIN_TOKEN_FICHIER;

  struct process **processus = nouvProcess(32);
  int iProc = 0;

  // Variables job
  struct job j;
  int isForeground = 1; // Par défaut, on met la commande au premier plan
  j.command = "";
  j.next = NULL;
  j.notified = 0;
  int idesc = STDIN_FILENO;  // Entrée
  int odesc = STDOUT_FILENO; // Sortie
  j.stdin = idesc;
  j.stdout = odesc;

  for (int i = 0; i < nbArgs; i++) {

    // ----------------------------
    //	Entr�e du flux
    // ----------------------------
    if (args[i][0] == '<') {
      // printf("Entr�e : %s\n", args[i]);
      if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
        perror("Erreur syntaxe : '<' ne peut pas être placé ici.\n");
        return -1;
      }

      prochain = PROCHAIN_TOKEN_ENTREE;
    }

    // ----------------------------
    //	Sortie du flux
    // ----------------------------
    else if (args[i][0] == '>') {
      // printf("Sortie : %s\n", args[i]);
      if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
        perror("Erreur syntaxe : '>' ne peut pas �tre plac� ici.\n");
        return -1;
      }

      prochain = PROCHAIN_TOKEN_SORTIE;
    }

    // ----------------------------
    //	S�parateur de programmes
    // ----------------------------
    else if (args[i][0] == '|') {
      // printf("Séparateur : %s\n", args[i]);
      if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
        perror("Erreur syntaxe : '|' ne peut pas �tre plac� ici.\n");
        return -1;
      }
      prochain = PROCHAIN_TOKEN_FICHIER;
    }

    // ----------------------------
    //	Inverseur de plan (switch foreground/background)
    // ----------------------------
    else if (args[i][0] == '*' && strlen(args[i]) == 1) {
      // printf("Foreground : %s\n", args[i]);
      isForeground = !isForeground;
    }

    // ----------------------------
    //	Fichier
    // ----------------------------
    else {
      // printf("Fichier : %s\n", args[i]);
      switch (prochain) {

      // Si l'entrée est attendue
      case PROCHAIN_TOKEN_ENTREE:;
        idesc = open(args[i], O_RDONLY);
        if (idesc == -1) {
          perror("Le fichier d'entree n'a pas ete trouve.\n");
          return -1;
        }

        j.stdin = idesc;
        prochain = PROCHAIN_TOKEN_SYMBOLE;
        break;

      // Si la sortie est attendue
      case PROCHAIN_TOKEN_SORTIE:;
        odesc = open(args[i], O_WRONLY | O_CREAT | O_EXCL, 0666);
        if (odesc == -1) {
          perror("Une erreur s'est produite lors de l'ouverture du flux de "
                 "sortie.\n");
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
          processus[iProc - 1]->next = processus[iProc];
        }

        char motSlash[32] = "./";
        strcat(motSlash, args[i]);
        char *tabMotPtr[33];
        int lenTab;
        splitStringCmd(motSlash, tabMotPtr, &lenTab);
        tabMotPtr[lenTab] = NULL;

        processus[iProc]->argv = tabMotPtr;
        processus[iProc]->completed = 0;
        processus[iProc]->stopped = 0;

        iProc++;
        prochain = PROCHAIN_TOKEN_SYMBOLE;

        break;

      default:
        perror("Un fichier n'était pas attendu ici.\n");
        return -1;
        break;
      }
    }
  }

  // ----------------------------
  //	V�rification rien en attente
  // ----------------------------
  if (prochain != PROCHAIN_TOKEN_SYMBOLE) {
    perror("La commande ne peut pas finir par un symbole.\n");
    return -1;
  }

  // Exécution du programme
  if (!isForeground && (j.stdin == STDIN_FILENO || j.stdout == STDOUT_FILENO)) {
    perror("Vous avez demande une execution en mode background, mais n'avez "
           "defini ni entree ni sortie.");
    return -1;
  }
  launch_job(&j, isForeground);

  return 0;
}
