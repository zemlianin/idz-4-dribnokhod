#include "UDPEchoServer.h" /* TCP echo server includes */
#include <sys/wait.h>      /* for waitpid() */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <sys/socket.h> /* for socket(), bind, and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <string.h>     /* for memset() */
#include <fcntl.h>      /* for fcntl() */
#include <sys/file.h>   /* for O_NONBLOCK and FASYNC */
#include <signal.h>     /* for signal() and SIGALRM */
#include <errno.h>
#define RCVBUFSIZE 32

sem_t *sem;
sem_t *sems;
float EPS = 0.01;
float fun_param_1 = 1;
float fun_param_2 = 2;
float fun_param_3 = 3;

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    unsigned short echoServPort;
    pid_t processID;
    unsigned int childProcCount = 0;
    const char *sem_name = "sem100";
    const char *sem_s = "sem10";
    int SIZE = 256;

    /////
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */
    unsigned int cliAddrLen;         /* Length of incoming message */
    /////

    int shm_fd;
    void *ptr;
    const char *name = "name";
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    ptr = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    const char *names1 = "s1";
    int shm_fds1;
    void *ptrs1;
    shm_fds1 = shm_open(names1, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fds1, SIZE);
    ptrs1 = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fds1, 0);

    sprintf(ptr, "%f", 0.0);

    sprintf(ptrs1, "%f", 0.0);

    float left = 0;
    float right = 10;
    float step = (right - left) / 10;

    if ((sem = sem_open(sem_name, O_CREAT, 0666, 1)) == 0)
    {
        perror("sem_open: Can not create admin semaphore");
        exit(-1);
    };
    if ((sems = sem_open(sem_s, O_CREAT, 0666, 1)) == 0)
    {
        perror("sem_open: Can not create admin semaphore");
        exit(-1);
    };

    if (argc != 2)
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    servSock = CreateUDPServerSocket(echoServPort);

    int cur = 0;
    int SIZE_F = sizeof(float);

    for (int i = 0; i < 4; i++)
    {
        cliAddrLen = sizeof(echoClntAddr);

        // clntSock = AcceptUDPConnection(servSock);
        if ((processID = fork()) < 0)
            DieWithError("fork() failed");
        else if (processID == 0)
        {
            char echoBuffer[RCVBUFSIZE];
            /* Block until receive message from a client */
            int recvMsgSize;
            char lb[SIZE_F];
            char rb[SIZE_F];
            char ansb[SIZE_F];
            float l = 0;
            float r = 0;
            if (recvfrom(servSock, ansb, SIZE_F, 0,
                         (struct sockaddr *)&echoClntAddr, &cliAddrLen) < 0)
            {
                DieWithError("recvfrom() failed");
            };
            while (cur < 10)
            {
                sem_wait(sem);
                float cur = atof((char *)ptr);
                sprintf(ptr, "%f", cur + 1);
                sem_post(sem);

                if (cur >= 10)
                {
                    sprintf(lb, "%f", -1.0);
                    sprintf(rb, "%f", -1.0);
                    sendto(servSock, lb, SIZE_F, 0,
                           (const struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr));
                    sendto(servSock, rb, SIZE_F, 0,
                           (const struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr));
                    break;
                }

                l = left + cur * step;
                r = left + (cur + 1) * step;
                sprintf(lb, "%f", l);
                sprintf(rb, "%f", r);
                sendto(servSock, lb, SIZE_F, 0,
                       (const struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr));
                sendto(servSock, rb, SIZE_F, 0,
                       (const struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr));

                if (recvfrom(servSock, ansb, SIZE_F, 0,
                             (struct sockaddr *)&echoClntAddr, &cliAddrLen) < 0)
                {
                    DieWithError("recvfrom() failed");
                };
                float ans = atof(ansb);
                sem_wait(sems);
                pid_t pid = getpid();
                printf("area: %f - Process %d\n", ans, pid);
                fflush(stdout);
                float s = atof((char *)ptrs1);
                sprintf(ptrs1, "%f", s + ans);
                sem_post(sems);
            }
            sem_unlink(sem_name);

            exit(0);
        }

        close(clntSock);

        while (childProcCount)
        {
            printf("%d\n", childProcCount);
            processID = waitpid((pid_t)-1, NULL, WNOHANG);
            if (processID < 0)
                DieWithError("waitpid() failed");
            else if (processID == 0)
                break;
            else
                childProcCount--;
        }
        float cur = atof((char *)ptr);
        /*if (childProcCount == 0 && cur >= 9)
        {
            float s = atof((char *)ptrs1);
            printf("all area: %f\n", s);
            fflush(stdout);
            close(servSock);
            exit(0);
        }*/
        exit(0);
    }
}
