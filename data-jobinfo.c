#include "datastructures/info/jobinfo.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>



#include "datastructures/job.h"

/* Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise.  */

int mark_process_status(pid_t pid, int status)
{
    struct job* j;
    struct process* p;

    if (pid > 0)
    {
        /* Update the record for the process.  */
        for (j = first_job; j; j = j->next)
            for (p = j->first_process; p; p = p->next)
                if (p->pid == pid)
                {
                    p->status = status;
                    if (WIFSTOPPED(status))
                        p->stopped = 1;
                    else
                    {
                        p->completed = 1;
                        if (WIFSIGNALED(status))
                            fprintf(stderr, "%d: Terminated by signal %d.\n",
                                (int)pid, WTERMSIG(p->status));
                    }
                    return 0;
                }
        fprintf(stderr, "No child process %d.\n", pid);
        return -1;
    }
    else if (pid == 0 || errno == ECHILD)
        /* No processes ready to report.  */
        return -1;
    else {
        /* Other weird errors.  */
        perror("waitpid");
        return -1;
    }
}

/* Check for processes that have status information available,
   without blocking.  */

void update_status(void)
{
    int status;
    pid_t pid;

    do
        pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    while (!mark_process_status(pid, status));
}



/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */

void wait_for_job(struct job* j)
{
    int status;
    pid_t pid;

    do
        pid = waitpid(WAIT_ANY, &status, WUNTRACED);
    while (!mark_process_status(pid, status)
        && !job_is_stopped(j)
        && !job_is_completed(j));
}



/* Format information about job status for the user to look at.  */

void format_job_info(struct job* j, const char* status)
{
    fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}



/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */

void do_job_notification(void)
{
    struct job* j, * jlast, * jnext;

    /* Update status information for child processes.  */
    update_status();

    jlast = NULL;
    for (j = first_job; j; j = jnext)
    {
        jnext = j->next;

        /* If all processes have completed, tell the user the job has
           completed and delete it from the list of active jobs.  */
        if (job_is_completed(j)) {
            format_job_info(j, "completed");
            if (jlast)
                jlast->next = jnext;
            else
                first_job = jnext;
            // free_job(j); C'est quoi ????????????????????????????????????????????
        }

        /* Notify the user about stopped jobs,
           marking them so that we won?t do this more than once.  */
        else if (job_is_stopped(j) && !j->notified) {
            format_job_info(j, "stopped");
            j->notified = 1;
            jlast = j;
        }

        /* Don?t say anything about jobs that are still running.  */
        else
            jlast = j;
    }
}