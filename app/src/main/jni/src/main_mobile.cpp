#include <SDL.h>
#include <string>
#include <iostream>
#include "android_support.h"

//Screen dimensions
SDL_Rect gScreenRect = { 0, 0, 320, 240 };

bool init()
{
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		SDL_Log( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
	{
		SDL_Log( "Warning: Linear texture filtering not enabled!" );
	}

	//Get device display mode
	SDL_DisplayMode displayMode;
	if( SDL_GetCurrentDisplayMode( 0, &displayMode ) == 0 )
	{
		gScreenRect.w = displayMode.w;
		gScreenRect.h = displayMode.h;
	}

	return true;
}

void close()
{
	//Quit SDL subsystems
	SDL_Quit();
}

int pushover_main(int argc, char * argv[]);


int main( int argc, char* args[] )
{
    redirect_cout_to_log();

    std::cout << "Is it me3?";

	//Start up SDL and create window
	if( !init() )
	{
		SDL_Log( "Failed to initialize!\n" );
		return -1;
	}

    int result = pushover_main(argc, args);
	close();

	return result;
}
