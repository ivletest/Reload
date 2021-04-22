@echo off
pushd %~dp0\..\..\..\
call tools\premake5\premake5.exe --scripts=scripts/premake vs2019
popd
PAUSE
