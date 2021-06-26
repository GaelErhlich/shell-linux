#include "init.h"

#include <sys/types.h>

#include "../datastructures/job.h"



void launch_process(struct process* p, pid_t pgid, int infile, int outfile,
    int errfile, int foreground);

void launch_job(struct job* j, int foreground);

void put_job_in_foreground(struct job* j, int cont);

void put_job_in_background(struct job* j, int cont);