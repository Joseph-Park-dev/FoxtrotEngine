param(
    [ValidateSet('Debug','Release','Foxtrot_Editor_Debug')][string]$Configuration = 'Debug',
    [ValidateSet('x64','x86')][string]$Platform = 'x64'
)
$ErrorActionPreference = 'Stop'
$solutionRoot = Split-Path $PSScriptRoot -Parent
$outputPlatform = if ($Platform -eq 'x86') { 'Win32' } else { 'x64' }
$binaryDirectory = Join-Path $solutionRoot "build\$outputPlatform\$Configuration"
$application = if ($Configuration -eq 'Foxtrot_Editor_Debug') { 'FoxtrotEditor.exe' } else { 'Game.exe' }
$executable = Join-Path $binaryDirectory $application
if (!(Test-Path -LiteralPath $executable)) { throw "Build first: $executable" }
$testDirectory = Join-Path $binaryDirectory 'test-data'
New-Item -ItemType Directory -Path $testDirectory -Force | Out-Null
$missingDirectory = Join-Path $testDirectory ("missing-game-" + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $missingDirectory | Out-Null
Copy-Item -LiteralPath $executable -Destination $missingDirectory
Get-ChildItem -LiteralPath $binaryDirectory -Filter '*.dll' | Where-Object Name -ne 'Game.dll' | Copy-Item -Destination $missingDirectory
foreach ($case in @(@{Name='reload'; Argument='--reload-test'; Exit=0}, @{Name='rollback'; Argument='--fail-after-runtime'; Exit=1}, @{Name='missing-game'; Argument='--smoke-test'; Exit=1})) {
    $errorLog = Join-Path $binaryDirectory "$($case.Name).stderr.log"
    $outputLog = Join-Path $binaryDirectory "$($case.Name).stdout.log"
    $caseExecutable = if ($case.Name -eq 'missing-game') { Join-Path $missingDirectory $application } else { $executable }
    $process = Start-Process -FilePath $caseExecutable -ArgumentList $case.Argument -WorkingDirectory $testDirectory -WindowStyle Hidden -PassThru -RedirectStandardError $errorLog -RedirectStandardOutput $outputLog
    $process.Handle | Out-Null
    if (!$process.WaitForExit(30000)) { Stop-Process -Id $process.Id; throw "$($case.Name) timed out" }
    $exitCode = $process.ExitCode
    $process.Dispose()
    $trace = Get-Content -LiteralPath $errorLog -Raw
    if ($exitCode -ne $case.Exit) { throw "$($case.Name) exited $exitCode`n$trace" }
    if ($case.Name -eq 'reload' -and ([regex]::Matches($trace, 'Game update system running')).Count -ne 2) { throw 'Both game sessions must execute their registered update callback.' }
    if ($case.Name -eq 'rollback' -and $trace -notmatch 'Injected startup failure') { throw 'Failure injection was not exercised.' }
    if ($case.Name -eq 'missing-game' -and ($trace -notmatch 'Cannot load Game' -or $trace -notmatch 'Unloading D3D11')) { throw 'Missing Game DLL must unwind the initialized graphics module.' }
    if ($trace -notmatch 'Unloading Core') { throw 'Core cleanup was not reached.' }
    Write-Output "PASS $Configuration $Platform $($case.Name)"
}
