#include <iostream>
#include <Eigen/Dense>
#include "image.h"
#include <stdio.h>
#include <string>

using namespace Eigen;
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main( int argc, char* args[] )
{
    SDL_Window* window = NULL;

    SDL_Surface* screenSurface = NULL;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ){
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }
    //Load image
    Image face;
    if(!face.loadImage("yalefaces/subject01.normal"))
        return 1;
    printf("Pixel bytes: %d\n", face.getFormat()->BitsPerPixel);

    /*SDL_LockSurface(face);
    Uint8 index;
    SDL_Color *color;
    index = *(Uint8 *)face->pixels;
    color = &fmt->palette->colors[index];
    SDL_UnlockSurface(face);
    printf("Pixel color-> Red: %d, Green: %d, Blue: %d. Index: %d\n",
      color->r, color->g, color->b, index);*/

    MatrixXd m(face.getImage()->w, face.getImage()->h);
    face.lock();
    int h = face.getImage()->h;
    int w = face.getImage()->w;
    cout << h << endl << w << endl;
    for(int j = 0; j < h; j++) {
        for(int i = 0; i < w; i++) {
            m(i, j) = face.getPixelIntensity(i, j);
        }
    }
    face.unlock();
    screenSurface = SDL_GetWindowSurface( window );
    SDL_BlitSurface(face.getImage(), NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface( window );
    SDL_Delay( 2000 );

    SDL_DestroyWindow( window );

    SDL_Quit();
    return 0;

}
