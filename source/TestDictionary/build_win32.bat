@echo off

set DDKPATH=X:\WINDDK\7600.16385.1
set VIKAPATH=Z:\Work\Samples\PE-editors\PE

set PATH=%DDKPATH%\bin\x86;%DDKPATH%\bin\x86\x86;%PATH%
set INCLUDE=%DDKPATH%\inc\api;%DDKPATH%\inc\crt;%VIKAPATH%\Include
set LIB=%DDKPATH%\lib\wnet\i386;%DDKPATH%\lib\crt\i386;%VIKAPATH%\Lib

cl /c /Zi /nologo /GS- add_synonym.c
link @win32.lnk
@echo ADD_SYNONYM completed!

cl /c /Zi /nologo /GS- print_synonym.c
link @print_synonym32.lnk
@echo PRINT_SYNONYM completed!
del *.obj
