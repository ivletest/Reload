@echo off
pushd %~dp0\..\..\..\
call tools\premake5\premake5.exe vs2019
popd
PAUSE
