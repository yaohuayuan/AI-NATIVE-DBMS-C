# Build And Run

This project currently targets Windows with MinGW and CMake presets.

## Requirements

- Windows PowerShell
- CMake
- MinGW-w64
- CTest

## Configure

```powershell
cmake --preset mingw-debug
```

## Build

```powershell
cmake --build --preset mingw-debug-build
```

## Run

```powershell
.\bin\debug\aidb.exe
```

## Test

```powershell
ctest --test-dir build/mingw-debug --output-on-failure
```

Default tests must not require network access, API keys, or calls to a real API.
