@echo off

set DDKPATH=X:\WINDDK\7600.16385.1
set VIKAPATH=Z:\Work\Samples\PE-editors\PE

set PATH=%DDKPATH%\bin\x86;%DDKPATH%\bin\x86\x86;%PATH%
set INCLUDE=%DDKPATH%\inc\api;%DDKPATH%\inc\crt;%VIKAPATH%\Include
set LIB=%DDKPATH%\lib\wnet\i386;%DDKPATH%\lib\crt\i386;%VIKAPATH%\Lib

cl /c /Zi /nologo /GS- file.c
cl /c /Zi /nologo /GS- section.c
cl /c /Zi /nologo /GS- import.c
cl /c /Zi /nologo /GS- PE_header.c

lib @win32.lib
del *.obj
