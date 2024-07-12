package io.github.oxi1224.remotecontroller.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Base64;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import io.github.oxi1224.common.Pair;
import io.github.oxi1224.logging.Logger;

public class Server extends java.net.ServerSocket {
  private Logger logger = Logger.getLogger(Server.class);
  public static final int VERSION = 1;
  public HashMap<Socket, Thread> clients = new HashMap<>();
  public Map<Socket, List<Pair<String, String>>> clientCommands = Collections.synchronizedMap(new HashMap<>());
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
    int clientVersion = f.payload[0];
    if (clientVersion != VERSION || f.payloadLength != 17) { // 17 = 1 byte for version + 16 for the key
      logger.warnf("Version mismatch from client %s (expected %s, received %s)", s.getInetAddress().toString(), VERSION, clientVersion);
      out.write(Frame.getClose("Version mismatch, required version: " + VERSION, false).getBytes());
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
      logger.error(e);
      out.write(Frame.getClose("Server side error, please try again", false).getBytes());
      s.close();
    }
    byte[] encoded = Base64.getEncoder().encode(sha1);
    f = new Frame(false, Opcode.CONNECT, false, encoded.length, null, encoded);
    out.write(f.getBytes());
    startTimeout(10_000, s);
    f = Frame.read(in);
    if (f.fin && f.opcode == Opcode.CONNECT) {
      handleConnection(s);
    } else {
      try {
        out.write(Frame.getClose("Expected last frame to have FIN = true and Opcode = CONNECT", false).getBytes());
        s.close();
      } catch (IOException e) {} // Socket was closed by client
    }
    in.close();
    out.close();
  }

  private void handleConnection(Socket s) {
    Thread t = new Thread(() -> {
      try {
        InputStream in = s.getInputStream();
        OutputStream out = s.getOutputStream();
        while (!Thread.interrupted()) {
          Frame f = Frame.read(in);
          switch (f.opcode) {
            case PING:
              out.write(Frame.getGeneric(Opcode.PING, false).getBytes());
              break;
            case CLOSE:
              out.write(Frame.getGeneric(Opcode.CLOSE, false).getBytes());
              break;
            case RESULT:
              break;
            // Invalid frames, close connection
            case UNUSED:
            case PONG:
            case COMMAND:
            case CONNECT:
              s.close();
              break;
          }
        }
        in.close();
        out.close();
      } catch (IOException e) {
        logger.errorf("Error in client thread %s", e, Thread.currentThread().toString());
      }
    });
    clients.put(s, t);
    t.start();
  }

  public void start() throws IOException {
    Socket client = accept();
    acceptConnection(client);
  }

  public synchronized void sendCommand(Socket s, String cmd) throws IOException {
    OutputStream out = s.getOutputStream();
    InputStream in = s.getInputStream();
    byte[] cmdBuf = cmd.getBytes();
    Frame f = new Frame(true, Opcode.COMMAND, false, cmdBuf.length, null, cmdBuf);
    out.write(f.getBytes());
    Frame res = Frame.read(in);
    if (res.opcode != Opcode.RESULT) {
      logger.infof("Client %s replied with %s instead of RESULT after COMMAND frame", s.getInetAddress().toString(), res.opcode.toString());
      s.close();
      return;
    }
    // Think on this, Opcode.RESULT can also be in handleConnection()
    String cmdResult = new String(res.payload);
    if (!clientCommands.containsKey(s)) clientCommands.put(s, Collections.synchronizedList(new ArrayList<Pair<String, String>>()));
    clientCommands.get(s).add(new Pair<>(cmd, cmdResult));
    out.close(); 
    in.close();
  }

  private void startTimeout(long delay, Socket s) {
    timer = new Timer();
    timer.schedule(new TimerTask() {
      @Override
      public void run() {
        try {
          logger.infof("Closing connection %s after %s ms without response", s.getInetAddress().toString(), delay);
          s.getOutputStream().write(Frame.getClose("Connection timed out", false).getBytes());
          s.close();
        } catch (IOException e) {
          logger.errorf("Exception while closing connection from %s", e, s.getInetAddress().toString());
        }
      }
    }, delay);
  }
}
