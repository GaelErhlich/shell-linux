#include "../job.h"


/* Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise.  */
int mark_process_status(pid_t pid, int status);


/* Check for processes that have status information available,
   without blocking.  */
void update_status(void);


/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */
void wait_for_job(struct job* j);


/* Format information about job status for the user to look at.  */
void format_job_info(struct job* j, const char* status);


/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */
void do_job_notification(void);