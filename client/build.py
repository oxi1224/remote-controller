import platform
import subprocess
import sys
import os
import shutil
import threading

system = platform.system()
delim = "\\" if system == "Windows" else "/"
prod = "--prod" in sys.argv or "-p" in sys.argv
test_build = "--test" in sys.argv or "-t" in sys.argv
sub_dir = "dev"

if (prod):
    sub_dir = "prod"
elif (test_build):
    sub_dir = "test"
out_path = os.getcwd() + f"{delim}out{delim}{system}{delim}{sub_dir}"

if (test_build and prod):
    print("Cannot have both testing and production build")
    exit(1);

if ("--help" in sys.argv or "-h" in sys.argv):
    print("     -t (--test)  Runs test after compiling (does not work in production)")
    print("     -r (--run)   Runs the compiled application after finishing")
    print("     -c (--clean) Cleans the output directory before compiling")
    print("     -p (--prod)  Compiles in production mode")
    print("     -h (--help)  Displays help information")
    exit(0)

def print_boxed(str):
    print(f"+{'-' * (len(str) + 2)}+")
    print(f"| {str} |")
    print(f"+{'-' * (len(str) + 2)}+")

if ("--clean" in sys.argv or "-c" in sys.argv):
    print_boxed("Cleaning output directory")
    shutil.rmtree(out_path)

def run(command, shell=False):
    print_boxed(' '.join(command))
    process = subprocess.Popen(command, shell=shell, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    def print_lines(stream, prefix=""):
        for line in iter(stream.readline, ''):
            print(f"{prefix}{line}", end='')

    stdout_thread = threading.Thread(target=print_lines, args=(process.stdout,))
    stderr_thread = threading.Thread(target=print_lines, args=(process.stderr, "Error: "))

    stdout_thread.start()
    stderr_thread.start()

    stdout_thread.join()
    stderr_thread.join()

    process.wait()
    if process.returncode != 0:
        print_boxed("An error occurred during compilation")
        exit(1)

if system == 'Windows':
    cmake_cmd = ["cmake", "-B", out_path, "-G", "MinGW Makefiles"]
    if (prod):
        cmake_cmd.append("-DDEV_BUILD=OFF")
    if (test_build):
        cmake_cmd.append("-DTEST_BUILD=ON")
    run(cmake_cmd)
    run(["make", "-C", out_path])
    run(["copy", "/Y", out_path + f"{delim}compile_commands.json", f".{delim}"], True)
    if (test_build):
        run(["ctest", "--output-on-failure", "--test-dir", out_path])

else:
    cmake_cmd = ["cmake" "-B", out_path]
    if (prod):
        cmake_cmd.append("-DDEV_BUILD=OFF")
    if (test_build):
        cmake_cmd.append("-DTEST_BUILD=ON")
    run(cmake_cmd)
    run(["make", "-C", out_path])
    if (test_build):
        run(["ctest","--output-on-failure", "--test-dir", out_path])

if ("--run" in sys.argv or "-r" in sys.argv):
    run([f"{out_path}{delim}remote-controller-client"])
