@echo off

set DDKPATH=X:\WINDDK\7600.16385.1
set VIKAPATH=Z:\Work\Samples\PE-editors\PE

set PATH=%DDKPATH%\bin\x86;%DDKPATH%\bin\x86\x86;%PATH%
set INCLUDE=%DDKPATH%\inc\api;%DDKPATH%\inc\crt;%VIKAPATH%\Include
set LIB=%DDKPATH%\lib\wnet\i386;%DDKPATH%\lib\crt\i386;%VIKAPATH%\Lib


cl /c /Zi /nologo /GS- buffer_list.c
cl /c /Zi /nologo /GS- add.c
cl /c /Zi /nologo /GS- load.c
cl /c /Zi /nologo /GS- find.c
cl /c /Zi /nologo /GS- print.c
lib @win32.lib

del *.obj
