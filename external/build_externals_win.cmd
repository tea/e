@echo off
REM If we're not running under a VS prompt, try to get one.
if "%VCInstallDir%"=="" call "%VS90COMNTOOLS%\vsvars32.bat"

set VARIANT=%1
if "%VARIANT%"=="" set VARIANT=Debug
set CONFIG="%VARIANT%|Win32"

if not EXIST build_logs mkdir build_logs

call :BUILD libtommath\libtommath.sln %CONFIG%
call :BUILD libtomcrypt\libtomcrypt.sln %CONFIG%
call :BUILD curl\lib\curllib.sln %CONFIG%
call :BUILD metakit\win\msvc90\mksrc.sln %CONFIG%
call :BUILD pcre\pcre.sln %CONFIG%
call :BUILD tinyxml\tinyxml.sln %CONFIG%

call build_wxwidgets_win.cmd %VARIANT%
call build_boost_win.cmd %VARIANT%

echo Builds complete, check build_logs for possible issues.

goto :EOF


REM **** Subroutines start here ****

REM  Usage: call :BUILD path_to_sln [, config_to_build="DEBUG|Win32"]
:BUILD
setlocal
set CONFIG=%2
if {%CONFIG%}=={} set CONFIG="DEBUG|Win32"

echo Building %1...
vcbuild %1 %CONFIG% > build_logs\%~n1.log
set RET=%ERRORLEVEL%
REM "mspdbsrv.exe" can hang, which prevents new .pdb's from
REM being created and causes subsequent command-line builds to fail.
REM Explicitly try to kill it after a command-line build.
taskkill.exe /f /t /im mspdbsrv.exe > nul 2> nul
endlocal & set RET=%RET%
goto :EOF
