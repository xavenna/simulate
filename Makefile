simulate.exe : main.o simulate.o
	g++ -L"C:\Users\Adam Evanson\SFML-251\lib" -o simulate.exe main.o simulate.o -lsfml-graphics-d -lfreetype -lsfml-window-d -lopengl32 -lgdi32 -lsfml-system-d -lwinmm
main.o : main.cpp simulate.h
	g++ "-IC:\Users\Adam Evanson\SFML-251" -Wall -std=c++11 -fexceptions -g -c main.cpp -o main.o
simulate.o : simulate.cpp simulate.h
	g++ "-IC:\Users\Adam Evanson\SFML-251" -Wall -std=c++11 -fexceptions -g -c simulate.cpp -o simulate.o
