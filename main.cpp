#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main( int argc, char* args[] )
{
    SDL_Window* window = NULL;

    SDL_Surface* screenSurface = NULL;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else 
        {
            SDL_Surface* face = IMG_Load("yalefaces/subject01.normal");
            screenSurface = SDL_GetWindowSurface( window );
            SDL_BlitSurface(face, NULL, screenSurface, NULL);
            SDL_UpdateWindowSurface( window );
            SDL_Delay( 2000 );
        }
    }

    SDL_DestroyWindow( window );

    SDL_Quit();
    return 0;

}
