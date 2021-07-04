@setlocal
@set std=%1
@if not "%std%"=="" set std=-std:%std%
clang-cl -m32 -W3 -EHsc %std% -I../include -I. -Dlest_FEATURE_AUTO_REGISTER=1 status-value.t.cpp && status-value.t.exe
@endlocal

