#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

struct ResponseData {
  std::chrono::microseconds response_time;
  int response_length;
};

ResponseData make_request(const char *host, const char *port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  struct addrinfo *servinfo;
  int res = getaddrinfo(host, port, &hints, &servinfo);
  if (res != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(res) << std::endl;
    exit(1);
  }

  res = connect(sock, servinfo->ai_addr, servinfo->ai_addrlen);
  if (res < 0) {
    perror("connect");
    exit(1);
  }

  freeaddrinfo(servinfo);

  const char *request = "GET / HTTP/1.0\r\n\r\n";
  auto start_time = std::chrono::high_resolution_clock::now();
  send(sock, request, strlen(request), 0);

  char response[1024];
  int response_length = recv(sock, response, sizeof response, 0);

  close(sock);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto response_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  return {response_time, response_length};
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " HOST PORT" << std::endl;
    return 1;
  }

  const char *host = argv[1];
  const char *port = argv[2];

  int num_threads = 100;
  std::vector<std::thread> threads;
  std::vector<ResponseData> results(num_threads);
  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back([&, i] {
      results[i] = make_request(host, port);
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  int total_length = 0;
  int total_time = 0;
  for (const auto &r : results) {
    total_length += r.response_length;
    total_time += r.response_time.count();
  }

  int average_time = total_time / num_threads;
  int average_length = total_length / num_threads;

  std::cout << "Average response time: " << average_time << " microseconds" << std::endl;
  std::cout << "Average response length: " << average_length << " bytes" << std::endl;

  return 0;
}
