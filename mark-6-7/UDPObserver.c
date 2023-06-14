#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdbool.h>

float EPS = 0.01;
float fun_param_1 = 1;
float fun_param_2 = 2;
float fun_param_3 = 3;

#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage); /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv()
                                        and total bytes read */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned int fromSize;

    if ((argc < 2) || (argc > 3)) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>]\n",
                argv[0]);
        exit(1);
    }

    servIP = argv[1]; /* First arg: server IP address (dotted quad) */

    if (argc == 3)
        echoServPort = atoi(argv[2]); /* Use given port, if any */

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);      /* Server port */

    char lb[1] = "1";
    sendto(sock, lb, 1, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr));
    if (recvfrom(sock, lb, 1, 0,
                 NULL, NULL) < 0)
    {
        DieWithError("recvfrom() failed");
    }
    sendto(sock, lb, 1, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr));
    while (true)
    {
        printf("aa\n");
        fflush(stdout);
        sleep(1);
        if (recvfrom(sock, lb, 1, 0,
                     NULL, NULL) < 0)
        {
            DieWithError("recvfrom() failed");
        }
        printf("aa\n");
        fflush(stdout);
        sleep(1);
        // bytesRcvd = read(sock, rb, SIZE_F);
        printf("%s", lb);
        if (lb[0] == '#')
        {
            break;
        }
    }
    printf("\n"); /* Print a final linefeed */
    sleep(2);

    close(sock);
    exit(0);
}
