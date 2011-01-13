@echo off

set VARIANT=%1
if "%VARIANT%"=="" set VARIANT=debug
if "%VARIANT%"=="Debug" set VARIANT=debug
if "%VARIANT%"=="DEBUG" set VARIANT=debug
if "%VARIANT%"=="Release" set VARIANT=release
if "%VARIANT%"=="RELEASE" set VARIANT=release

set LINK=shared
if "%VARIANT%"=="release" set LINK=static

echo Building boost...
pushd boost

.\bootstrap && .\bjam toolset=msvc-9.0 link=static runtime-link=%LINK% variant=%VARIANT% && popd
