#pragma once

#include "opcode.hpp"
#include "logger.hpp"
#include <vector>
#include <winsock2.h>

class Frame {
  public:
  bool fin;
  Opcode opcode;
  bool masked;
  int payloadLength;
  byte maskingKey[4];
  std::vector<byte> payload;

  Frame(
    bool fin,
    Opcode opcode,
    int payloadLength,
    byte maskingKey[4],
    std::vector<byte> payload
  );

  static Frame read(SOCKET* s);
  std::vector<byte> getBytes();

  private:
  static int recvData(SOCKET* s, char *buf, int len);
  inline static Logger log = Logger("Frame");
};
