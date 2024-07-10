package io.github.oxi1224.remotecontroller.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;
import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;

public class Server extends java.net.ServerSocket {
  public static final int VERSION = 1;
  public HashMap<Socket, Thread> clients = new HashMap<>();
  private Timer timer;

  public Server(int port) throws IOException {
    super(port);
  }

  public Server(int port, int backlog) throws IOException {
    super(port, backlog);
  }

  public Server(int port, int backlog, InetAddress bindAddr) throws IOException {
    super(port, backlog, bindAddr);
  }

  private void acceptConnection(Socket s) throws IOException {
    InputStream in = s.getInputStream();
    OutputStream out = s.getOutputStream();
    startTimeout(10_000, s);
    Frame f = Frame.read(in);
    timer.cancel();
    int version = f.payload[0];
    if (version != VERSION || f.payloadLength != 17) { // 17 = 1 byte for version + 16 for the key
      /// TODO: Proper response for this
      s.close();
    }
    byte[] keyBytes = new byte[16]; 
    System.arraycopy(f.payload, 1, keyBytes, 0, keyBytes.length);
    String key = new String(keyBytes);
    byte[] sha1 = new byte[0];
    try {
      sha1 = MessageDigest.getInstance("SHA-1")
      .digest((key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
        .getBytes(StandardCharsets.UTF_8));
    } catch (NoSuchAlgorithmException e) {
      s.close();
      e.printStackTrace();
    }
    byte[] encoded = Base64.getEncoder().encode(sha1);
    f = new Frame(false, Opcode.CONNECT, false, encoded.length, new byte[0], encoded);
    out.write(f.getBytes());
    startTimeout(10_000, s);
    f = Frame.read(in);
    if (f.fin && f.opcode == Opcode.CONNECT) {
      handleConnection(s);
    } else {
      try {
        s.close();
      } catch (IOException e) {} // Socket was closed by client
    }
    in.close();
    out.close();
  }

  private void handleConnection(Socket s) {
    Thread t = new Thread(() -> {
      while (!Thread.interrupted()) {
        try {
          InputStream in = s.getInputStream();
          Frame f = Frame.read(in);

          switch (f.opcode) {
            case PING:
            break;
            case PONG:
            break;
            case CONNECT:
            break;
            case CLOSE:
            break;
            case UNUSED:
            break;
            case COMMAND:
            break;
            case RESULT:
            break;
          }
        } catch (IOException e) {
          e.printStackTrace();
          break;
        }
      }
    });
    clients.put(s, t);
    t.start();
  }

  public void start() throws IOException {
    Socket client = accept();
    acceptConnection(client);
  }

  private void startTimeout(long delay, Socket s) {
    timer = new Timer();
    timer.schedule(new TimerTask() {
      @Override
      public void run() {
        try {
          s.close();
        } catch (IOException err) {
          System.out.println("Exception while trying to close a connection");
          err.printStackTrace();
        }
      }
    }, delay);
  }
}
