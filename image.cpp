#include "image.h"

Image::Image() {
    surf = NULL;
    fmt = NULL;
}

Image::~Image() {
    SDL_FreeSurface(surf);
    delete fmt;
}

bool Image::loadImage(string name) {
    surf = IMG_Load(name.c_str());
    if(surf == NULL)
        return false;
    fmt = surf->format;
    return true;
}

SDL_Surface* Image::getImage() {
    return surf;
}

SDL_PixelFormat* Image::getFormat() {
    return fmt;
}

Uint8 Image::getPixelIntensity(int x, int y){
    //Assumes that the surface is locked
    //Assumes 8 bit surface

    Uint8* pixels = (Uint8*)surf->pixels;
    Uint8 target = pixels[(y*surf->w) + x];
    return target;
}

void Image::lock() {
    SDL_LockSurface(surf);
}
void Image::unlock() {
    SDL_UnlockSurface(surf);
}
