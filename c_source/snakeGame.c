/*
 * Name: snakeGame.c
 * Auth: Blake Wingard.
 * Desc: A replica of the snake game using SDL2.
 * Vers 1.0.0 06/25/2019 CBW - Original code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH   	640
#define SCREEN_HEIGHT  	480
#define SNAKE_DIAMETER  25
#define TILE  		25
#define TOP_WALL  	29
#define BOTTOM_WALL    	404
#define LEFT_WALL  	20
#define RIGHT_WALL  	595
#define MAX_SCORE	381
#define CENTER_X	295
#define CENTER_Y	229
#define NUMBER_X	107
#define NUMBER_Y	440
#define NUMBER_WIDTH 	15
#define NUMBER_HEIGHT	27
#define TILES_WIDE	24
#define TILES_TALL	16

typedef enum {
	UP, RIGHT, DOWN, LEFT
} snakeDirection;

void logSDLError( const char *msg );
void renderScore( SDL_Renderer *renderer, SDL_Texture *numbers, SDL_Rect *number, SDL_Rect numberLoc, int score );
void spawnFood( SDL_Rect snakeLoc, SDL_Rect *foodLoc, SDL_Rect *bodyLoc, int bodyCount );
int eat( SDL_Rect snake, SDL_Rect food );

int main( int argc, char **argv ){
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Surface *tmpSurface;
	SDL_Texture *background;
	SDL_Texture *snake;
	SDL_Rect snakeLoc;
	SDL_Rect snakePart[ 4 ];
	SDL_Rect body;
	SDL_Rect bodyLoc[ MAX_SCORE + 2 ];
	SDL_Texture *food;
	SDL_Rect foodLoc;
	SDL_Texture *numbers;
	SDL_Rect number[ 10 ];
	SDL_Rect numberLoc;
	SDL_Event event;
	int quit;

	quit = 0;
	srand( time( NULL ) );

	if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ){
		logSDLError( "SDL_Init" );
		return( 1 );
	}
	if(( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) != IMG_INIT_PNG ){
		logSDLError( "IMG_Init" );
		SDL_Quit();
		return( 1 );
	}
	window = SDL_CreateWindow( "Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( window == NULL ){
		logSDLError( "SDL_CreateWindow" );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	renderer = SDL_CreateRenderer( window, -1, 0 );
	if( renderer == NULL ){
		logSDLError( "SDL_CreateRenderer" );
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	tmpSurface = IMG_Load( "sprites/background.png" );
	if( tmpSurface == NULL ){
		logSDLError( "IMG_Load \"Background\"");
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	background = SDL_CreateTextureFromSurface( renderer, tmpSurface );
	if( background == NULL ){
		logSDLError( "CreateTexture \"Background\"" );
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	SDL_FreeSurface( tmpSurface );
	tmpSurface = IMG_Load( "sprites/number.png" );
	if( tmpSurface == NULL ){
		logSDLError( "IMG_Load \"Number\"");
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	numbers = SDL_CreateTextureFromSurface( renderer, tmpSurface );
	if( numbers == NULL ){
		logSDLError( "CreateTexture \"Number\"" );
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	SDL_FreeSurface( tmpSurface );
	tmpSurface = IMG_Load( "sprites/food.png" );
	if( tmpSurface == NULL ){
		logSDLError( "IMG_Load \"Food\"");
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	food = SDL_CreateTextureFromSurface( renderer, tmpSurface );
	if( food == NULL ){
		logSDLError( "CreateTexture \"Food\"" );
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	SDL_FreeSurface( tmpSurface );
	tmpSurface = IMG_Load( "sprites/snake.png" );
	if( tmpSurface == NULL ){
		logSDLError( "IMG_Load \"Snake\"");
		SDL_DestroyRenderer( renderer );
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	snake = SDL_CreateTextureFromSurface( renderer, tmpSurface );
	if( snake == NULL ){
		logSDLError( "CreateTexture \"Snake\"" );
		SDL_DestroyRenderer( renderer );
		SDL_DestroyWindow( window );
		IMG_Quit();
		SDL_Quit();
		return( 1 );
	}
	SDL_FreeSurface( tmpSurface );
	for( int index = 0; index < 10; ++index ){
		number[ index ].x = index % 3 * 15;
		number[ index ].y = index / 3 * 27;
		number[ index ].w = NUMBER_WIDTH;
		number[ index ].h = NUMBER_HEIGHT;
	}
	numberLoc.x = NUMBER_X;
	numberLoc.y = NUMBER_Y;
	numberLoc.w = NUMBER_WIDTH;
	numberLoc.h = NUMBER_HEIGHT;
	int bodyCount;
	snakeLoc.x = CENTER_X;
	snakeLoc.y = CENTER_Y;
	snakeLoc.w = SNAKE_DIAMETER;
	snakeLoc.h = SNAKE_DIAMETER;
	for( int index = 0; index < 4; ++index ){
		snakePart[ index ].x = index % 2 * SNAKE_DIAMETER;
		snakePart[ index ].y = index / 2 * SNAKE_DIAMETER;
		snakePart[ index ].w = SNAKE_DIAMETER;
		snakePart[ index ].h = SNAKE_DIAMETER;
	}
	body.x = 0;
	body.y = 2 * SNAKE_DIAMETER;
	body.w = SNAKE_DIAMETER;
	body.h = SNAKE_DIAMETER;
	for( int index = 0; index < 2; ++index ){
		bodyLoc[ index ].x = CENTER_X - SNAKE_DIAMETER * ( index + 1 );
		bodyLoc[ index ].y = CENTER_Y;
		bodyLoc[ index ].w = SNAKE_DIAMETER;
		bodyLoc[ index ].h = SNAKE_DIAMETER;
	}
	SDL_QueryTexture( food, NULL, NULL, &foodLoc.w, &foodLoc.h );
	bodyCount = 2;
	spawnFood( snakeLoc, &foodLoc, bodyLoc, bodyCount );
	snakeDirection direction = RIGHT;
	int score;
	score = 0;
	while( !quit ){
		while( SDL_PollEvent( &event ) != 0 ){
			if( event.type == SDL_QUIT ){
				quit = 1;
			} else if( event.type == SDL_KEYDOWN ){
				switch( event.key.keysym.sym ){
					case SDLK_UP:
					case SDLK_w:
						direction = UP;
					break;
					case SDLK_DOWN:
					case SDLK_s:
						direction = DOWN;
					break;
					case SDLK_RIGHT:
					case SDLK_d:
						direction = RIGHT;
					break;
					case SDLK_LEFT:
					case SDLK_a:
						direction = LEFT;
					break;
					default:
					break;
				}
			}
		}
		SDL_RenderClear( renderer );
		SDL_RenderCopy( renderer, background, NULL, NULL );
		for( int index = bodyCount; index > 0; --index ){
			bodyLoc[ index ].x = bodyLoc[ index - 1 ].x;
			bodyLoc[ index ].y = bodyLoc[ index - 1 ].y;
		}
		bodyLoc[ 0 ].x = snakeLoc.x;
		bodyLoc[ 0 ].y = snakeLoc.y;
		if(( direction == UP ) && ( snakeLoc.y > TOP_WALL )){
			snakeLoc.y -= SNAKE_DIAMETER;
		} else if(( direction == DOWN ) && ( snakeLoc.y < BOTTOM_WALL )){
			snakeLoc.y += SNAKE_DIAMETER;
		} else if(( direction == RIGHT ) && ( snakeLoc.x < RIGHT_WALL )){
			snakeLoc.x += SNAKE_DIAMETER;
		} else if(( direction == LEFT ) && ( snakeLoc.x > LEFT_WALL )){
			snakeLoc.x -= SNAKE_DIAMETER;
		} else {
			quit = 1;
		}
		for( int index = 0; index < bodyCount; ++index ){
			if( eat( snakeLoc, bodyLoc[ index ] ) == 1 ){
				quit = 1;
			}
		}
		if( eat( snakeLoc, foodLoc ) == 1 ){
			bodyLoc[ bodyCount ].w = SNAKE_DIAMETER;
			bodyLoc[ bodyCount ].h = SNAKE_DIAMETER;
			++bodyCount;
			++score;
			spawnFood( snakeLoc, &foodLoc, bodyLoc, bodyCount );
		}
		SDL_RenderCopy( renderer, snake, &snakePart[ direction ], &snakeLoc );
		for( int index = 0; index < bodyCount; ++index ){
			SDL_RenderCopy( renderer, snake, &body, &bodyLoc[ index ] );
		}
		renderScore( renderer, numbers, number, numberLoc, score );
		SDL_RenderCopy( renderer, food, NULL, &foodLoc );
		SDL_RenderPresent( renderer );
		SDL_Delay( 250 );
	}
	SDL_DestroyTexture( background );
	SDL_DestroyTexture( numbers );
	SDL_DestroyTexture( snake );
	SDL_DestroyTexture( food );
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	IMG_Quit();
	SDL_Quit();
	return( 0 );
	// start the main menu
	// if exit is selected close window.
	// if options is selected, go to options
	// if high score is selected, go to high scores
	// if start is selected, start the game

	// option
	// Difficulty:
	// 	Easy
	// 	Medium
	// 	Hard
	// Color: 
	// 	Snake:
	// 		Red
	// 		Yellow
	// 		Green
	// 		Blue
	// 		(maybe add dynamic color)
	// 	BackGround:
	// 		Black
	// 		White
	// 		Green
	// 		Red
	// 		Blue
	// 		Yellow
	// 		(maybe add dynamic color)
	// Sound:
	// 	On
	// 	Off

	// high score
	// List of top 5 scores
	// delete button for each
	// clear button to clear all with warning

	// end game with death screen
	// 	if score is higher then one of the top 5,
	// 		congratulate the player and ask for name.
	// 		save score.
	// 	return to main menu
	return( EXIT_SUCCESS );
}

/*
 * Name: logSDLError
 * Desc: Generic error message.
 * Args:
 * 	const char *msg - the type of error
 */
void logSDLError( const char *msg ){
	printf("%s Error: %s\n", msg, SDL_GetError());
}

/*
 * Name: eat
 * Desc: checks if the snake has eaten
 * Args:
 * 	SDL_Rect snake - the stucture of the snake.
 * 	SDL_Rect food  - the structure of the food.
 */
int eat( SDL_Rect snake, SDL_Rect food ){
	int results;
	results = 0;
	if(( snake.x == food.x ) && ( snake.y == food.y )){
		results = 1;
	}
	return( results );
}

/*
 * Name: renderScore
 * Desc: Renders the score.
 * Args:
 * 	SDL_Renderer *renderer - the renderer.
 * 	SDL_Texture *numbers   - the numbers texture.
 * 	SDL_Rect *number       - the number clips.
 * 	SDL_Rect numberLoc     - the numbers location.
 * 	int score   	       - the score.
 */
void renderScore( SDL_Renderer *renderer, SDL_Texture *numbers, SDL_Rect *number, SDL_Rect numberLoc, int score ){
	if( score / 100 > 0 ){
		SDL_RenderCopy( renderer, numbers, &number[ score / 100 ], &numberLoc );
		numberLoc.x += ( NUMBER_WIDTH + 3 );
	}
	if( score / 10 > 0 ){
		SDL_RenderCopy( renderer, numbers, &number[ ( score / 10 ) % 10 ], &numberLoc );
		numberLoc.x += ( NUMBER_WIDTH + 3 );
	}
	SDL_RenderCopy( renderer, numbers, &number[ score % 10 ], &numberLoc );
}

/*
 * Name: spawnFood
 * Desc: Spawns food where the snake isn't located
 * Args:
 * 	SDL_Rect snakeLoc - the snake's head location.
 * 	SDL_Rect *foodLoc - the food's location.
 *	SDL_Rect *bodyLoc - the snake's body locations.
 *	int bodyCount     - The number of body segments.
 */
void spawnFood( SDL_Rect snakeLoc, SDL_Rect *foodLoc, SDL_Rect *bodyLoc, int bodyCount ){
	int check;
	check = 0;
	check = 1;
	while( check == 1 ){
		check = 0;
		foodLoc->x = (( rand() % TILES_WIDE ) * TILE ) + 20;
		foodLoc->y = (( rand() % TILES_TALL ) * TILE ) + 29;
		for( int index = 0; index < bodyCount; ++index ){
			if( eat( bodyLoc[ index ], *foodLoc ) == 1 ){
				check = 1;
			}
		}
		if( eat( snakeLoc, *foodLoc ) == 1 ){
			check = 1;
		}
	}

}
