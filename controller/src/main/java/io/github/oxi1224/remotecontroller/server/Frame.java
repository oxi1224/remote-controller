package io.github.oxi1224.remotecontroller.server;

import java.io.IOException;
import java.io.InputStream;

public class Frame {
  public final boolean fin;
  public final Opcode opcode;
  public final boolean masked;
  public final int payloadLength;
  public final byte[] maskingKey;
  public final byte[] payload;

  public Frame(
    boolean fin,
    Opcode opcode,
    boolean masked,
    int payloadLength,
    byte[] maskingKey,
    byte[] payload
  ) {
    this.fin = fin;
    this.opcode = opcode;
    this.masked = masked;
    this.payloadLength = payloadLength;
    this.maskingKey = maskingKey == null ? new byte[0] : maskingKey;
    this.payload = payload == null ? new byte[0] : payload;
  }

  public byte[] getBytes() {
    int calcLength = 2 + payloadLength; // 2 = min len;
    if (payloadLength <= 65535 && payloadLength > 125) calcLength += 3; // 1 byte + next 2
    else if (payloadLength > 65535) calcLength += 9; // 1 byte + next 8
    if (masked && maskingKey.length != 4) {
      throw new IllegalArgumentException("The masking key must be an array of 4 bytes");
    } else if (masked ) {
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
      if (masked) b |= 0b10000000;
      b |= payloadLength;
      out[idx] = b;
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
      /// TODO: masking
    } else {
      System.arraycopy(payload, 0, out, idx, payloadLength);
    }
    return out;
  }

  public static Frame read(InputStream in) throws IOException {
    byte b = (byte)(in.read()); 
    boolean fin = (b & 0x80) != 0;
    Opcode opcode = Opcode.findByValue(b & 0x0F);

    b = (byte)(in.read());
    boolean masked = (0x80 & b) != 0;
    int payloadLength = (byte)(0x7F & b);
    int bytesToRead = 0;
    if (payloadLength == 126) bytesToRead = 2;
    if (payloadLength == 127) bytesToRead = 8;
    if (bytesToRead != 0) {
      byte[] bytes = in.readNBytes(bytesToRead);
      payloadLength = 0; 
      for (byte _b : bytes) {
        payloadLength = (payloadLength << 8) + (_b & 0xFF);
      }
    };
    byte[] maskingKey = null;
    if (masked) {
      maskingKey = new byte[4];
      in.read(maskingKey, 0, 4);
    }
    byte[] payload = new byte[payloadLength];
    in.read(payload, 0, payloadLength);
    return new Frame(fin, opcode, masked, payloadLength, maskingKey, payload);
  }

  public static byte[] getMaskingKey() {
    byte[] key = new byte[4];
    for (int i = 0; i < 4; i++) {
      key[i] = (byte)(Math.random() * 255);
    }
    return key;
  }

  public static Frame getGeneric(Opcode opcode, boolean masked) {
    byte[] key = masked ? getMaskingKey() : null;
    return new Frame(true, opcode, masked, 0, key, null);
  }

  public static Frame getClose(String reason, boolean masked) {
    byte[] key = masked ? getMaskingKey() : null;
    byte[] payload = reason.getBytes();
    return new Frame(true, Opcode.CLOSE, masked, payload.length, key, payload);
  }
}
