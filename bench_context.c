#define ITERATIONS 300

#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE

#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>

int main(void)
{

    cpu_set_t single_cpu_mask;
    CPU_ZERO(&single_cpu_mask);

    int my_cpu = sched_getcpu();
    if (my_cpu == -1)
    {
        perror("getcpu");
        exit(1);
    }
    CPU_SET(my_cpu, &single_cpu_mask);

    // set parent to use a single cpu
    int ssa_res = sched_setaffinity(0, sizeof(cpu_set_t), &single_cpu_mask);
    if (ssa_res == -1)
    {
        perror("set affinity");
        exit(1);
    }

    int parent_write_pipe[2];
    if (pipe(parent_write_pipe) == -1)
    {
        perror("parent pipe");
        exit(1);
    }

    int child_write_pipe[2];
    if (pipe(child_write_pipe) == -1)
    {
        perror("child pipe");
        exit(1);
    }

    int rc = fork();

    if (rc < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (rc == 0)
    {
        // in child now
        // make child run on same cpu
        int csca_res = sched_setaffinity(0, sizeof(cpu_set_t), &single_cpu_mask);
        if (csca_res == -1)
        {
            perror("set set affinity");
            exit(1);
        }

        close(parent_write_pipe[1]);
        close(child_write_pipe[0]);

        char buf = 'a';
        // assume nothing will go wrong with read/write (no error checks)
        for (int i = 0; i < ITERATIONS; i++)
        {
            // printf("c r\n");
            read(parent_write_pipe[0], &buf, 1);
            // printf("c w\n");
            write(child_write_pipe[1], &buf, 1);
        }

        close(child_write_pipe[1]);
        close(parent_write_pipe[0]);
    }
    else
    {
        // parent

        close(child_write_pipe[1]);
        close(parent_write_pipe[0]);

        struct timeval tv_start;
        struct timeval tv_end;

        gettimeofday(&tv_start, NULL);

        char buf = 'a';
        // assume nothing will go wrong with read/write (no error checks)
        for (int i = 0; i < ITERATIONS; i++)
        {
            // printf("p w\n");
            write(parent_write_pipe[1], &buf, 1);
            // printf("p r\n");
            read(child_write_pipe[0], &buf, 1);
            // printf(" ------ done\n");
        }

        gettimeofday(&tv_end, NULL);

        close(parent_write_pipe[1]);
        close(child_write_pipe[0]);

        // because we measure 2 pipe write/reads so we also have to halve it
        long double estimated_time = ((long double) (tv_end.tv_usec - tv_start.tv_usec)) / ITERATIONS * 2;

        printf("estimated time: %Lfus\n", estimated_time);
    }

    return 0;
}