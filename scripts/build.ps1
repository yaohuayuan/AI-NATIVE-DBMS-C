Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

cmake --preset mingw-debug
cmake --build --preset mingw-debug-build
