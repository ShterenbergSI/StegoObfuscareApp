@echo off

set DDKPATH=D:\SHARE\WINDDK\7600.16385.1
set VIKAPATH=D:\SHARE\Work\Stego

set PATH=%DDKPATH%\bin\x86;%DDKPATH%\bin\x86\x86;%PATH%
set INCLUDE=%DDKPATH%\inc\api;%DDKPATH%\inc\crt;%VIKAPATH%\Include
set LIB=%DDKPATH%\lib\wnet\i386;%DDKPATH%\lib\crt\i386;%VIKAPATH%\Lib

cl /c /Zi /nologo /GS- test.c
link @win32.lnk

del *.obj
