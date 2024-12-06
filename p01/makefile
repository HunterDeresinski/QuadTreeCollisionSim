exe: main.cpp
	g++ *.cpp src/*.cpp src/*.c `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -Iincludes 

run:
	./a.out

r: run

clean:
	rm -f a.out

c: clean
#-g -fsanitize=address