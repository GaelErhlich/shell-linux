#include "./commands/cp.h"
#include "./commands/handler.h"
#include "./core/init.h"
#include "./core/job.h"
#include "./datastructures/job.h"
#include "./utils/stringUtils.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  init_shell();

  while (1) {
    char cmd[32];
    strcpy(cmd, "");
    char args[1024];
    strcpy(args, "");
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      printf("%s$ ", cwd);
    }
    fscanf(stdin, "%s", cmd);
    fgets(args, 100, stdin);
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
    strcpy(fullCmd, "");
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