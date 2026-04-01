$ErrorActionPreference = 'Stop'

$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    Write-Error "gcc not found. Install MinGW-w64 or LLVM/Clang and add it to PATH."
}

Write-Host "Compiling project..."
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/main.c src/group29_manager.c -o groupX_manager.exe
Write-Host "Build successful: groupX_manager.exe"
