@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%
echo Starting ClubConnect Server on http://localhost:8080...
start http://localhost:8080
clubconnect.exe
pause
