#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace std;

void receive_client(int client_fd, sockaddr_in client_addr) {
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr, client_ip, INET_ADDRSTRLEN);
  cout << "Connected: " << client_ip << endl;
}

class serror : std::exception {
  string err_msg;

public:
  explicit serror(const string &msg) : err_msg(msg) {}
  const char *what() const noexcept override { return err_msg.c_str(); }
};

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket connection failed");
    return 1;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = INADDR_ANY; // bind to all interfaces

  if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("binding failed");
    return 1;
  }

  if (listen(server_fd, 1) < 0) {
    perror("listen failed");
    return 1;
  }

  cout << "server connected on port 8080........." << endl;

  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(server_fd, (sockaddr *)&server_fd, &client_len);
  if (client_fd < 0) {
    perror("accept");
    return 1;
  }

  char buffer[1024] = {0};
  read(client_fd, buffer, 1024);
  cout << "Received: " << buffer << '\n';

  string response = "hello from server";
  send(client_fd, response.c_str(), response.size(), 0);

  close(client_fd);
  close(server_fd);
  return 0;
}
