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
call :clone "!REPOS[2]!"

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