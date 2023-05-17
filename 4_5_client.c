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
    printf("Использование: %s <IP-адрес сервера> <порт сервера>\n", argv[0]);
    exit(1);
  }

  struct sockaddr_in server_addr;
  int client_socket;

  // Создание сокета
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  sleep(1); // Добавление задержки перед подключением к серверу

  // Подключение к серверу
  connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Получение данных о фрагменте текста
  int fragment_start, fragment_size;
  recv(client_socket, &fragment_start, sizeof(fragment_start), 0);
  recv(client_socket, &fragment_size, sizeof(fragment_size), 0);

  // Получение фрагмента текста от сервера
  char input[4096], output[4096];
  recv(client_socket, input, fragment_size, 0);

  // Шифрование фрагмента текста
  encrypt(input, output, fragment_size);

  // Отправка зашифрованного фрагмента обратно на сервер
  send(client_socket, output, fragment_size, 0);

  close(client_socket);

  return 0;
}

