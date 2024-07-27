#include "opcode.hpp"

Opcode toOpcode(byte val) {
  switch (val) {
    case CLOSE:
    case CONNECT:
    case COMMAND:
    case RESULT:
    case PING:
    case PONG:
    case UNUSED:
      return static_cast<Opcode>(val);
    default:
      return UNUSED;
  }
}

bool opcode_equals(Opcode opcode, byte value) {
  return opcode == toOpcode(value);
}

