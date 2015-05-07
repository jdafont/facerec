#include <iostream>
#include <Eigen/Dense>
#include "image.h"
#include <stdio.h>
#include <string>

using namespace Eigen;
using namespace std;

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 243;


int main( int argc, char* args[] )
{
    string faceLocs[] = {
        "yalefaces/subject01.normal",
        "yalefaces/subject02.normal",
        "yalefaces/subject03.normal",
        "yalefaces/subject04.normal",
        "yalefaces/subject05.normal",
        "yalefaces/subject06.normal",
        "yalefaces/subject07.normal",
        "yalefaces/subject08.normal",
        "yalefaces/subject09.normal",
        "yalefaces/subject10.normal",
        "yalefaces/subject11.normal",
        "yalefaces/subject12.normal",
        "yalefaces/subject13.normal",
        "yalefaces/subject14.normal",
        "yalefaces/subject15.normal"
    };
    int numFaces = sizeof(faceLocs)/sizeof(string);
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

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

    renderer = SDL_CreateRenderer(window, -1, 0);
    if( renderer == NULL ){
        printf( "Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Load image
    Image* faces = new Image[numFaces];
    for(int i = 0; i < numFaces; i++) {
        if(!faces[i].loadImage(faceLocs[i])) {
            cout << "Image " << faceLocs[i] << " failed to load." << endl;
            return 1;
        }
    }
    printf("Pixel bytes: %d\n", faces[0].getFormat()->BitsPerPixel);

    int h = faces[0].getImage()->h;
    int w = faces[0].getImage()->w;
    MatrixXd m(w*h, numFaces);
    cout << h << endl << w << endl;

    //Copy pixel intensity data to matrix
    for(int k = 0; k < numFaces; k++) {
        faces[k].lock();
        for(int j = 0; j < h; j++) {
            for(int i = 0; i < w; i++) {
                //Concat to vector
                m((j*w + i), k) = faces[k].getPixelIntensity(i, j); 
            }
        }
        faces[k].unlock();
    }

    //Calculate average face from matrix data
    MatrixXd psi(w*h, 1);
    for(int i = 0; i < w*h; i++) {
        int sum = 0;
        for(int k = 0; k < numFaces; k++) { 
           sum += m(i, k); 
        }
        float average = sum/numFaces;
        psi(i, 0) = average;
    }

    //Mean subtract all of the faces
    MatrixXd A(w*h, numFaces);
    for(int k = 0; k < numFaces; k++) {
        for(int j = 0; j < h; j++) {
            for(int i = 0; i < w; i++) {
                A(j*w + i, k) = m(j*w + i, k) - psi(j*w + i, 0);
            }
        }
    }

    MatrixXd S(numFaces, numFaces);
    S = A.transpose() * A;

    EigenSolver<MatrixXd> es(S);
    //cout << es.eigenvectors() << endl << es.eigenvalues() << endl;
    
    int eig = atoi(args[1]);
    //Compute the first eigenface?
    MatrixXd EigFace1(w*h, 1);
    EigFace1 =  (A*(es.eigenvectors().col(eig).real())).cast<double>();
    //Draw the first eigen face
    for(int x = 0; x < w; x++){
        for(int y = 0; y< h; y++) {
            int i = (y*w) + x;
            int c = EigFace1(i, 0);
            SDL_SetRenderDrawColor(renderer, c, c, c, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay( 4000 );

    /*SDL_Surface* screenshot = SDL_CreateRGBSurface(0, SCREEN_WIDTH, 
      SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if(screenshot) {
        SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(window),
          screenshot->pixels, screenshot->pitch);
        SDL_SaveBMP(screenshot, "Screenshot.bmp");
        SDL_FreeSurface(screenshot);
    }*/


    SDL_DestroyWindow( window );
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;

}
