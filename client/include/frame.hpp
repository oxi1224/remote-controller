#pragma once

#include "opcode.hpp"
#include "logger.hpp"
#include <array>
#include <vector>
#include <winsock2.h>

class Frame {
  public:
  bool fin;
  Opcode opcode;
  bool masked;
  int payloadLength;
  std::array<byte, 4> maskingKey;
  std::vector<byte> payload;

  Frame(
    bool fin,
    Opcode opcode,
    bool masked,
    int payloadLength,
    std::array<byte, 4> maskingKey,
    std::vector<byte> payload
  );

  static Frame read(SOCKET* s);
  static std::array<byte, 4> getMaskingKey();
  static std::string getConnectionKey();
  std::vector<char> getBytes();
  bool validMaskingKey(std::array<byte, 4> key);
  void sendFrame(SOCKET* s);

  private:
  static int recvData(SOCKET* s, char *buf, int len);
  inline static Logger log = Logger("Frame");
};
