@echo off
setlocal enabledelayedexpansion

REM ===== HEADER LINES =====
set "h1=// kmMqtt (https://github.com/KMiseckas/kmMqtt)"
set "h2=// Copyright (c) 2026 Klaudijus Miseckas"
set "h3=// Licensed under the Apache License, Version 2.0"
set "h4=// See LICENSE file in the project root for full license information."

REM ===== PROCESS FILES =====
for /r %%F in (*.cpp) do call :add_header "%%F"
for /r %%F in (*.h) do call :add_header "%%F"
for /r %%F in (*.hpp) do call :add_header "%%F"

echo Done.
endlocal
goto :eof

:add_header
set "file=%~1"

REM Skip if already contains copyright line
findstr /m /c:"Copyright (c^) 2026" "!file!" >nul
if errorlevel 1 (
    (
        echo !h1!
        echo !h2!
        echo !h3!
        echo !h4!
        echo.
        type "!file!"
    ) > "!file!.tmp"

    move /y "!file!.tmp" "!file!" >nul
    echo Added header: !file!
) else (
    echo Skipped: !file!
)
goto :eof
