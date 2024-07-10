package io.github.oxi1224.remotecontroller.server;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

class FrameTest {
  @Test
  public void testToBytes() {
    Frame f = new Frame(
      true,
      Opcode.PING,
      false,
      11,
      null,
      new byte[]{0x48, 0x45, 0x4C, 0x4C, 0x4F, 0x20, 0x57, 0x4F, 0x52, 0x4C, 0x44}
    );

    byte[] bytes = {(byte)0b10001101, 0b1011, 0x48, 0x45, 0x4C, 0x4C, 0x4F, 0x20, 0x57, 0x4F, 0x52, 0x4C, 0x44};
    byte[] out = f.getBytes(); 

    assertEquals(bytes.length, out.length, "Outputted buffer has a different length than expected");
    for (int i = 0; i < bytes.length; i++) {
      assertEquals(bytes[i] & 0xFF, out[i] & 0xFF, String.format("Bytes at index %s do not match", i));
    }
  }
}
