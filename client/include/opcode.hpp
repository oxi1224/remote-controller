#pragma once

typedef unsigned char byte;

enum Opcode : byte {
  CLOSE = 0x0,
  CONNECT = 0x1,

  COMMAND = 0x6,
  RESULT = 0x7,

  PING = 0xD,
  PONG = 0xE,
  UNUSED = 0xF
};

Opcode toOpcode(byte val);
bool opcode_equals(Opcode opcode, byte value);
