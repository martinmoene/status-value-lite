@echo off & setlocal enableextensions enabledelayedexpansion
::
:: tg.bat - compile & run tests (GNUC).
::

set basename=status_value

:: if no std is given, use c++11

set std=%1
shift
set args=%*
if "%std%" == "" set std=c++11

call :CompilerVersion version
echo g++ %version%: %std% %args%

set span_contract=^
    -Dnssv_CONFIG_NO_EXCEPTIONS=0

set span_config=

set gcc_defines=^
    -Dlest_FEATURE_AUTO_REGISTER=1

set flags=-Wpedantic -Wconversion -Wsign-conversion -Wno-padded -Wno-missing-noreturn
set   gpp=g++

%gpp% -std=%std% -O2 -Wall -Wextra %flags% %stdspn% %span_contract% %span_config% %gcc_defines% -o %basename%.t.exe -I../include/nonstd %basename%.t.cpp && %basename%.t.exe

endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>     > %tmpsource%
echo int main(){printf("%%d.%%d.%%d\n",__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__);} >> %tmpsource%

g++ -o %tmpprogram% %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
endlocal & set %1=%version%& goto :EOF
