Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$BuildDir = Join-Path $ProjectRoot "build\mingw-debug"
$BinDir = Join-Path $ProjectRoot "bin\debug"

Push-Location $ProjectRoot
try {
    $ProcessRoots = @($BuildDir, $BinDir)

    Get-Process | ForEach-Object {
        $Proc = $_
        $ProcessPath = $null

        try {
            $ProcessPath = $Proc.Path
        }
        catch {
        }

        if ($null -ne $ProcessPath) {
            foreach ($Root in $ProcessRoots) {
                if ($ProcessPath.StartsWith($Root, [System.StringComparison]::OrdinalIgnoreCase)) {
                    Stop-Process -Id $Proc.Id -Force -ErrorAction SilentlyContinue
                    break
                }
            }
        }
    }

    if (Test-Path -LiteralPath $BuildDir) {
        Remove-Item -LiteralPath $BuildDir -Recurse -Force
    }

    if (Test-Path -LiteralPath $BinDir) {
        Remove-Item -LiteralPath $BinDir -Recurse -Force
    }
}
finally {
    Pop-Location
}
