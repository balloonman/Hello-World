/* walk.cpp - SDL walking demo
 * Paul Kennedy, CSE 20212
 * March 27, 2013
 */

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;

// screen atributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

// fps
const int FRAMES_PER_SECOND = 10;

// sprite dimensions
const int CHARACTER_WIDTH = 75;
const int CHARACTER_HEIGHT = 100;

// surfaces
SDL_Surface * sprite_sheet = NULL;
SDL_Surface * screen = NULL;

// event structure
SDL_Event event;

// sprite sheet divisions
SDL_Rect clips[10];

/*** prototypes ***/
// character class
class Character
{
private:
	int offSet;
	int velocity;
	int frame;
public:
	Character();
	void move();
	void show();
};

// timer class
class Timer
{
private:
	int startTicks;
	int pausedTicks;

	bool paused;
	bool started;
public:
	Timer();

	void start();
	void stop();
	void pause();
	void unpause();

	int getTicks();

	bool isStarted();
	bool isPaused();
};

// load an image from file then optimize it for the screen
SDL_Surface * load_image( std::string filename );

// apply the given source to the given destination with the given offsets
void apply_surface( int x, int y, SDL_Surface * source, SDL_Surface * destination, SDL_Rect * clip = NULL );

// clip the sprite sheet
void set_clips()
{
	int x_offset = 0; // updates for each clip
	const int y_offset = CHARACTER_HEIGHT * 2; // remains the same

	for (int i = 0; i < 10; ++i) {
		clips[i].x = x_offset;
		clips[i].y = y_offset;
		clips[i].w = CHARACTER_WIDTH;
		clips[i].h = CHARACTER_HEIGHT;

		x_offset += CHARACTER_WIDTH;
	}
}

/*** main function ***/

int main( int argc, char ** args )
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) // start up SDL
		return 1;

	// set up the screen surface
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE ); 

	if( screen == NULL ) // make sure the screen loaded successfully
		return 1;

	SDL_WM_SetCaption( "SDL Walking Demo", NULL ); // set a caption

	sprite_sheet = load_image( "walk_iso.jpg" ); // load the image

	set_clips(); // clip the sprite sheet

	// fill the screen white
	SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF));

	Timer fps;
	Character enemy;

	bool quit = false;

	while (quit == false) { // while the user hasn't quit
		fps.start(); // start the timer

		while (SDL_PollEvent(&event)) { // check for quit
			if (event.type == SDL_QUIT)
				quit = true;
		}

		// move and show the character
		enemy.move();
		SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF)); // clear the screen
		enemy.show();

		if (SDL_Flip(screen) == -1) return 1; // update the screen
		if (fps.getTicks() < 1000 / FRAMES_PER_SECOND)
			SDL_Delay( (1000/FRAMES_PER_SECOND) - fps.getTicks() );
	}
	
SDL_FreeSurface( sprite_sheet );

	SDL_Quit();

	return 0;
}

/*** function implementations ***/

SDL_Surface * load_image( std::string filename )
{
	SDL_Surface * loadedImage = NULL;
	SDL_Surface * optimizedImage = NULL;

	loadedImage = IMG_Load( filename.c_str() );

	if( loadedImage != NULL ) {
		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );

		if (optimizedImage != NULL) {
			SDL_SetColorKey (optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB (optimizedImage->format, 0xFF, 0xFF, 0xFF));
		}
	}
	return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface * source, SDL_Surface * destination, SDL_Rect * clip  )
{
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	SDL_BlitSurface( source, clip, destination, &offset );
}

Character::Character()
{
	offSet = 0;
	velocity = 8;
	frame = 0;
}

void Character::move()
{
	offSet += velocity;

	// keep the character in bounds
	if (offSet + CHARACTER_WIDTH > SCREEN_WIDTH)
		offSet -= velocity;
}

void Character::show()
{
	// loop animation
	if (frame >= 9)
		frame = 0;
	else frame++;

	// apply surface
	apply_surface( offSet, SCREEN_HEIGHT - CHARACTER_HEIGHT, sprite_sheet, screen, &clips[frame] );
}

Timer::Timer()
{
	startTicks = 0;
	pausedTicks = 0;
	paused = false;
	started = false;
}

void Timer::start()
{
	started = true;
	paused = false;
	startTicks = SDL_GetTicks();
}

void Timer::stop()
{
	started = false;
	paused = false;
}

void Timer::pause()
{
	if ((started == true) && (paused == false)) {
		paused = true;
		pausedTicks = SDL_GetTicks() - startTicks;
	}
}

void Timer::unpause()
{
	if (paused == true) {
		paused = false;
		startTicks = SDL_GetTicks() - pausedTicks;
		pausedTicks = 0;
	}
}

int Timer::getTicks()
{
	if (started == true) {
		if (paused == true)
			return pausedTicks;
		else
			return SDL_GetTicks() - startTicks;
	}

	return 0; // timer isn't running
}

bool Timer::isStarted()
{
	return started;
}

bool Timer::isPaused()
{
	return paused;
}
