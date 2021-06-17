#include "headers/job.h"



/* A process is a single process.  */
typedef struct process
{
    struct process* next;       /* next process in pipeline */
    char** argv;                /* for exec */
    pid_t pid;                  /* process ID */
    char completed;             /* true if process has completed */
    char stopped;               /* true if process has stopped */
    int status;                 /* reported status value */
} process;

/* A job is a pipeline of processes.  */
typedef struct job
{
    struct job* next;           /* next active job */
    char* command;              /* command line, used for messages */
    process* first_process;     /* list of processes in this job */
    pid_t pgid;                 /* process group ID */
    char notified;              /* true if user told about stopped job */
    struct termios tmodes;      /* saved terminal modes */
    int stdin, stdout, stderr;  /* standard i/o channels */
} job;




/* The active jobs are linked into a list.  This is its head.   */
job* first_job = NULL;
Here are some utility functions that are used for operating on job objects.

/* Find the active job with the indicated pgid.  */
struct job*
find_job(pid_t pgid)
{
    job* j;

    for (j = first_job; j; j = j->next)
        if (j->pgid == pgid)
            return j;
    return NULL;
}

/* Return true if all processes in the job have stopped or completed.  */
int
job_is_stopped(struct job * j)
{
    process* p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed && !p->stopped)
            return 0;
    return 1;
}

/* Return true if all processes in the job have completed.  */
int
job_is_completed(struct job * j)
{
    process* p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed)
            return 0;
    return 1;
}