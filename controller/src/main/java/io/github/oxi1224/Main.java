package io.github.oxi1224;

import java.io.IOException;

import io.github.oxi1224.remotecontroller.server.Server;

public class Main {
  public String test() {
    return "hello world";
  }

  public static void main(String[] args) throws IOException {
    Server s = new Server(6000);
    s.start();
    s.close();
  }
}
