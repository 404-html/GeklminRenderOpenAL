#/bin/bash
# this version doesn't compile an SO file
rm *.o
rm *.bin
rm *.so
g++ -O3 -ffast-math -msse4 -c *.c *.cpp  -lGL -lglfw -lopenal -m64 -Wno-undef
rm main2.o
rm main.o
g++ -O3 -ffast-math -msse4 -o prog2.bin *.o main2.cpp  -ldl -lGL -lglfw -lopenal -m64 -Wno-undef
g++ -O3 -ffast-math -msse4 -o prog.bin *.o main.cpp  -ldl -lGL -lglfw -lopenal -m64 -Wno-undef
