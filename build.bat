@echo off
rem =====================================================
rem PROYECTO TLI - Compilacion rapida en Windows
rem Requiere MinGW-w64 (w64devkit) en el PATH.
rem =====================================================
setlocal

where gcc >nul 2>nul
if errorlevel 1 (
    echo [ERROR] gcc no esta en el PATH.
    echo Abre una terminal nueva o ejecuta C:\tools\w64devkit\w64devkit.exe
    exit /b 1
)

gcc -std=c11 -Wall -Wextra -o tli.exe main.c punto2.c punto4.c trabajo.c arbol.c validaciones.c tli_calculos.c tli_explicaciones.c
if errorlevel 1 (
    echo.
    echo [ERROR] Fallo la compilacion.
    exit /b 1
)

echo.
echo Compilacion OK: tli.exe
endlocal
