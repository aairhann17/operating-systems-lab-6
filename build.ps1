$ErrorActionPreference = 'Stop'

$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    Write-Error "gcc not found. Install MinGW-w64 or LLVM/Clang and add it to PATH."
}

Write-Host "Compiling project..."
gcc -std=c11 -Wall -Wextra -pedantic group29_manager.c -o group29_manager.exe
Write-Host "Build successful: group29_manager.exe"
