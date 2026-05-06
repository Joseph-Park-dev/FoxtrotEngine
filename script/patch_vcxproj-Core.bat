@echo off
setlocal enabledelayedexpansion
:: ============================================================
:: Configuration
:: ============================================================
set VCXPROJ=.\src\Core\Core.vcxproj
set PS_TEMP=%~dp0patch_vcxproj_temp.ps1

:: ============================================================
:: Source files (.cpp) to add
:: ============================================================
set SOURCES[0]=$(SolutionDir)extern\imgui\imgui.cpp
set /a SRC_COUNT+=1
set SOURCES[1]=$(SolutionDir)extern\imgui\imgui_draw.cpp
set /a SRC_COUNT+=1
set SOURCES[2]=$(SolutionDir)extern\imgui\imgui_tables.cpp
set /a SRC_COUNT+=1
set SOURCES[3]=$(SolutionDir)extern\imgui\imgui_widgets.cpp
set /a SRC_COUNT+=1

:: ============================================================
:: Header files (.h) to add
:: ============================================================
::set HDR_COUNT=0
::set HEADERS[0]=include\foo.h
::set /a HDR_COUNT+=1
::set HEADERS[1]=include\bar.h
::set /a HDR_COUNT+=1

:: ============================================================
:: Write PowerShell script to temp file
:: ============================================================
set /a SRC_LAST=SRC_COUNT-1
set /a HDR_LAST=HDR_COUNT-1

echo $proj = '%VCXPROJ%'                                              > "%PS_TEMP%"
echo $sources = @(                                                    >> "%PS_TEMP%"
for /L %%i in (0,1,!SRC_LAST!) do (
    echo     '!SOURCES[%%i]!'                                         >> "%PS_TEMP%"
)
echo )                                                                >> "%PS_TEMP%"
echo $headers = @(                                                    >> "%PS_TEMP%"
for /L %%i in (0,1,!HDR_LAST!) do (
    echo     '!HEADERS[%%i]!'                                         >> "%PS_TEMP%"
)
echo )                                                                >> "%PS_TEMP%"
echo [xml]$xml = Get-Content $proj                                    >> "%PS_TEMP%"
echo $ns = 'http://schemas.microsoft.com/developer/msbuild/2003'     >> "%PS_TEMP%"
echo $nsmgr = New-Object System.Xml.XmlNamespaceManager($xml.NameTable) >> "%PS_TEMP%"
echo $nsmgr.AddNamespace('ms', $ns)                                  >> "%PS_TEMP%"
echo $srcGroup = $xml.SelectSingleNode('//ms:ItemGroup[ms:ClCompile]', $nsmgr) >> "%PS_TEMP%"
echo if (-not $srcGroup) { $srcGroup = $xml.CreateElement('ItemGroup', $ns); $xml.Project.AppendChild($srcGroup) ^| Out-Null } >> "%PS_TEMP%"
echo foreach ($f in $sources) {                                       >> "%PS_TEMP%"
echo   $existing = $srcGroup.SelectSingleNode("ms:ClCompile[@Include='$f']", $nsmgr) >> "%PS_TEMP%"
echo   if (-not $existing) {                                          >> "%PS_TEMP%"
echo     $el = $xml.CreateElement('ClCompile', $ns)                  >> "%PS_TEMP%"
echo     $el.SetAttribute('Include', $f)                             >> "%PS_TEMP%"
echo     $srcGroup.AppendChild($el) ^| Out-Null                      >> "%PS_TEMP%"
echo     Write-Host "[OK] Added source: $f"                          >> "%PS_TEMP%"
echo   } else { Write-Host "[!] Already present: $f" }               >> "%PS_TEMP%"
echo }                                                                >> "%PS_TEMP%"
echo $hdrGroup = $xml.SelectSingleNode('//ms:ItemGroup[ms:ClInclude]', $nsmgr) >> "%PS_TEMP%"
echo if (-not $hdrGroup) { $hdrGroup = $xml.CreateElement('ItemGroup', $ns); $xml.Project.AppendChild($hdrGroup) ^| Out-Null } >> "%PS_TEMP%"
echo foreach ($f in $headers) {                                       >> "%PS_TEMP%"
echo   $existing = $hdrGroup.SelectSingleNode("ms:ClInclude[@Include='$f']", $nsmgr) >> "%PS_TEMP%"
echo   if (-not $existing) {                                          >> "%PS_TEMP%"
echo     $el = $xml.CreateElement('ClInclude', $ns)                  >> "%PS_TEMP%"
echo     $el.SetAttribute('Include', $f)                             >> "%PS_TEMP%"
echo     $hdrGroup.AppendChild($el) ^| Out-Null                      >> "%PS_TEMP%"
echo     Write-Host "[OK] Added header: $f"                          >> "%PS_TEMP%"
echo   } else { Write-Host "[!] Already present: $f" }               >> "%PS_TEMP%"
echo }                                                                >> "%PS_TEMP%"
echo $xml.Save((Resolve-Path $proj))                                  >> "%PS_TEMP%"

:: ============================================================
:: Execute and clean up
:: ============================================================
powershell -NoProfile -ExecutionPolicy Bypass -File "%PS_TEMP%"
del "%PS_TEMP%"

echo.
echo Done patching %VCXPROJ%.