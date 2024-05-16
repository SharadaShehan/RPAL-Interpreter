all: myrpal

myrpal: main.o
	g++ main.o -o myrpal

main.o: main.cpp
	g++ -c -Iheaders main.cpp -o main.o

clean:
	rm -f *.o myrpal
