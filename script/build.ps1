param(
    [ValidateSet('Debug','Release','Foxtrot_Editor_Debug')][string]$Configuration = 'Debug',
    [ValidateSet('x64','x86')][string]$Platform = 'x64'
)
$ErrorActionPreference = 'Stop'
$solutionRoot = Split-Path $PSScriptRoot -Parent
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$installation = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -property installationPath
if (!$installation) { throw 'Visual Studio with Desktop development with C++ is required.' }
$msbuild = Join-Path $installation 'MSBuild\Current\Bin\MSBuild.exe'
$logDirectory = Join-Path $solutionRoot 'build'
New-Item -ItemType Directory -Path $logDirectory -Force | Out-Null
$buildLog = Join-Path $logDirectory "$Configuration-$Platform.log"
& $msbuild (Join-Path $solutionRoot 'FoxtrotEngine_DirectX.sln') /m:2 "/p:Configuration=$Configuration" "/p:Platform=$Platform" /v:minimal /nologo /fl "/flp:logfile=$buildLog;verbosity=minimal"
exit $LASTEXITCODE
