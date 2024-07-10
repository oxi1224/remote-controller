package io.github.oxi1224;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

class MainTest {
  @Test
  public void test() {
    Main inst = new Main();
    assertEquals("hello world", inst.test());
  }
}
