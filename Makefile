INCDIR = C:\SFML\include
LIBDIR = C:\SFML\lib
simulate.exe : main.o simulate.o
	g++ "-L$(LIBDIR)" -o simulate.exe main.o simulate.o -lsfml-graphics-s-d -lfreetype -lsfml-window-s-d -lopengl32 -lgdi32 -lsfml-system-s-d -lwinmm
main.o : main.cpp simulate.h
	g++ "-I$(INCDIR)" -Wall -std=c++11 -DSFML_STATIC -fexceptions -g -c main.cpp -o main.o
simulate.o : simulate.cpp simulate.h
	g++ "-I$(INCDIR)" -Wall -std=c++11 -DSFML_STATIC -fexceptions -g -c simulate.cpp -o simulate.o
