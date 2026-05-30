param(
    [switch]$Fresh
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

Push-Location $ProjectRoot
try {
    if ($Fresh) {
        & (Join-Path $PSScriptRoot "clean.ps1")
    }

    cmake --preset mingw-debug
    cmake --build --preset mingw-debug-build
    ctest --test-dir build/mingw-debug --output-on-failure
}
finally {
    Pop-Location
}
