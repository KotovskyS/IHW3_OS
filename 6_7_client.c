#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void encrypt(const char *input, char *output, int fragment_size) {
  for (int i = 0; i < fragment_size; i++) {
    output[i] = input[i] + 3;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <Server IP Address> <Server Port>\n", argv[0]);
    exit(1);
  }

  struct sockaddr_in server_addr;
  int client_socket;

  // Socket creation
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Cannot create socket");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  sleep(1); // Add delay before connecting to server

  // Connecting to server
  if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Connect failed");
    exit(1);
  }

  // Get fragment info
  int fragment_start, fragment_size;
  if (recv(client_socket, &fragment_start, sizeof(fragment_start), 0) < 0) {
    perror("Recv failed");
    exit(1);
  }
  if (recv(client_socket, &fragment_size, sizeof(fragment_size), 0) < 0) {
    perror("Recv failed");
    exit(1);
  }

  // Get fragment from server
  char input[4096], output[4096];
  if (recv(client_socket, input, fragment_size, 0) < 0) {
    perror("Recv failed");
    exit(1);
  }

  // Encrypt fragment
  encrypt(input, output, fragment_size);

  // Send encrypted fragment back to server
  if (send(client_socket, output, fragment_size, 0) < 0) {
    perror("Send failed");
    exit(1);
  }

  close(client_socket);

  return 0;
}

