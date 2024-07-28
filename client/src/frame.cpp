#include "frame.hpp"
#include "opcode.hpp"
#include <array>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>
#include <winsock2.h>

Frame::Frame(
  bool fin,
  Opcode opcode,
  bool masked,
  int payloadLength,
  std::array<byte, 4> maskingKey,
  std::vector<byte> payload
) {
  this->fin = fin;
  this->opcode = opcode;
  this->masked = masked;
  this->payloadLength = payloadLength;
  this->maskingKey = maskingKey;
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

bool Frame::validMaskingKey(std::array<byte, 4> key) {
  if (
    key[0] == 0 &&
    key[1] == 0 &&
    key[2] == 0 &&
    key[3] == 0
  ) return false;
  return true;
}

std::array<byte, 4> Frame::getMaskingKey() {
  std::array<byte, 4> out;
  for (int i = 0; i < 4; i++) {
    out[i] = std::rand() % 255 + 1;
  }
  return out;
}

const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
std::string Frame::getConnectionKey() {
  std::string out;
  for (int i = 0; i < 16; i++) {
    out += chars[std::rand() % chars.length()];
  }
  return out;
}

Frame Frame::read(SOCKET* s) {
  if (s == nullptr || *s == INVALID_SOCKET) {
    log.error("An invalid socket has been provied to Frame.read()");
    throw new std::invalid_argument("Provided socket is invalid");
  }
  char info_buf[2];
  int result = recvData(s, info_buf, sizeof(info_buf));

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
  
  std::array<byte, 4> maskingKey;
  if (masked) {
    char mask_buf[4];
    recvData(s, mask_buf, 4);
    for (int i = 0; i < 4; i++) {
      maskingKey[i] = mask_buf[i];
    }
  }
  
  char payload_buf[payloadLength];
  recvData(s, payload_buf, payloadLength);
  std::size_t size = sizeof(payload_buf) / sizeof(payload_buf[0]);
  std::vector<byte> payload(payload_buf, payload_buf + size);
  if (masked) {
    for (int i = 0; i < payloadLength; i++) {
      payload[i] = payload[i] ^ maskingKey[i % 4];
    }
  }

  return Frame(fin, opcode, masked, payloadLength, maskingKey, payload);
}

std::vector<char> Frame::getBytes() {
  if (payloadLength != payload.size()) {
    log.error("payload.size() does not match payloadLength when trying to call getBytes()");
    throw new std::invalid_argument("payload.size() does not match payloadLength when trying to call getBytes()");
  }
  int calcLength = 2 + payloadLength; // 2 = min len;
  if (payloadLength <= 65535 && payloadLength > 125) calcLength += 3; // 1 byte + next 2
  else if (payloadLength > 65535) calcLength += 9; // 1 byte + next 8

  if (masked && !validMaskingKey(maskingKey)) {
    log.error("Masked set to true but frame does not contain valid maskingKey");
    throw new std::invalid_argument("Masked set to true but frame does not contain valid maskingKey");
  } else {
    calcLength += 4;
  }

  std::vector<char> out;
  out.resize(calcLength);
  int idx = 0;

  if (fin) out[idx] |= 0b10000000;
  out[idx] |= static_cast<byte>(opcode);
  idx++;

  if (payloadLength <= 125) {
    if (masked) out[idx] |= 0b10000000;
    out[idx] |= payloadLength;
    idx += 1;
  } else if (payloadLength <= 65535){
    if (masked) out[idx] = (byte)(0b10000000);
    out[idx] |= 126;
    out[idx + 1] = (byte)((payloadLength >> 8) & 0xFF); 
    out[idx + 2] = (byte)(payloadLength & 0xFF);
    idx += 3;
  } else {
    if (masked) out[idx] = (byte)(0b10000000);
    out[idx] |= 127;
    idx += 1;
    for (int i = 7; i >= 0; i--) {
      out[idx + 8 - i] = (byte)((payloadLength >> (i * 8)) & 0xFF);
      idx += 1;
    }
  }

  if (masked) {
    for (int i = 0; i < 4; i++) {
      out[idx] = maskingKey[i];
      idx++;
    }
    for (int i = 0; i < payloadLength; i++) {
      out[idx] = payload[i] ^ maskingKey[i % 4];
      idx++;
    }
  } else {
    out.insert(out.begin() + idx, payload.begin(), payload.end());
  }
  return out;
}

void Frame::sendFrame(SOCKET* s) {
  std::vector<char> outvec = getBytes();
  char* outbuf = outvec.data();
  send(*s, outbuf, sizeof(outbuf), 0);
}
