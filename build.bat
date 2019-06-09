tools\cc65\bin\ca65 src\main.asm -g -I src -o main.o
tools\cc65\bin\ld65 -C ldscripts\nes.ld -o main.nes -m main.map -Ln main.ln -vm main.o
pause
