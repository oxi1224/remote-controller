#include "client.hpp"
#include "frame.hpp"
#include "opcode.hpp"
#include "sha1.hpp"
#include "base64.hpp"
#include <string>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <chrono>

Client::~Client() {
  close();
}

// Avoid name collision with winsock2 connect
void Client::connectTo(std::string ipaddr, int port) {
  if (WSAStartup(MAKEWORD(2, 0), &m_wsaData) != 0) {
    logger.errorf("Error {} while calling WSAStartup", WSAGetLastError());
    return;
  }

  m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, "192.168.50.143", &addr.sin_addr);
  addr.sin_port = htons(6000);

  if (connect(m_socket, (const sockaddr*)&addr, sizeof(addr)) != 0) {
    logger.errorf("Error {} while connecting to the server", WSAGetLastError());
    WSACleanup();
    return;
  } else {
    logger.log("Connected to client, attempting handshake");
  }
  
  performHandshake();
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  WSACleanup();
}

void Client::performHandshake() {
  std::vector<char> payload;
  std::string key = Frame::getConnectionKey();
  payload.push_back(VERSION);
  payload.insert(payload.end(), key.begin(), key.end());
  Frame connectFrame(false, Opcode::CONNECT, masking, payload.size(), std::array<byte, 4>(), payload);
  sendFrame(connectFrame);
  Frame response = Frame::read(&m_socket);

  SHA1 sha_instance;
  sha_instance.update(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
  std::string encoded = Base64::encode(sha_instance.finalStringBytes());
  std::string received = response.getPayloadString();
  if (received != encoded) {
    Frame closeFrame = Frame::getClose("Received invalid verification key", masking);
    sendFrame(closeFrame);
    logger.warn("Received invalid encoded key, closing connection");
    close();
    return;
  }
  logger.log("Successfully connected to controller");
}

void Client::sendFrame(Frame f) {
  std::vector<char> outvec = f.getBytes();
  char* outbuf = outvec.data();
  int res = send(m_socket, outbuf, (int)strlen(outbuf), 0);
  if (res == SOCKET_ERROR) {
    logger.errorf("send returned with error code {}", WSAGetLastError());
  }
}

void Client::close() {
  WSACleanup();
  closesocket(m_socket);
}
