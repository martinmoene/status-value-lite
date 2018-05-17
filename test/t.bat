@echo off & setlocal enableextensions enabledelayedexpansion
::
:: t.bat - compile & run tests (MSVC).
::

set basename=status_value

set arg1=%1
if  "%arg1%"=="c++98" ( 
   set basename=status_value_cpp98
) else (
   if not "%arg1%"=="" set std=-std:%std%
)

set CppCoreCheckInclude=%VCINSTALLDIR%\Auxiliary\VS\include

call :CompilerVersion version
echo VC%version%: %args%

set span_contract=^
    -Dnssv_CONFIG_NO_EXCEPTIONS=0
    
set span_config=^
    -Dnssv_CONFIG_MAX_ALIGN_HACK=0

set msvc_defines=^
    -DNOMINMAX ^
    -D_CRT_SECURE_NO_WARNINGS ^
    -Dlest_FEATURE_AUTO_REGISTER=1

cl -W3 -EHsc %std% %stdspn% %span_contract% %span_config% %msvc_defines% -I../include/nonstd %basename%.t.cpp && %basename%.t.exe
endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
@echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>                   >%tmpsource%
echo int main(){printf("%%d\n",_MSC_VER);} >>%tmpsource%

cl /nologo %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
set offset=0
if %version% LSS 1900 set /a offset=1
set /a version="version / 10 - 10 * ( 5 + offset )"
endlocal & set %1=%version%& goto :EOF
