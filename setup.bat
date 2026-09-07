@echo off
echo [*] Cloning dependencies...
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0script\setup-dependencies.ps1"
if errorlevel 1 exit /b 1

rem Project source lists are checked in; do not patch Core with renderer dependencies.

echo.
echo All steps complete.
pause
