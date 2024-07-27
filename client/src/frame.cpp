#include "frame.hpp"
#include "opcode.hpp"
#include <iterator>
#include <stdexcept>
#include <vector>
#include <winsock2.h>

Frame::Frame(
  bool fin,
  Opcode opcode,
  int payloadLength,
  byte maskingKey[4],
  std::vector<byte> payload
) {
  this->fin = fin;
  this->opcode = opcode;
  this->payloadLength = payloadLength;
  memcpy(this->maskingKey, maskingKey, 4);
  this->payload = payload;
}

int Frame::recvData(SOCKET *s, char* buf, int len) {
  int result = recv(*s, buf, len, 0);
  if (result <= 0) {
    log.error("Connection closed while trying to read from socket");
    throw new std::runtime_error("Connection closed while reading");
  }
  return result;
}

Frame Frame::read(SOCKET* s) {
  if (s == nullptr || *s == INVALID_SOCKET) {
    log.error("An invalid socket has been provied to Frame.read()");
    throw new std::invalid_argument("Provided socket is invalid");
  }
  char info_buf[2];
  int result = recvData(s, info_buf, sizeof(info_buf));
  if (std::size(info_buf) != 2) {
    log.error("Received buffer does not contain 2 elements");
    throw new std::runtime_error("Received buffer does not contain 2 elements");
  }

  byte b = info_buf[0];
  boolean fin = (b & 0x80) != 0;
  Opcode opcode = toOpcode(b & 0x0F);
  b = info_buf[1];
  boolean masked = (b & 0x80) != 0;
  int payloadLength = b & 0x0F;
  int bytesToRead = 0;

  if (payloadLength == 126) bytesToRead = 2;
  if (payloadLength == 127) bytesToRead = 8;
  if (bytesToRead != 0) {
    char payload_len_buf[bytesToRead];
    recvData(s, payload_len_buf, sizeof(payload_len_buf));
    payloadLength = 0;
    for (int i = 0; i < bytesToRead; i++) {
      payloadLength = (payloadLength << 8) + (payload_len_buf[i] & 0xFF);
    }
  }
  
  byte maskingKey[4];
  if (masked) {
    char mask_buf[4];
    recvData(s, mask_buf, 4);
    for (int i = 0; i < 4; i++) {
      maskingKey[i] = mask_buf[i];
    }
  }
  
  char payload_buf[payloadLength];
  recvData(s, payload_buf, payloadLength);
  // std::end causes the compilation to freeze and idk why
  std::size_t size = sizeof(payload_buf) / sizeof(payload_buf[0]);
  std::vector<byte> payload(payload_buf, payload_buf + size);
  return Frame(fin, opcode, masked, maskingKey, payload);

}
