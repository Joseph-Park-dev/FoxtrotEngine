$ErrorActionPreference = 'Stop'
foreach ($platform in @('x64', 'x86')) {
    foreach ($configuration in @('Debug', 'Release', 'Foxtrot_Editor_Debug')) {
        & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'build.ps1') -Configuration $configuration -Platform $platform
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
        & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'test-modules.ps1') -Configuration $configuration -Platform $platform
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
        & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'test-cursor.ps1') -Configuration $configuration -Platform $platform
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
}
Write-Output 'All six solution configurations, DLL lifecycle tests and cursor regressions passed.'
