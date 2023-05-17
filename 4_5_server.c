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
  if (argc != 4) {
    printf("Использование: %s <IP-адрес> <порт> <текст>\n", argv[0]);
    exit(1);
  }

  struct sockaddr_in server_addr;
  int server_socket;
  int num_clients = 3;  // Количество клиентов
  char encrypted_text[4096];

  // Создание сокета
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  // Привязка сокета к адресу
  bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Ожидание подключений клиентов
  listen(server_socket, num_clients);

  // Обработка подключений клиентов
  for (int i = 0; i < num_clients; i++) {
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

    // Отправка текста и информации о фрагменте клиенту
    int fragment_start = (strlen(argv[3]) / num_clients) * i;
    int fragment_size = (i == num_clients - 1) ? (strlen(argv[3]) - fragment_start) : (strlen(argv[3]) / num_clients);
    send(client_socket, &fragment_start, sizeof(fragment_start), 0);
    send(client_socket, &fragment_size, sizeof(fragment_size), 0);
    send(client_socket, argv[3] + fragment_start, fragment_size, 0);

    // Получение зашифрованного текста от клиента
    recv(client_socket, encrypted_text + fragment_start, fragment_size, 0);

    close(client_socket);
  }

  close(server_socket);

  encrypted_text[strlen(argv[3])] = '\0';
  printf("Зашифрованный текст: %s\n", encrypted_text);

  return 0;
}

