@echo off
echo [*] Cloning dependencies...
call script\clone_deps.bat

echo [*] Patching vcxproj...
call script\patch_vcxproj-Core.bat

echo.
echo All steps complete.
pause