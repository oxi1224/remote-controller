package io.github.oxi1224.logging;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class Logger {
  public static final String ANSI_RESET = "\033[0m";
  public static final String ANSI_RED = "\033[38;5;160m";
  public static final String ANSI_GREEN = "\033[38;5;157m";
  public static final String ANSI_YELLOW = "\033[38;5;220m";
  public static final String ANSI_GRAY = "\033[38;5;251m";
  public static final String ANSI_WHITE = "\033[37m";

  private static String logFilePath = "";
  private static DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("yyyy-MM-dd | HH:mm:ss");
  private String filePath;
  private Class<?> clazz;

  private Logger(Class<?> clazz, String filePath) {
    this.clazz = clazz ;
    this.filePath= filePath;
  }

  public void setLogFilePath(String logFilePath) {
    Logger.logFilePath = logFilePath;
  }

  public static Logger getLogger(Class<?> clazz) {
    return getLogger(clazz, "log.txt");
  }

  public static Logger getLogger(Class<?> clazz, String fileName) {
    String filePath = logFilePath + fileName;
    File logFile = new File(filePath);
    if (!logFile.exists()) {
      try {
        logFile.createNewFile();
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
    if (!logFile.canWrite()) {
      throw new IllegalArgumentException(String.format("File %s cannot be written to", filePath));
    }
    return new Logger(clazz, filePath);
  }

  public void info(String text) {
    write("[INFO] " + text, ANSI_WHITE);
  }

  public void infof(String text, Object... fargs) {
    info(String.format(text, fargs)); 
  }

  public void debug(String text) {
    write("[DEBUG] " + text, ANSI_GRAY);
  }

  public void debugf(String text, Object... fargs) {
    debug(String.format(text, fargs)); 
  }

  public void warn(String text) {
    write("[WARN] " + text, ANSI_YELLOW);
  }

  public void warnf(String text, Object... fargs) {
    warn(String.format(text, fargs));
  }

  public void error(String text) {
    write("[ERROR] " + text, ANSI_RED);
  }

  public synchronized void error(String text, Throwable throwable) {
    StringBuilder sb = new StringBuilder();
    if (!text.isBlank()) sb.append(text + " ");
    sb.append(throwable.getMessage());
    sb.append(" [");
    sb.append(clazz.getPackageName() + ".");
    sb.append(clazz.getName() + ".");
    sb.append(throwable.getStackTrace()[0].getMethodName());
    sb.append("]: ");
    sb.append(throwable.getClass().getPackageName() + ".");
    sb.append(throwable.getClass().getName());
    sb.append("\n\t");
    StringWriter sw = new StringWriter();
    PrintWriter pw = new PrintWriter(sw);
    throwable.printStackTrace(pw);
    sb.append(sw.toString());
    sb.delete(sb.length() - 2, sb.length()); // Remove the last newline

    error(sb.toString());
  }

  public void error(Throwable throwable) {
    error("", throwable);
  }

  public void errorf(String text, Object... fargs) {
    error(String.format(text, fargs)); 
  }

  public void errorf(String text, Throwable throwable, Object... fargs) {
    error(String.format(text, fargs), throwable);
  }

  private synchronized void write(String text, String ansiColor) {
    try {
      FileWriter w = new FileWriter(filePath, true);
      String dateStr = "[" + LocalDateTime.now().format(dateFormatter) + "]  ";
      w.write(dateStr + text + "\n");
      System.out.println(ANSI_GREEN + dateStr + ANSI_RESET + ansiColor + text + ANSI_RESET);
      w.close();
    } catch (IOException e) {
      e.printStackTrace();  
    }
  }
}
