#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<signal.h>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int client_sock_fd;
FILE *fp = NULL;


void Stop(int signal_no) {
    printf("Oops! stop!!! %d\n", signal_no);
    close(client_sock_fd);
    fclose(fp);
    _exit(0);
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "ERROR: Usage: %s <port of server> <log file> \n", argv[0]);
        return 1;
    }

    signal(SIGINT, Stop);

    char buf[1024];
    int port;
    int len;

    struct sockaddr_in remote_addr;

    port = atoi(argv[1]);
    if (port == 0) {
        fprintf(stderr, "ERROR: Wrong port number\n");
        fflush(stdout);
        return -1;
    }


    fp = fopen(argv[2], "w+");


    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(port);

    client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock_fd == -1) {
        fprintf(stderr, "ERROR: Socket established error: %s\n", (char *) strerror(errno));
        return -1;
    }


    if (connect(client_sock_fd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr)) < 0) {
        fprintf(stderr, "ERROR: Socket connect error: %s\n", (char *) strerror(errno));
        return -1;
    }

    while (1) {
        if ((len = read(client_sock_fd, buf, sizeof(buf) - 1)) > 0) {
            buf[len] = 0;
            printf("%s\n", buf);
            fprintf(fp, buf, len);
        } else {
            printf("INFO: connection closed");
            break;
        }
    }
    close(client_sock_fd);
    fclose(fp);
    return 0;
}

#pragma clang diagnostic pop