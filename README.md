# Operating Systems Lab 6 (Virtual Memory Manager)

This project implements virtual-address translation with:

- Input: 32-bit logical addresses from standard input (`stdin`)
- Output per address:
  - `Virtual address: <v> Physical address: <p> Value: <value>`

## Project Structure

- `main.c` - Program entry point
- `group29_manager.c` - Main virtual memory manager
- `group29_manager.h` - Header for main manager
- `group29_bonus.c` - Bonus version (128 frames + FIFO replacement)
- `Makefile` - Build and run targets
- `build.ps1` - PowerShell build script

## Run in WSL / Linux

From the repository root:

```bash
cd /mnt/c/Users/aaraa/operating-systems-lab-6
```

### Main application (`group29_manager`)

Build:

```bash
make
```

Run with provided input file:

```bash
./group29_manager < addresses.txt
```

Run with custom backing store path:

```bash
./group29_manager /path/to/BACKING_STORE.bin < addresses.txt
```

### Bonus application (`group29_bonus`)

Build:

```bash
make bonus
```

Run:

```bash
./group29_bonus BACKING_STORE.bin < addresses.txt
```

One-command build + run:

```bash
make bonus-run
```

Override default input/backing-store files:

```bash
make bonus-run ADDRESSES_FILE=/path/to/addresses.txt BACKING_STORE_FILE=/path/to/BACKING_STORE.bin
```

## Run in PowerShell (Windows)

Build main executable:

```powershell
./build.ps1
```

Run main executable with `addresses.txt` via stdin:

```powershell
Get-Content addresses.txt | .\group29_manager.exe
```

## Submission Note

Do not include executables in the final tar.gz submission.

Clean build artifacts first:

```bash
make clean
```

## Notes

- The program masks each input value to the lower 16 bits before translation.
- Main version uses 256-byte pages, 256 logical pages, 16 TLB entries, and 256 physical frames.
- Bonus version uses 128 physical frames with FIFO page replacement.
- Final statistics are printed to three decimal places.
