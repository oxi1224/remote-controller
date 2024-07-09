package io.github.oxi1224.remotecontroller.server;

public class Frame {
  public final boolean fin;
  public final Opcode opcode;
  public final boolean encrypted;
  public final int payloadLength;
  public final byte[] encryptionKey;
  public final byte[] payload;

  public Frame(
    boolean fin,
    Opcode opcode,
    boolean encrypted,
    int payloadLength,
    byte[] encryptionKey,
    byte[] payload
  ) {
    this.fin = fin;
    this.opcode = opcode;
    this.encrypted = encrypted;
    this.payloadLength = payloadLength;
    this.encryptionKey = encryptionKey;
    this.payload = payload;
  }

  public byte[] getBytes() {
    int calcLength = 2 + payloadLength; // 2 = min len;
    if (payloadLength <= 65535 && payloadLength > 125) calcLength += 3; // 1 byte + next 2
    else if (payloadLength > 65535) calcLength += 9; // 1 byte + next 8
    if (encrypted && encryptionKey.length != 4) {
      throw new IllegalArgumentException("The encryption key must be an array of 4 bytes");
    } else if (encrypted) {
      calcLength += 4;
    }
    byte[] out = new byte[calcLength];
    int idx = 0;
    byte b = 0x0;
    if (fin) b |= 0b10000000;
    b |= opcode.getValue();
    out[idx] = (byte)(b & 0xFF);
    idx += 1;

    if (payloadLength <= 125) {
      b = 0x0;
      if (encrypted) b |= 0b10000000;
      b |= payloadLength;
      out[idx] = b;
      idx += 1;
    } else if (payloadLength <= 65535){
      if (encrypted) out[idx] = (byte)(0b10000000);
      out[idx] |= 126;
      out[idx + 1] = (byte)((payloadLength >> 8) & 0xFF); 
      out[idx + 2] = (byte)(payloadLength & 0xFF);
      idx += 3;
    } else {
      if (encrypted) out[idx] = (byte)(0b10000000);
      out[idx] |= 127;
      idx += 1;
      for (int i = 7; i >= 0; i--) {
        out[idx + 8 - i] = (byte)((payloadLength >> (i * 8)) & 0xFF);
        idx += 1;
      }
    }

    if (encrypted) {
      for (int i = 0; i < 4; i++) {
        out[idx + i] = encryptionKey[i];
        idx++;
      }
      /// TODO: Encryption
    } else {
      System.arraycopy(payload, 0, out, idx, payloadLength);
    }
    return out;
  }
}
