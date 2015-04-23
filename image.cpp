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
