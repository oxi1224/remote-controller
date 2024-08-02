#include <array>
#include <cstdlib>
#include <string>
#include <vector>
#include <string>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <chrono>
#include "logger.hpp"
#include "frame.hpp"
#include "opcode.hpp"
#include "sha1.hpp"
#include "base64.hpp"

#pragma comment(lib, "Ws2_32.lib")
const int VERSION = 1;

Logger logger("main.cpp");

/// TODO: Break this out into a separate class? (+performHandshake())
std::vector<byte> hexStringToBytes(const std::string& hex) {
  std::vector<byte> out;
  for (int i = 0; i < hex.length(); i += 2) {
    std::string b = hex.substr(i, 2);
    char byte = (char)strtol(b.c_str(), nullptr, 16);
    out.push_back(byte);
  }
  return out;
}

void performHandshake(SOCKET *s) {
  std::vector<char> payload;
  std::string key = Frame::getConnectionKey();
  payload.push_back(VERSION);
  payload.insert(payload.end(), key.begin(), key.end());
  std::cout << payload.size() << std::endl;
  Frame connectFrame(false, Opcode::CONNECT, false, payload.size(), std::array<byte, 4>(), payload);
  connectFrame.sendFrame(s);
  Frame response = Frame::read(s);

  SHA1 sha_instance;
  sha_instance.update(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
  std::string hexString = sha_instance.final();
  // SHA1.final() returns a hex string instead of byte array
  // Convert the hex string to bytes and convert those to characters
  std::string sha1;
  for (int i = 0; i < hexString.length(); i += 2) {
    std::string b = hexString.substr(i, 2);
    char c = (char)strtol(b.c_str(), nullptr, 16);
    sha1 += c;
  }
  std::string encoded = Base64::encode(sha1);
  std::string recKey = response.getPayloadString();
  if (recKey != encoded) {
    Frame closeFrame(true, Opcode::CLOSE, false, 0, std::array<byte, 4>(), std::vector<char>());
    closeFrame.sendFrame(s);
    logger.warn("Received invalid encoded key, closing connection");
    return;
  }
  logger.log("Successfully connected to controller");
}

int main() {
  WSAData wsaData;
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
    logger.errorf("Error {} while calling WSAStartup", WSAGetLastError());
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
    logger.errorf("Error {} while connecting to the server", WSAGetLastError());
    WSACleanup();
    return 1;
  }
  
  performHandshake(&s);
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  WSACleanup();
}
