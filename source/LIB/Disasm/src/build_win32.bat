@echo off

set DDKPATH=X:\WINDDK\7600.16385.1
set Z0PATH=X:\Z0\FirstLook
set VIKAPATH=Z:\Work

set PATH=%DDKPATH%\bin\x86;%DDKPATH%\bin\x86\x86;%PATH%
set INCLUDE=%DDKPATH%\inc\api;%DDKPATH%\inc\crt;%Z0PATH%\Include;%VIKAPATH%\Include
set LIB=%DDKPATH%\lib\wnet\i386;%DDKPATH%\lib\crt\i386;%Z0PATH%\lib\win32;%VIKAPATH%\Lib


cl /c /Zi /nologo /Gs- decoder.c
cl /c /Zi /nologo /Gs- distorm.c
cl /c /Zi /nologo /Gs- instructions.c
cl /c /Zi /nologo /Gs- insts.c
cl /c /Zi /nologo /Gs- mnemonics.c
cl /c /Zi /nologo /Gs- operands.c
cl /c /Zi /nologo /Gs- prefix.c
cl /c /Zi /nologo /Gs- textdefs.c
cl /c /Zi /nologo /Gs- wstring.c
lib @win32.lib

del *.obj
