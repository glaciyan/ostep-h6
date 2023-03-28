#define ITERATIONS 10000

#define _POSIX_C_SOURCE 1

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
    int32_t *times = (int32_t *) calloc(ITERATIONS, sizeof(int32_t));

    struct timeval tv_start;
    struct timeval tv_end;

    int fd = open("./somefile.txt", O_CREAT | O_WRONLY, S_IRWXU);

    for (int32_t i = 1; i < ITERATIONS; i++)
    {
        gettimeofday(&tv_start, NULL);

        if (write(fd, NULL, 0) == -1)
        {
            perror("write");
            exit(1);
        }

        gettimeofday(&tv_end, NULL);
        times[i] = (int32_t) (tv_end.tv_usec - tv_start.tv_usec);
    }

    uintmax_t time_sum = 0;

    for (int32_t i = 0; i < ITERATIONS; i++)
    {
        time_sum += times[i];
    }

    long double average_time = (long double) time_sum / (long double) ITERATIONS;

    printf("Average time: %Lfus\n", average_time);

    free(times);
    close(fd);
    return 0;
}