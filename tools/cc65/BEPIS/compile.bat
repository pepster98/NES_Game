@echo off

set name="main"

set path=%path%;..\bin\

set CA65_HOME=..\

ca65 %name%.a
ld65 -t nes -o %name%.nes main.o

del *.o

pause

%name%.nes