# Operating Systems Lab 6 (C Starter)

This is a C project scaffold for an Operating Systems Lab 6 assignment using Banker's algorithm.

## Project Structure

- `src/main.c` - Program entry point
- `src/lab6.c` - Banker's algorithm implementation (safe-state + request handling)
- `include/lab6.h` - Function declarations
- `Makefile` - Build and run shortcuts for `make`
- `build.ps1` - PowerShell build script for Windows
- `tests/` - Sample input files and a simple shell test runner

## Build (GCC)

```powershell
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/main.c src/lab6.c -o lab6.exe
```

## Run

```powershell
./lab6.exe
```

In WSL/Linux builds, if you compile to `lab6`:

```bash
./lab6
```

## Program Modes

At runtime, the program supports:

- Built-in example case (classic safe sequence)
- Custom input case (enter process/resource counts and matrices)
- One resource request simulation after the initial safety check

The implementation includes both common Lab 6 tasks:

- Safety algorithm (`is_safe_state`)
- Resource-request algorithm (`request_resources`)

## Build + Run with Make

If you have `make` installed:

```powershell
make
make run
```

## Build + Run with PowerShell Script

```powershell
./build.ps1
./lab6.exe
```

## Run Sample Tests (WSL/Linux)

```bash
chmod +x tests/run_tests.sh
./tests/run_tests.sh
```

Sample input files:

- `tests/safe_builtin.in`
- `tests/unsafe_custom.in`
- `tests/request_granted_builtin.in`

## Notes

- If your instructor's exact Lab 6 format differs, you can keep the same core logic and only adjust input/output formatting in `src/lab6.c`.
