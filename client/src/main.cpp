#include <iostream>
#include <ostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "logger.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main() {
  Logger l;
  l.log("hello world");
  l.error("error");
  // WSAData wsaData;
  // if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
  //   std::cout << "Error occured while calling WSAStartup" << std::endl;
  //   std::cout << WSAGetLastError() << std::endl;
  //   return 1;
  // }
  //
  // SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // inet_pton(AF_INET, "192.168.50.143", &addr.sin_addr);
  // addr.sin_port = htons(6000);
  //
  // if (connect(s, (const sockaddr*)&addr, sizeof(addr)) != 0) {
  //   std::cout << "Error occured while connecting to server" << std::endl;
  //   std::cout << WSAGetLastError() << std::endl;
  //   WSACleanup();
  //   return 1;
  // }
  // WSACleanup();
}
