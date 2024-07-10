package io.github.oxi1224.remotecontroller.server;

public enum Opcode {
  CLOSE(0x0),
  CONNECT(0x1),

  COMMAND(0x6),
  RESULT(0x7),

  PING(0xD),
  PONG(0xE),
  UNUSED(0xF);

  public static Opcode findByValue(int value) {
    for (Opcode c : values()) {
      if (c.getValue() == value) return c;
    }
    return Opcode.UNUSED;
  }
  private final byte code;
  Opcode(int code) { this.code = (byte)code; }
  public byte getValue() { return this.code; }
}
