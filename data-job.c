#include "datastructures/job.h"


#include <unistd.h>
#include <stdlib.h>





/* Creer une instance de process */
struct process** nouvProcess(int nbProcess) {
    struct process** processArray = malloc(sizeof(struct process)*nbProcess);

    
    for (int i = 0; i < nbProcess; i++) {
        processArray[i] = malloc(sizeof(struct process));
        if (processArray[i] == NULL)
            return NULL;
    }

    return processArray;
}




//Here are some utility functions that are used for operating on job objects.

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