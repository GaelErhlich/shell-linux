#include "./init.c"

void launch_process(process* p, pid_t pgid, int infile, int outfile,
    int errfile, int foreground);

void launch_job(job* j, int foreground);

void put_job_in_foreground(job* j, int cont);

void put_job_in_background(job* j, int cont);