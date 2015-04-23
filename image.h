#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

using namespace std;

class Image {
    SDL_Surface *surf;
    SDL_PixelFormat *fmt;
    public:
    Image();
    ~Image();

    bool loadImage(string name);
    SDL_Surface* getImage();
    SDL_PixelFormat* getFormat();
    Uint8 getPixelIntensity(int x, int y);

    void lock();
    void unlock();
};
