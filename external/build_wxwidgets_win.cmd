@echo off

set VARIANT=%1
if "%VARIANT%"=="" set VARIANT=debug
if "%VARIANT%"=="Debug" set VARIANT=debug
if "%VARIANT%"=="DEBUG" set VARIANT=debug
if "%VARIANT%"=="Release" set VARIANT=release
if "%VARIANT%"=="RELEASE" set VARIANT=release

set LINK=dynamic
if "%VARIANT%"=="release" set LINK=static

echo Building wxwidgets...
pushd wxwidgets\build\msw

REM Using a .sln might be faster, but don't want to keep all the .vcprojs up-to-date
nmake -f makefile.vc BUILD=%VARIANT% RUNTIME_LIBS=%LINK% UNICODE=1 > wxwidgets_build.log

REM Make sure this isn't hanging around causing problems...
taskkill.exe /f /t /im mspdbsrv.exe > nul 2> nul

popd
