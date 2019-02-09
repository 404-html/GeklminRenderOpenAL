#/bin/bash
g++ -O3 -ffast-math -msse4 -c *.c *.cpp  -lGL -lglfw -lopenal -m64 -Wno-undef -fPIC
rm main.o
rm main2.o
g++ -O3 -ffast-math -msse4 -shared -o GeklminRender.so *.o -m64 -Wno-undef -lGL -lglfw
echo "Compiled SO file!"
#In your root window:
#cp GeklminRender.so /usr/lib/GeklminRender.so
#echo "copied SO!"
#Use -Wl,-rpath=$(pwd) to have GeklminRender.so be in this same directory 
g++ -O3 -ffast-math -msse4 -lGL -lglfw -lopenal -m64 -Wno-undef -L. -l:GeklminRender.so -Wl,-rpath="$(pwd)" -o prog.bin main.cpp
g++ -O3 -ffast-math -msse4 -lGL -lglfw -lopenal -m64 -Wno-undef -L. -l:GeklminRender.so -Wl,-rpath="$(pwd)" -o prog2.bin main2.cpp
#./prog2.bin
