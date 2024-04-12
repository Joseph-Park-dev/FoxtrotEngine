#include <string>
#include <stdlib.h>
#include <SDL2/SDL_image.h>

#include "Texture.h"
#include "CCore.h"

SDL_Surface* Texture::LoadSurface()
{
	size_t size;
	int bufferSize = wcslen(mRelativePath.c_str()) * sizeof(wchar_t);
	char* path = new char[bufferSize];
	wcstombs_s(
		&size, 
		path,
		(size_t)bufferSize,
		mRelativePath.c_str(),
		(size_t)bufferSize -1
		);
	SDL_Surface* surf = IMG_Load(path);
	if (!surf)
	{
		SDL_Log("Failed to load texture file %s", path);
		return nullptr;
	}
	if (path)
	{
		delete[] path;
	}
	return surf;
}

void Texture::CreateTextureFromSurface(SDL_Surface* surface)
{
#ifdef _DEBUG
	SDL_Texture* editorTex = nullptr;
	editorTex = SDL_CreateTextureFromSurface(
		CCore::GetInstance()->GetEditorRenderer(), surface
	);
	if (!editorTex)
	{
		SDL_Log("Failed to create surface from texture for %ls", mRelativePath.c_str());
		return;
	}
	else
		mEditorTexture = editorTex;
#endif
	SDL_Texture* gameviewTex = nullptr;
	gameviewTex = SDL_CreateTextureFromSurface(
		CCore::GetInstance()->GetGameRenderer(), surface
	);

	SDL_FreeSurface(surface);
	if (!gameviewTex)
	{
		SDL_Log("Failed to create surface from texture for %ls", mRelativePath.c_str());
		return;
	}
	else
		mGameviewTexture = gameviewTex;
}

Texture::Texture()
	: mEditorTexture(nullptr)
	, mGameviewTexture(nullptr)
{

}

Texture::~Texture()
{
#ifdef _DEBUG
	if (CCore::GetInstance()->GetEditorRenderer())
	{
		SDL_DestroyTexture(mEditorTexture);
		mEditorTexture = nullptr;
	}
#endif // _DEBUG
	if (CCore::GetInstance()->GetGameRenderer())
	{
		SDL_DestroyTexture(mGameviewTexture);
		mGameviewTexture = nullptr;
	}
}