# Operating Systems Lab 6 (Virtual Memory Manager)

This project targets the virtual-address translation lab format shown in your files:

- Input: 32-bit logical addresses from standard input (`stdin`)
- Output: one line per address in the exact style:
	- `Virtual address: <v> Physical address: <p> Value: <value>`

## Project Structure

- `src/main.c` - Program entry point
- `src/group29_manager.c` - Virtual memory translation logic using page table + backing store
- `include/group29_manager.h` - Function declarations
- `Makefile` - Builds required executable name `groupX_manager`
- `build.ps1` - PowerShell build script for Windows

## Build (GCC)

```powershell
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/main.c src/group29_manager.c -o groupX_manager
```

## Run (Required Style)

WSL/Linux:

```bash
./groupX_manager < addresses.txt
```

PowerShell:

```powershell
Get-Content addresses.txt | .\groupX_manager.exe
```

## Run (Custom Backing Store Path)

If `BACKING_STORE.bin` is not in the working directory, pass it as the first argument.

```bash
./groupX_manager /path/to/BACKING_STORE.bin < addresses.txt
```

## Notes

- The program masks every input number to the lower 16 bits before translation.
- The implementation uses 256-byte pages, 256 logical pages, 16 TLB entries, and 256 physical frames.
- Output is printed in `correct.txt` style:
	- `Virtual address: <logical> Physical address: <physical> Value: <signed_byte>`
- Final statistics are printed to three decimal places.

## Build with Make

```bash
make
```

## Build with PowerShell Script

```powershell
./build.ps1
```

## Optional Bonus (128 Frames + FIFO Replacement)

Build with Make:

```bash
make bonus
```

Build and run in one command:

```bash
make bonus-run
```

Override input and backing-store paths if needed:

```bash
make bonus-run ADDRESSES_FILE=/path/to/addresses.txt BACKING_STORE_FILE=/path/to/BACKING_STORE.bin
```

Compile:

```bash
gcc -std=c11 -Wall -Wextra -pedantic group29_bonus.c -o group29_bonus
```

Run with stdin addresses:

```bash
./group29_bonus /path/to/BACKING_STORE.bin < addresses.txt
```

The bonus program keeps the same per-address output format and adds:

- `Page Replacements = <count>`
