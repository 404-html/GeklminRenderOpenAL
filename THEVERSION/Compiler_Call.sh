#/bin/bash
g++ -O3 -ffast-math -msse4 -c *.c *.cpp  -lGL -lglfw -m64 -Wno-undef -fPIC
rm main.o
g++ -O3 -ffast-math -msse4 -shared -o GeklminRender.so *.o -m64 -Wno-undef -lGL -lglfw
echo "Compiled SO file!"
#In your root window:
cp GeklminRender.so /usr/lib/GeklminRender.so
echo "copied SO!"
g++ -O3 -ffast-math -msse4 -lGL -lglfw -m64 -Wno-undef -Wall -L. -l:GeklminRender.so -o prog2.bin main.cpp
#./prog2.bin