#include<sys/types.h>
#include<sys/socket.h>
#include<strings.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<signal.h>
#include<sys/wait.h>

#define MAX_CONNECTION_QUEUE 5
#define true 1
#define false 0

int tcp_socket;
int conn_fd;

void Stop(int signal_no) {
    printf("Oops! stop!!! %d\n", signal_no);
    close(tcp_socket);
    close(conn_fd);
    _exit(0);
}

void sendMsg(int fd, const char *str) {
    if (send(fd, str, strlen(str), 0) < 0) {
        fprintf(stderr, "Send Failed\n");
    }
}


int main(int argc, char **argv) {
    pid_t child_pid;
    socklen_t addr_len;
    int port;
    signal(SIGINT, Stop);

    fflush(stdout);
    if (argc != 2) {
        fprintf(stderr, "ERROR: Wrong number of arguments, please %s PORT_NUM\n", argv[0]);
        fflush(stdout);
        return -1;
    }
    port = atoi(argv[1]);
    if (port == 0) {
        fprintf(stderr, "ERROR: Wrong port number\n");
        fflush(stdout);
        return -1;
    }

    printf("Started server\n");

    struct sockaddr_in tcp_client, tcp_server;

    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        fprintf(stderr, "ERROR: Socket established error: %s\n", (char *) strerror(errno));
        return -1;
    }

    bzero(&tcp_server, sizeof(tcp_server));
    tcp_server.sin_family = AF_INET;
    tcp_server.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_server.sin_port = htons(port);

    // Bind error
    if (bind(tcp_socket, (struct sockaddr *) &tcp_server, sizeof(tcp_server))) {
        fprintf(stderr, "ERROR: Bind error: %s\n", strerror(errno));
        return -1;
    }

    if (listen(tcp_socket, MAX_CONNECTION_QUEUE) < 0) {
        fprintf(stderr, "ERROR: listen() failed\n");
        return -1;
    }

    while (true) {
        addr_len = sizeof(tcp_client);
        conn_fd = accept(tcp_socket, (struct sockaddr *) &tcp_client, &addr_len);
        if (conn_fd == -1) {
            fprintf(stderr, "ERROR: Accept client error: %s\n", strerror(errno));
            return -1;
        } else {
            printf("Client connected\n");
        }

        if ((child_pid = fork()) == 0) {
            printf("Client from %s\n", inet_ntoa(tcp_client.sin_addr));
            fflush(stdout);
            for (int i = 0; i < 5; ++i) {
                char buf[1024];
                pid_t pid = getpid();
                sprintf(buf, "[%d] PID %d Iteration #%d\n", port, pid, i);
                sendMsg(conn_fd, buf);
                sleep(2);
            }
            exit(0);
        } else if (child_pid < 0)
            fprintf(stderr, "ERROR: fork error: %s\n", strerror(errno));
        close(conn_fd);
    }
}