all:
	gcc c_source/snakeGame.c -o executable/snakeGame -lmingw32 -mwindows -mconsole -I ./libraries/ -L ./libraries/lib/ -lSDL2main -lSDL2 -lSDL2_image
