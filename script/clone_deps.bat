@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: Configuration
:: ============================================================
set BASE_DIR=%~dp0..\extern
set GIT=git

:: ============================================================
:: Repositories to clone
:: ============================================================
set REPOS[0]=https://github.com/ocornut/imgui.git
set REPOS[1]=https://github.com/aiekick/ImGuiFileDialog.git
set REPOS[2]=https://github.com/nlohmann/json.git

:: ============================================================
:: Clone loop
:: ============================================================
if not exist "%BASE_DIR%" mkdir "%BASE_DIR%"

call :clone "!REPOS[0]!" "docking"
call :clone "!REPOS[1]!"

echo.
echo All done.
exit /b 0

:: ============================================================
:: :clone subroutine
:: ============================================================
:clone
set REPO_URL=%~1
set BRANCH=%~2
for %%F in (%REPO_URL%) do set REPO_NAME=%%~nF

echo [*] Cloning %REPO_NAME% from %REPO_URL%...

if exist "%BASE_DIR%\%REPO_NAME%" (
    echo [!] Already exists, skipping: %REPO_NAME%
) else (
    if not "%BRANCH%"=="" (
        %GIT% clone -b "%BRANCH%" "%REPO_URL%" "%BASE_DIR%\%REPO_NAME%"
    ) else (
        %GIT% clone "%REPO_URL%" "%BASE_DIR%\%REPO_NAME%"
    )
    if errorlevel 1 (
        echo [ERROR] Failed to clone %REPO_NAME%
    ) else (
        echo [OK] Cloned %REPO_NAME%
        if not "%BRANCH%"=="" echo [OK] Checked out branch: %BRANCH%
    )
)
exit /b

:: ============================================================
:: Configuration
:: ============================================================
set VCXPROJ=src\Core\Core.vcxproj

:: Source files (.cpp) to add
set SOURCES[0]=extern\imgui\imgui.cpp
set SOURCES[1]=extern\imgui\imgui_draw.cpp
set SOURCES[1]=extern\imgui\imgui_tables.cpp
set SOURCES[1]=extern\imgui\imgui_widgetscpp

:: ============================================================
:: Build PowerShell argument lists
:: ============================================================
set SRC_LIST=
for /L %%i in (0,1,1) do (
    set SRC_LIST=!SRC_LIST!'!SOURCES[%%i]!',
)
set SRC_LIST=!SRC_LIST:~0,-1!

set HDR_LIST=
for /L %%i in (0,1,1) do (
    set HDR_LIST=!HDR_LIST!'!HEADERS[%%i]!',
)
set HDR_LIST=!HDR_LIST:~0,-1!

:: ============================================================
:: Invoke PowerShell to patch the .vcxproj
:: ============================================================
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$proj = '%VCXPROJ%';" ^
    "$sources = @(%SRC_LIST%);" ^
    "$headers = @(%HDR_LIST%);" ^
    "[xml]$xml = Get-Content $proj;" ^
    "$ns = 'http://schemas.microsoft.com/developer/msbuild/2003';" ^
    "$nsmgr = New-Object System.Xml.XmlNamespaceManager($xml.NameTable);" ^
    "$nsmgr.AddNamespace('ms', $ns);" ^
    "$srcGroup = $xml.SelectSingleNode('//ms:ItemGroup[ms:ClCompile]', $nsmgr);" ^
    "if (-not $srcGroup) { $srcGroup = $xml.CreateElement('ItemGroup', $ns); $xml.Project.AppendChild($srcGroup) | Out-Null; }" ^
    "foreach ($f in $sources) {" ^
    "  $existing = $srcGroup.SelectSingleNode(\"ms:ClCompile[@Include='$f']\", $nsmgr);" ^
    "  if (-not $existing) {" ^
    "    $el = $xml.CreateElement('ClCompile', $ns);" ^
    "    $el.SetAttribute('Include', $f);" ^
    "    $srcGroup.AppendChild($el) | Out-Null;" ^
    "    Write-Host \"[OK] Added source: $f\";" ^
    "  } else { Write-Host \"[!] Already present: $f\"; }" ^
    "}" ^
    "$hdrGroup = $xml.SelectSingleNode('//ms:ItemGroup[ms:ClInclude]', $nsmgr);" ^
    "if (-not $hdrGroup) { $hdrGroup = $xml.CreateElement('ItemGroup', $ns); $xml.Project.AppendChild($hdrGroup) | Out-Null; }" ^
    "foreach ($f in $headers) {" ^
    "  $existing = $hdrGroup.SelectSingleNode(\"ms:ClInclude[@Include='$f']\", $nsmgr);" ^
    "  if (-not $existing) {" ^
    "    $el = $xml.CreateElement('ClInclude', $ns);" ^
    "    $el.SetAttribute('Include', $f);" ^
    "    $hdrGroup.AppendChild($el) | Out-Null;" ^
    "    Write-Host \"[OK] Added header: $f\";" ^
    "  } else { Write-Host \"[!] Already present: $f\"; }" ^
    "}" ^
    "$xml.Save((Resolve-Path $proj));"

echo.
echo Done patching %VCXPROJ%.
pause