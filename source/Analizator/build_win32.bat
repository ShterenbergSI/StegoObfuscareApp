@echo off

set DDKPATH=Z:\WINDDK\7600.16385.1
set Z0PATH=X:\Z0\FirstLook
set VIKAPATH=Z:\Work\Commonlib

set PATH=%DDKPATH%\bin\x86;%DDKPATH%\bin\x86\x86;%PATH%
set INCLUDE=%DDKPATH%\inc\api;%DDKPATH%\inc\crt;%Z0PATH%\Include;Z:\Work\CommonLib;%VIKAPATH%
set LIB=%DDKPATH%\lib\wnet\i386;%DDKPATH%\lib\crt\i386;%Z0PATH%\lib\win32;%VIKAPATH%

cl /c /Zi /nologo /GS- main.c
link @win32.lnk

del *.obj

