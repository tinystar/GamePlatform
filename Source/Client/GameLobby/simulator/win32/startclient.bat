:: convenient shortcut for start client with parameters and copy needed resources
:: Author: xiaoxin
:: Date: 2017/7/19
@echo off

:: change to the dir where the bat file is in
cd %~dp0

:: copy needed resources
echo copy resources...
xcopy "..\..\res" ".\res" /D /E /I /F /Y
xcopy "..\..\src" ".\src" /D /E /I /F /Y

:: start client
echo
echo start the client...
start GameLobby.exe

::pause