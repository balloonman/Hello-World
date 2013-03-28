walk: walk.o
	g++ walk.o -o walk -lSDL -lSDL_image

walk.o: walk.cpp
	g++ -c walk.cpp

clean:
	rm -rf *.o walk
