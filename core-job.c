#include "core/job.h"

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "datastructures/info/jobinfo.h"




void launch_process(struct process *p, pid_t pgid, int infile, int outfile,
                    int errfile, int foreground) {
  pid_t pid;

  if (shell_is_interactive) {
    /* Put the process into the process group and give the process group
       the terminal, if appropriate.
       This has to be done both by the shell and in the individual
       child processes because of potential race conditions.  */
    pid = getpid();
    if (pgid == 0)
      pgid = pid;
    setpgid(pid, pgid);
    if (foreground)
      tcsetpgrp(shell_terminal, pgid);


    /* Set the handling for job control signals back to the default.  */
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
  }


  /* Set the standard input/output channels of the new process.  */
  if (infile != STDIN_FILENO) {
    dup2(infile, STDIN_FILENO);
    close(infile);
  }
  if (outfile != STDOUT_FILENO) {
    dup2(outfile, STDOUT_FILENO);
    close(outfile);
  }
  if (errfile != STDERR_FILENO) {
    dup2(errfile, STDERR_FILENO);
    close(errfile);
  }

  //printf("Après set standard i/o channels of process\n");

  /* Exec the new process.  Make sure we exit.  */
  //printf("argv : %s, %s, %s, ...\n", p->argv[0], p->argv[1], p->argv[2]);
  execvp(p->argv[0], p->argv);
  perror("execvp");
  exit(1);
}

void launch_job(struct job *j, int foreground) {
    //printf("Debut launch_job, avec in %d et out %d, et foreground %d\n", j->stdin, j->stdout, foreground);
  struct process *p;
  pid_t pid;
  int mypipe[2], infile, outfile;

  infile = j->stdin;
  for (p = j->first_process; p; p = p->next) {
      /*if (p->next) {
        printf("1 processus : %s, %s, %s (suiv : %s)\n", p->argv[0], p->argv[1], p->argv[2], p->next->argv[0]);
      } else {
          printf("1 processus : %s, %s, %s (suiv : %s)\n", p->argv[0], p->argv[1], p->argv[2], "aucun");
      }*/
      
    /* Set up pipes, if necessary.  */
    if (p->next) {
      if (pipe(mypipe) < 0) {
        perror("pipe");
        exit(1);
      }
      outfile = mypipe[1];
    } else
      outfile = j->stdout;

    /* Fork the child processes.  */
    pid = fork();
    if (pid == 0)
      /* This is the child process.  */
      launch_process(p, j->pgid, infile, outfile, j->stderr, foreground);
    else if (pid < 0) {
      /* The fork failed.  */
      perror("fork");
      exit(1);
    } else {
      /* This is the parent process.  */
      p->pid = pid;
      if (shell_is_interactive) {
        if (!j->pgid)
          j->pgid = pid;
        setpgid(pid, j->pgid);
      }
    }

    /* Clean up after pipes.  */
    
    
    if (infile != j->stdin)
      close(infile);
    if (outfile != j->stdout)
      close(outfile);
    
    infile = mypipe[0];
  }

  //format_job_info(j, "launched");

  if (!shell_is_interactive)
    wait_for_job(j);
  else if (foreground)
    put_job_in_foreground(j, 0);
  else
    put_job_in_background(j, 0);
}

void put_job_in_foreground(struct job *j, int cont) {
  /* Put the job into the foreground.  */
  tcsetpgrp(shell_terminal, j->pgid);

  /* Send the job a continue signal, if necessary.  */
  if (cont) {
    tcsetattr(shell_terminal, TCSADRAIN, &j->tmodes);
    if (kill(-j->pgid, SIGCONT) < 0)
      perror("kill (SIGCONT)");
  }

  /* Wait for it to report.  */
  wait_for_job(j);

  /* Put the shell back in the foreground.  */
  tcsetpgrp(shell_terminal, shell_pgid);

  /* Restore the shell’s terminal modes.  */
  tcgetattr(shell_terminal, &j->tmodes);
  tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
}

void put_job_in_background(struct job *j, int cont) {
  /* Send the job a continue signal, if necessary.  */
  if (cont)
    if (kill(-j->pgid, SIGCONT) < 0)
      perror("kill (SIGCONT)");
}
