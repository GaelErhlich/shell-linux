#include "./cp.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

void cp(char *source, char *destination, struct stat src_stat);
void copy(int argc, char *argv[], bool recursive);
void recur(char *source, char *destination);
int maincp(int argc, char *argv[]);

void cp(char *source, char *destination, struct stat src_stat) {
  int src, dst;
  int numbytesread;
  char buf[MAX_BUF_SIZE];
  src = open(source, O_RDONLY);
  dst = open(destination, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
  while ((numbytesread = read(src, buf, MAX_BUF_SIZE)) > 0) {
    write(dst, buf, numbytesread);
  }
  close(src);
  close(dst);
}

void copy(int argc, char *argv[], bool recursive) {
  struct stat src_stat, dst_stat;
  bool is_dir = false;
  if (lstat(argv[argc - 1], &dst_stat) == -1) {

  } else {
    if (S_ISDIR(dst_stat.st_mode)) {
      is_dir = true;
    }
  }
  for (int i = 0; i < argc - 1; i++) {
    char *source = argv[i];
    char *destination;
    lstat(source, &src_stat);
    if (S_ISDIR(src_stat.st_mode)) {
      if (recursive) {
        recur(source, argv[argc - 1]);
        continue;
      }
    }
    if (is_dir) {
      char *src_filename = basename(source);
      char *dir_name = argv[argc - 1];
      char path[strlen(dir_name) + 1 + strlen(src_filename)];
      char *slash = (dir_name[strlen(dir_name) - 1] == '/') ? "" : "/";
      strcpy(path, dir_name);
      strcat(path, slash);
      strcat(path, src_filename);
      destination = strdup(path);
    } else {
      destination = strdup(argv[argc - 1]);
    }
    if (lstat(destination, &dst_stat) == -1) {
      // le fichier n'existe pas
    }

    cp(source, destination, src_stat);
    free(destination);
  }
}

void recur(char *source, char *destination) {
  DIR *src_dir;
  struct dirent *src_dirent;
  struct stat src_stat, dst_stat;
  lstat(source, &src_stat);
  if (lstat(destination, &dst_stat) == -1) {
    mkdir(destination, src_stat.st_mode);
  }
  src_dir = opendir(source);
  while ((src_dirent = readdir(src_dir)) != NULL) {
    char *arg[2];
    char *filename = src_dirent->d_name;
    if (strcmp(filename, ".") == 0)
      continue;
    if (strcmp(filename, "..") == 0)
      continue;
    char path[strlen(source) + 1 + strlen(filename)];
    char *slash = (source[strlen(source) - 1] == '/') ? "" : "/";
    struct stat ent_stat;
    strcpy(path, source);
    strcat(path, slash);
    strcat(path, filename);
    lstat(path, &ent_stat);
    if (S_ISDIR(ent_stat.st_mode)) {
      char dst_path[strlen(destination) + 1 + strlen(filename)];
      slash = (source[strlen(source) - 1] == '/') ? "" : "/";
      strcpy(dst_path, destination);
      strcat(dst_path, slash);
      strcat(dst_path, filename);
      recur(path, dst_path);
    } else {
      arg[0] = path;
      arg[1] = destination;
      copy(2, arg, true);
    }
  }
}

int maincp(int argc, char *argv[]) {
  bool recursive = false;
  if (getopt(argc, argv, "r") == 'r') {
    recursive = true;
  }
  if (argc - optind >= 2) {
    copy(argc - optind, argv + optind, recursive);
  }
}