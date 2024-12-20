@echo off

set PN=vm
set PP=C:\Dev\cpp\%PN%
set E=%PP%\e

set ccflags=/Od /utf-8 /D_CRT_SECURE_NO_WARNINGS /DVM_DEBUG /std:c++latest -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4624 -wd4530 -wd4244 -wd4201 -wd4100 -wd4101 -wd4189 -wd4505 -wd4127 -wd4702 -wd4310 -FC -Z7 -I%PP%/q
set ldflags= -incremental:no -opt:ref

rmdir /Q /S %E%
mkdir %E%
pushd %E%

cls

rem cl %ccflags% %PP%\b\prog.cpp -Feprog.exe /link %ldflags%
rem cl %ccflags% %PP%\b\bank.cpp -Febank.exe /link %ldflags%
rem cl %ccflags% %PP%\b\konsole.cpp -Fekonsole.exe /link %ldflags%
cl %ccflags% /DASM_AST_AUSGEBEN %PP%\b\asm.cpp -Feasm.exe /link %ldflags%

popd

