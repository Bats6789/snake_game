all:
	gcc c_source/snakeGame.c -o executable/snakeGame -lmingw32 -mwindows -mconsole -I D:/C_library/SDL2-2.0.9/i686-w64-mingw32/include/ -L D:/C_library/SDL2-2.0.9/i686-w64-mingw32/lib/ -lSDL2main -lSDL2 -lSDL2_image
