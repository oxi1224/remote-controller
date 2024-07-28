#include <array>
#include <chrono>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "logger.hpp"
#include "frame.hpp"
#include "opcode.hpp"

#pragma comment(lib, "Ws2_32.lib")

const int VERSION = 1;

void performHandshake(SOCKET *s) {
  std::vector<byte> payload;
  std::string key = Frame::getConnectionKey();
  payload.push_back(VERSION);
  payload.insert(payload.end(), key.begin(), key.end());
  Frame connectFrame(false, Opcode::CONNECT, false, payload.size(), std::array<byte, 4>(), payload);
  Frame response = Frame::read(s);
  connectFrame.sendFrame(s);
}

int main() {
  Logger l("main()");
  WSAData wsaData;
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
    l.errorf("Error {} while calling WSAStartup", WSAGetLastError());
    std::cout << "Error occured while calling WSAStartup" << std::endl;
    std::cout << WSAGetLastError() << std::endl;
    return 1;
  }

  SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, "192.168.50.143", &addr.sin_addr);
  addr.sin_port = htons(6000);

  if (connect(s, (const sockaddr*)&addr, sizeof(addr)) != 0) {
    l.errorf("Error {} while connecting to the server", WSAGetLastError());
    WSACleanup();
    return 1;
  }
  
  performHandshake(&s);
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  WSACleanup();
}
