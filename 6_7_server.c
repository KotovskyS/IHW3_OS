#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Запускайте так: %s <Server IP> <Server Port> <Text> <Client Count> <Observer Port>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in server_addr, observer_addr;
    int server_socket, observer_socket, observer;
    int num_clients = atoi(argv[4]);
    char encrypted_text[4096];
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    observer_socket = socket(AF_INET, SOCK_STREAM, 0);
    observer_addr.sin_family = AF_INET;
    observer_addr.sin_port = htons(atoi(argv[5]));
    inet_pton(AF_INET, argv[1], &observer_addr.sin_addr);
    bind(observer_socket, (struct sockaddr *) &observer_addr, sizeof(observer_addr));
    listen(server_socket, num_clients);
    listen(observer_socket, 1);
    observer = accept(observer_socket, NULL, NULL);
    for (int i = 0; i < num_clients; i++) {
        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);
        int fragment_start = (strlen(argv[3]) / num_clients) * i;
        int fragment_size = (i == num_clients - 1) ? (strlen(argv[3]) - fragment_start) : (strlen(argv[3]) /
                                                                                           num_clients);
        send(client_socket, &fragment_start, sizeof(fragment_start), 0);
        send(client_socket, &fragment_size, sizeof(fragment_size), 0);
        send(client_socket, argv[3] + fragment_start, fragment_size, 0);
        recv(client_socket, encrypted_text + fragment_start, fragment_size, 0);
        char update[4096];
        sprintf(update, "Client %d encrypted part of the message...\n", i + 1);
        send(observer, update, strlen(update), 0);
        close(client_socket);
    }
    close(server_socket);
    close(observer_socket);
    encrypted_text[strlen(argv[3])] = '\0';
    printf("Encrypted text: %s\n", encrypted_text);
    return 0;
}

