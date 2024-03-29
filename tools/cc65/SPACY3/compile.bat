@echo off

set name="SPACY3"

set path=%path%;..\bin\

set CC65_HOME=..\

cc65 -Oi %name%.c --add-source
ca65 reset.s
ca65 %name%.s
ca65 asm4c.s

ld65 -C nes.cfg -o %name%.nes reset.o %name%.o asm4c.o nes.lib -Ln labels.txt

del *.o

pause

%name%.nes
