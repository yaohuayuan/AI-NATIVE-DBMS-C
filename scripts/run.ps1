Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

cmake --build --preset mingw-debug-build
& "$PSScriptRoot\..\bin\debug\aidb.exe"
