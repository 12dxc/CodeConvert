@echo off
echo 正在编译项目...

REM 编译 C++ 源文件
g++ -o output main.cpp utils.cpp

echo 编译完成!
pause