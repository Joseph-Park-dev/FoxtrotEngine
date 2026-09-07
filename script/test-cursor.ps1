param(
    [ValidateSet('Debug','Release','Foxtrot_Editor_Debug')][string]$Configuration = 'Foxtrot_Editor_Debug',
    [ValidateSet('x64','x86')][string]$Platform = 'x64'
)
$ErrorActionPreference = 'Stop'
$solutionRoot = Split-Path $PSScriptRoot -Parent
$nativePlatform = if ($Platform -eq 'x86') { 'Win32' } else { 'x64' }
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$installation = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -property installationPath
if (!$installation) { throw 'Visual Studio C++ is required.' }
$msbuild = Join-Path $installation 'MSBuild\Current\Bin\MSBuild.exe'
& $msbuild (Join-Path $solutionRoot 'tests\CursorRegression.vcxproj') /m:2 /nologo /v:quiet "/p:Configuration=$Configuration" "/p:Platform=$nativePlatform" "/p:SolutionDir=$solutionRoot\"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
$binaryDirectory = Join-Path $solutionRoot "build\$nativePlatform\$Configuration"
$outputLog = Join-Path $binaryDirectory 'cursor.stdout.log'
$errorLog = Join-Path $binaryDirectory 'cursor.stderr.log'
$process = Start-Process -FilePath (Join-Path $binaryDirectory 'CursorRegression.exe') -WorkingDirectory $binaryDirectory -WindowStyle Hidden -PassThru -RedirectStandardOutput $outputLog -RedirectStandardError $errorLog
$process.Handle | Out-Null
if (!$process.WaitForExit(30000)) { Stop-Process -Id $process.Id; throw 'Cursor regression timed out.' }
$code = $process.ExitCode
$process.Dispose()
if ($code -ne 0) { throw "Cursor regression exited $code : $(Get-Content -LiteralPath $errorLog -Raw)" }
Write-Output "PASS $Configuration $Platform cursor regression"
