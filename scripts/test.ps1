Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

ctest --test-dir build/mingw-debug --output-on-failure
