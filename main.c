#include "./commands/handler.h"
#include "./core/init.h"
#include "./core/job.h"
#include "./datastructures/job.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void testsStrtokToFullStringArg() {
  char cmd[128] = "Hier j'ai voulu manger une pomme de type \"pomme golden\" "
                  "et, c'est de mon point de vue exquis.";

  char *args[64];
  int nbArgs;
  splitStringCmd(cmd, args, &nbArgs);

  if (strcmp(args[0], "Hier")) {
    printf("Erreur : splitStringCmd aurait du renvoyer 'Hier'. (%s)\n",
           args[0]);
  }
  if (strcmp(args[8], "pomme golden")) {
    printf("Erreur : splitStringCmd aurait du renvoyer 'pomme golden'. (%s)\n",
           args[8]);
  }
  if (strcmp(args[9], "et,")) {
    printf("Erreur : splitStringCmd aurait du renvoyer 'et,'. (%s)\n", args[9]);
  }
  if (strcmp(args[16], "exquis.")) {
    printf("Erreur : splitStringCmd aurait du renvoyer 'exquis. (%s)'.\n",
           args[16]);
  }
}

int main(int argc, char *argv[]) {

  testsStrtokToFullStringArg();

  init_shell();

  while (1) {
    char cmd[32];
    char args[1024];
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      printf("%s$ ", cwd);
    }
    fscanf(stdin, "%s", cmd);
    fgets(args, 100, stdin);

    printf("Command Entered : %s \n", cmd);
    printf("Arguments entered : %s \n", args);
    size_t i = 1;
    while (1) {
      if (args[i] == 10) {
        break;
      } else {
        i++;
      }
    }
    while (i < 1024) {
      args[i] = 0;
      i++;
    }
    memcpy(args, args + 1, sizeof(args) - 1);
    char fullCmd[1024];
    strcat(fullCmd, cmd);
    char space[] = " ";
    strcat(fullCmd, space);
    strcat(fullCmd, args);

    if (!strcmp(cmd, "exit")) {
      exit(1);
    } else if (!strcmp(cmd, "cd")) {
      if (chdir(args) != 0) {
        perror("Failed");
      }
    } else if (!strcmp(cmd, "mkdir")) {
      if (mkdir(args, S_IRWXU) != 0) {
        perror("Failed");
      }
    } else if (!strcmp(cmd, "cp")) {
      char *argsSplit[3];
      int argsNum;
      splitStringCmd(fullCmd, argsSplit, &argsNum);
      maincp(argsNum, argsSplit);

    } else {
      handleCommand(fullCmd);
    }
  }

  return 0;
}