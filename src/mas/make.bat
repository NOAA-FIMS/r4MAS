g++ -std=c++17 -O3 -D_USE_MATH_DEFINES -DMAS_COMMAND_TOOL -Ithird_party/Quadra/external/eigen main.cpp -o mas.exe
if errorlevel 1 exit /b 1
if not exist bin mkdir bin
move mas.exe bin\mas.exe
