@echo off
setlocal EnableExtensions

rem Unloads Game.dll in a running FoxtrotEditor, builds the Game project, then loads Game.dll again.
rem Usage: build-game.bat [Configuration] [Platform]
rem Defaults: Foxtrot_Editor_Debug x64

set "CONFIG=%~1"
set "PLATFORM=%~2"
if "%CONFIG%"=="" set "CONFIG=Foxtrot_Editor_Debug"
if "%PLATFORM%"=="" set "PLATFORM=x64"

pushd "%~dp0.."
set "ROOT=%CD%"
popd

set "REQ_DIR=%TEMP%\FoxtrotEngine"
set "REQ=%REQ_DIR%\game-dll.req"
set "ACK=%REQ_DIR%\game-dll.ack"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo vswhere.exe was not found. Install Visual Studio with the C++ workload.
    exit /b 1
)

set "VSINSTALL="
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do set "VSINSTALL=%%I"
if not defined VSINSTALL (
    echo Visual Studio with MSBuild was not found.
    exit /b 1
)

set "MSBUILD=%VSINSTALL%\MSBuild\Current\Bin\MSBuild.exe"
if not exist "%MSBUILD%" (
    echo MSBuild.exe was not found.
    exit /b 1
)

if not exist "%REQ_DIR%" mkdir "%REQ_DIR%"

tasklist /FI "IMAGENAME eq FoxtrotEditor.exe" | find /I "FoxtrotEditor.exe" >nul
if errorlevel 1 (
    echo FoxtrotEditor.exe is not running. Building Game.dll.
    goto :BuildGame
)

echo Unloading Game.dll...
call :RequestGameDll unload
if errorlevel 1 exit /b 1

:BuildGame
echo Building Game ^(%CONFIG%^|%PLATFORM%^)...
rem Link beside the editor, then copy Game.dll into the project Plugins folder.
rem A unique PDB avoids LNK1201 while the debugger still has the previous PDB open.
rem Linking straight into Plugins deletes Game.dll when that write fails.
"%MSBUILD%" "%ROOT%\FoxtrotEngine_DirectX.sln" /t:Game /m /nologo /v:minimal /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /p:BuildProjectReferences=false /p:HotReloadId=%RANDOM%%RANDOM%
if errorlevel 1 exit /b 1

tasklist /FI "IMAGENAME eq FoxtrotEditor.exe" | find /I "FoxtrotEditor.exe" >nul
if errorlevel 1 (
    echo Built Game.dll. FoxtrotEditor.exe is not running, so it was not reloaded.
    exit /b 0
)

echo Reloading Game.dll...
call :RequestGameDll reload
exit /b %ERRORLEVEL%

:RequestGameDll
del /q "%ACK%" 2>nul
del /q "%REQ%" 2>nul
>"%REQ%.tmp" echo %~1
move /y "%REQ%.tmp" "%REQ%" >nul
if errorlevel 1 (
    echo Could not write %REQ%.
    exit /b 1
)
set /a TRIES=0

:WaitAck
if exist "%ACK%" goto :ReadAck
set /a TRIES+=1
if %TRIES% GEQ 20 (
    echo FoxtrotEditor did not respond to %~1.
    echo Rebuild and restart the editor so it watches %%TEMP%%\FoxtrotEngine\game-dll.req.
    exit /b 1
)
ping -n 2 127.0.0.1 >nul
goto :WaitAck

:ReadAck
findstr /b /i /c:"ok" "%ACK%" >nul
if errorlevel 1 (
    echo FoxtrotEditor failed to %~1 Game.dll.
    exit /b 1
)
exit /b 0
