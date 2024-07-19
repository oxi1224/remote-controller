import platform
import subprocess
import sys
import os

system = platform.system()
delim = "\\" if system == "Windows" else "/"
prod = "--prod" in sys.argv or "-P" in sys.argv
out_path = os.getcwd() + f"{delim}out{delim}{system}{delim}{'prod' if prod else 'dev'}"

if ("--help" in sys.argv or "-h" in sys.argv):
    print("     -T (--test) Runs test after compiling (does not work in production)")
    print("     -R (--run)  Runs the compiled application after finishing")
    print("     -P (--prod) Compiles in production mode")
    print("     -h (--help) Displays help information")
    exit(0)

def run(command, shell=False):
    joined = ' '.join(command)
    print(f"+{'-' * (len(joined) + 2)}+")
    print(f"| {joined} |")
    print(f"+{'-' * (len(joined) + 2)}+")

    process = subprocess.Popen(command, shell=shell, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    while True:
        output = process.stdout.readline()
        if output == "" and process.poll() is not None:
            break
        if output:
            print(output.strip())

    rc = process.poll()
    if rc is not None:
        remaining_output, error_output = process.communicate()
        if remaining_output:
            print(remaining_output.strip())
        if error_output:
            print(f"Error: {error_output.strip()}")

if system == 'Windows':
    cmake_cmd = ["cmake", "-B", out_path, "-G", "MinGW Makefiles"]
    if (prod):
        cmake_cmd.append("-DDEV_BUILD=OFF")
    run(cmake_cmd)
    run(["make", "-C", out_path])
    run(["copy", "/Y", out_path + f"{delim}compile_commands.json", f".{delim}"], True)
else:
    cmake_cmd = ["cmake" "-B", out_path]
    if (prod):
        cmake_cmd.append("-DDEV_BUILD=OFF")
    run(cmake_cmd)
    run(["make", "-C", out_path])

if (("--test" in sys.argv or "-T" in sys.argv) and prod):
    print("Tests cannot be ran in production")
if (("--test" in sys.argv or "-T" in sys.argv) and not prod):
    run(["ctest", "--test-dir", out_path])
if ("--run" in sys.argv or "-R" in sys.argv):
    run([f"{out_path}{delim}remote-controller-client"])
