@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: Configuration
:: ============================================================
set BASE_DIR=%~dp0..\third_party
set GIT=git

:: ============================================================
:: Repositories to clone
:: ============================================================
set REPOS[0]=https://github.com/ocornut/imgui.git
set REPOS[1]=https://github.com/aiekick/ImGuiFileDialog.git
set REPOS[2]=https://github.com/nlohmann/json.git
set REPOS[3]=https://github.com/erincatto/box2d.git

:: ============================================================
:: Clone loop
:: ============================================================
if not exist "%BASE_DIR%" mkdir "%BASE_DIR%"

call :clone "!REPOS[0]!" "docking"
call :clone "!REPOS[1]!"
call :clone "!REPOS[2]!"
call :clone "!REPOS[3]!"

:: ============================================================
:: spine-cpp runtime only, at the latest default-branch tip
:: ============================================================
call :update_spine
if errorlevel 1 exit /b 1

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
:: :update_spine subroutine
:: Sparse-checkout the spine-cpp runtime and move it to latest.
:: Current runtimes keep sources in spine-cpp/src and headers
:: in spine-cpp/include.
:: ============================================================
:update_spine
set "SPINE_DIR=%BASE_DIR%\spine-runtimes"
set "SPINE_URL=https://github.com/esotericsoftware/spine-runtimes.git"
set "SPINE_PATH=spine-cpp"

if exist "%SPINE_DIR%" if not exist "%SPINE_DIR%\.git" (
    echo [ERROR] %SPINE_DIR% exists but is not a git checkout
    exit /b 1
)

if exist "%SPINE_DIR%\.git" goto spine_fetch

echo [*] Cloning spine-cpp runtime...
%GIT% clone --no-checkout --depth=1 --filter=blob:none --sparse "%SPINE_URL%" "%SPINE_DIR%"
if errorlevel 1 (
    echo [ERROR] Failed to clone spine-runtimes
    exit /b 1
)

:spine_fetch
echo [*] Updating spine-cpp runtime to latest...
%GIT% -C "%SPINE_DIR%" sparse-checkout set --cone "%SPINE_PATH%"
if errorlevel 1 (
    echo [ERROR] Failed to limit checkout to the spine-cpp runtime
    exit /b 1
)
%GIT% -C "%SPINE_DIR%" fetch --depth=1 origin HEAD
if errorlevel 1 (
    echo [ERROR] Failed to fetch the latest spine-runtimes tip
    exit /b 1
)
%GIT% -C "%SPINE_DIR%" reset --hard FETCH_HEAD
if errorlevel 1 (
    echo [ERROR] Failed to check out the latest spine-cpp runtime
    exit /b 1
)
%GIT% -C "%SPINE_DIR%" sparse-checkout reapply
if errorlevel 1 (
    echo [ERROR] Failed to materialize the spine-cpp runtime
    exit /b 1
)
if not exist "%SPINE_DIR%\spine-cpp\src\spine\" (
    echo [ERROR] spine-cpp runtime sources were not checked out
    exit /b 1
)
if not exist "%SPINE_DIR%\spine-cpp\include\spine\" (
    echo [ERROR] spine-cpp runtime headers were not checked out
    exit /b 1
)

for /f %%H in ('%GIT% -C "%SPINE_DIR%" rev-parse --short HEAD') do set SPINE_REV=%%H
echo [OK] spine-cpp runtime at !SPINE_REV!
exit /b 0