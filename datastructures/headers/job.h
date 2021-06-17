#include <sys/types.h>


struct process;
struct job;



/* Find the active job with the indicated pgid.  */
struct job* find_job(pid_t pgid);

/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped(struct job* j);

/* Return true if all processes in the job have completed.  */
int job_is_completed(struct job* j);