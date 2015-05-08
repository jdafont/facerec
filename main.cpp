#include <iostream>
#include <Eigen/Dense>
#include "image.h"
#include <stdio.h>
#include <string>

using namespace Eigen;
using namespace std;

const int SCREEN_WIDTH = 320*2;
const int SCREEN_HEIGHT = 243*1;


int main( int argc, char* args[] )
{
    SDL_Event event;
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
    Image* testFace = new Image;
    for(int i = 0; i < numFaces; i++) {
        if(!faces[i].loadImage(faceLocs[i])) {
            cout << "Image " << faceLocs[i] << " failed to load." << endl;
            return 1;
        }
    }
    if(!testFace->loadImage(args[1])) {
        cout << "Could not load or determine which image to identify." << endl;
        return 1;
    }
    printf("Pixel bytes: %d\n", faces[0].getFormat()->BitsPerPixel);

    int h = faces[0].getImage()->h;
    int w = faces[0].getImage()->w;
    MatrixXd m(w*h, numFaces);
    MatrixXd identify(w*h, 1);
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

    testFace->lock();
    for(int j = 0; j < h; j++) {
        for(int i = 0; i < w; i++) {
            //Concat to vector
            identify((j*w + i), 0) = testFace->getPixelIntensity(i, j); 
        }
    }
    testFace->unlock();
    
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

    MatrixXd idavg(w*h, 1);
    for(int j = 0; j < h; j++) {
        for(int i = 0; i < w; i++) {
            idavg(j*w + i, 0) = identify(j*w + i, 0) - psi(j*w + i, 0);
        }
    }

    MatrixXd S(numFaces, numFaces);
    S = A.transpose() * A;

    EigenSolver<MatrixXd> es(S);
    //cout << es.eigenvectors() << endl << es.eigenvalues() << endl;
    
    //Compute the first eigenface?
    MatrixXd eigenFaces(w*h, numFaces);
    for(int eig = 0; eig < numFaces; eig++) {
        MatrixXd EigFace1(w*h, 1);
        EigFace1 =  (A*(es.eigenvectors().col(eig).real())).cast<double>();
        eigenFaces.col(eig) = EigFace1.col(0);

        double max = 0;
        double min = 0;
        for(int i = 0; i < w*h; i++) {
            if(EigFace1(i, 0) < min) min = EigFace1(i,0);
            if(EigFace1(i, 0) > max) max = EigFace1(i,0);
        }
        MatrixXd normd(w*h, 1);
        for(int i = 0; i < w*h; i++) {
            normd(i, 0) = ((EigFace1(i,0)-min)*(255))/(max-min);
        }

        /*//Draw the first eigen face
        for(int x = 0; x < w; x++){
            for(int y = 0; y< h; y++) {
                int i = (y*w) + x;
                int c = normd(i, 0);
                SDL_SetRenderDrawColor(renderer, c, c, c, 255);
                SDL_RenderDrawPoint(renderer, x + ((eig%4)*320), y + (eig/4)*243);
            }
        }
        */
    }

    //TODO:  Project all original faces into face space
    // wk = UkTRANSPOSE(Gamma - Psi)
    //A has all mean reduced faces
    MatrixXd omega(numFaces, numFaces);

    for(int i = 0; i < numFaces; i++) {
        for(int k = 0; k < numFaces; k++) {
            omega(k,i) = eigenFaces.col(k).transpose() * A.col(i);
        }
    }
    MatrixXd projIdentify(numFaces, 1);
    for(int k = 0; k < numFaces; k++) {
        projIdentify(k,0) = eigenFaces.col(k).transpose() * idavg.col(0);
    }

    //cout << projIdentify << endl;

    int match = -1;
    float minDist = -1;
    for(int i = 0; i < numFaces; i++) {
        float dist = (omega.col(i) - projIdentify.col(0)).norm();
        if(minDist == -1){
            minDist = dist;
            match = i;
        }
        else if (dist < minDist) {
            match = i;
            minDist = dist;
        }
    }

    cout << "The closest match is " << faceLocs[match] << endl;
    for(int x = 0; x < w; x++){
        for(int y = 0; y< h; y++) {
            int i = (y*w) + x;
            int c = identify(i, 0);
            SDL_SetRenderDrawColor(renderer, c, c, c, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    for(int x = 0; x < w; x++){
        for(int y = 0; y< h; y++) {
            int i = (y*w) + x;
            int c = m(i, match);
            SDL_SetRenderDrawColor(renderer, c, c, c, 255);
            SDL_RenderDrawPoint(renderer, x + 320, y);
        }
    }

    SDL_RenderPresent(renderer);
    //SDL_Delay( 4000 );

    /*(SDL_Surface* screenshot = SDL_CreateRGBSurface(0, SCREEN_WIDTH, 
      SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if(screenshot) {
        SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(window),
          screenshot->pixels, screenshot->pitch);
        SDL_SaveBMP(screenshot, "EigenFaces.bmp");
        SDL_FreeSurface(screenshot);
    }*/

    bool quit = false;
    while(!quit)
    while(SDL_PollEvent( &event )) {
        if( event.type == SDL_QUIT )
            quit = true;
    }
    SDL_DestroyWindow( window );
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;

}
