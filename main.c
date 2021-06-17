#include "./commands/handler.c"
#include "./core/init.c"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void main() {
  init_shell();
  char cmd[30];
  char args[1024];

  while (1) {
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
    }
    else {
        handleCommand(args);
    }
  }
}