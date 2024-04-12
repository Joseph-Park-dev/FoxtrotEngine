#pragma once
#include "FTResource.h"
#include <SDL2/SDL.h>

class Texture :
    public FTResource
{
public:
    SDL_Texture* GetEditorTexture() { return mEditorTexture; }
    SDL_Texture* GetGameviewTexture() { return mGameviewTexture; }

private:
    SDL_Texture* mEditorTexture;
    SDL_Texture* mGameviewTexture;

private:
    Texture();
    ~Texture() override;
    friend class ResourceManager;

private:
    SDL_Surface* LoadSurface();
    void CreateTextureFromSurface(SDL_Surface* surface);
};