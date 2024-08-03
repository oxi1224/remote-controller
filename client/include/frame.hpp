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
  std::vector<char> payload;

  Frame(
    bool fin,
    Opcode opcode,
    bool masked,
    int payloadLength,
    std::array<byte, 4> maskingKey,
    std::vector<char> payload
  );

  static Frame read(SOCKET* s);
  static std::array<byte, 4> getMaskingKey();
  static std::string getConnectionKey();
  static Frame getGeneric(Opcode opcode, bool masked);
  static Frame getClose(std::string reason, bool masked);
  std::vector<char> getBytes();
  std::string getPayloadString();
  bool validMaskingKey(std::array<byte, 4> key);

  private:
  static int recvData(SOCKET* s, char *buf, int len);
  inline static Logger logger = Logger("Frame");
};
