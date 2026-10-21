@echo off
rem Builds and runs tests\hdr_color_test.c with the bundled llvm-mingw. Exit code 0 = all checks passed.
setlocal
cd /d "%~dp0.."
set "CC=deps\llvm-mingw\bin\x86_64-w64-mingw32-clang.exe"
if not exist build\hdr mkdir build\hdr
"%CC%" -O1 -Wall -o build\hdr\hdr_color_test.exe tests\hdr_color_test.c -lm || exit /b 1
build\hdr\hdr_color_test.exe
