@echo off
pushd %~dp0\..\..\
call rmdir c:\test /s /q
popd
PAUSE