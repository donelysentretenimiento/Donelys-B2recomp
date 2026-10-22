@echo off
rem Builds and runs tests\hdr_color_test.c with the bundled llvm-mingw. Exit code 0 = all checks passed.
setlocal
cd /d "%~dp0.."
if defined LLVM_MINGW (set "CC=%LLVM_MINGW%\bin\x86_64-w64-mingw32-clang.exe") else (set "CC=deps\llvm-mingw\bin\x86_64-w64-mingw32-clang.exe")
if not exist build\hdr mkdir build\hdr
"%CC%" -O1 -Wall -Wextra -Werror -o build\hdr\hdr_color_test.exe tests\hdr_color_test.c -lm || exit /b 1
build\hdr\hdr_color_test.exe || exit /b 1
"%CC%" -O1 -Wall -Wextra -Werror -o build\hdr\hdr_win_change_test.exe tests\hdr_win_change_test.c || exit /b 1
build\hdr\hdr_win_change_test.exe
