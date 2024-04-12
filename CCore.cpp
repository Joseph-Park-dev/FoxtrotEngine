#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <nlohmann/json.hpp>
#include <algorithm>

#include "CCore.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "KeyInputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "CollisionManager.h"
#include "Camera2D.h"
#include "UIManager.h"
#include "Physics2D.h"
#include "ParticleSystem.h"
#include "ChunkLoader.h"
#include "EventManager.h"

#ifdef _DEBUG
#include "EditorLayer.h"
#include "EditorCamera2D.h"
#endif // _DEBUG

CCore* CCore::gPInst = nullptr;

CCore::CCore()
	: mGameTitle("Cat Blasters! ver.0.01")
	, mGameviewWindow(nullptr)
	, mGameRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mResolution(Vector2(1920.f, 1080.f))
#ifdef _DEBUG
	, mEditorWindow(nullptr)
	, mEditorRenderer(nullptr)
	, mIsUpdatingGame(false)
	, mEditorResolution()
	, mEditorDataFileName("EditorData.json")
	, mEditorResolutionKey("EditorResolution")
#endif
{
#ifdef _DEBUG
	std::ifstream inStream(mEditorDataFileName);
	if (!inStream.fail())
	{
		nlohmann::ordered_json editorData = nlohmann::ordered_json::parse(inStream);
		if (!editorData.is_null())
		{
			int editorW = editorData[mEditorResolutionKey]["x"];
			int editorH = editorData[mEditorResolutionKey]["y"];
			mEditorResolution = Vector2(editorW, editorH);
			return;
		}
	}
	mEditorResolution = mResolution;
#endif // _DEBUG
}

CCore::~CCore()
{}

CCore* CCore::GetInstance()
{
	if (gPInst == nullptr)
	{
		gPInst = new CCore;
	}
	return gPInst;
}

void CCore::Release()
{
	if (gPInst != nullptr)
	{
		delete gPInst;
		gPInst = nullptr;
	}
}

bool CCore::Initialize()
{
	if (!InitSDL())
		return false;
	if (!InitMainWindow())
		return false;
	if (!InitSDLRenderer())
		return false;
	InitSingletonManagers();
	InitTicks();
	return true;
}

bool CCore::InitSDL()
{
	int sdlInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	if (sdlInitResult != 0)
	{
		SDL_Log("ERROR: SDL init failed : %s", SDL_GetError());
		return false;
	}
	return true;
}

bool CCore::InitMainWindow()
{
#ifdef _DEBUG
	mEditorWindow = SDL_CreateWindow(
		"Foxtrot Engine Editor"
		, 0
		, 0
		, (int)mEditorResolution.x
		, (int)mEditorResolution.y
		, 0
	);
	if (!mEditorWindow)
	{
		SDL_Log("ERROR: SDL create Editor Window failed : %s", SDL_GetError());
		return false;
	}
	SDL_SetWindowResizable(mEditorWindow, SDL_TRUE);
#endif

	mGameviewWindow = SDL_CreateWindow(
		mGameTitle.c_str()
		, SDL_WINDOWPOS_CENTERED_DISPLAY(1)
		, SDL_WINDOWPOS_CENTERED_DISPLAY(1)
		, (int)mResolution.x
		, (int)mResolution.y
		, 0
	);
	if (!mGameviewWindow)
	{
		SDL_Log("ERROR: SDL create Game Window failed : %s", SDL_GetError());
		return false;
	}
	SDL_SetWindowResizable(mGameviewWindow, SDL_FALSE);
	return true;
}

bool CCore::InitSDLRenderer()
{
	// Initialize renderer for editor window
	mEditorRenderer = SDL_CreateRenderer(
		mEditorWindow, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mEditorRenderer)
	{
		SDL_Log("ERROR: SDL create Editor Renderer failed : %s", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawBlendMode(mEditorRenderer, SDL_BLENDMODE_BLEND);

	// Initialize renderer for game window
	mGameRenderer = SDL_CreateRenderer(
		mGameviewWindow, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mGameRenderer)
	{
		SDL_Log("ERROR: SDL create Game Renderer failed : %s", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawBlendMode(mGameRenderer, SDL_BLENDMODE_BLEND);

	// Initialize SDL IMG
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_Image: %s", SDL_GetError());
		return false;
	}
	return true;
}

bool CCore::InitDirect3DRenderer()
{
	return false;
}

void CCore::InitSingletonManagers()
{
	Physics2D::GetInstance()->Init();
	Camera2D::GetInstance()->Init();
	SceneManager::GetInstance()->Init();
	ResourceManager::GetInstance();
	UIManager::GetInstance();
	EventManager::GetInstance();
	KeyInputManager::GetInstance();

#ifdef _DEBUG
	EditorCamera2D::GetInstance();
	EditorLayer::GetInstance()->Init(mEditorWindow, mEditorRenderer);
#endif // _DEBUG
}

void CCore::InitTicks()
{
	mTicksCount = SDL_GetTicks();
}

void CCore::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();

		ProcessEvent();
	}
}

void CCore::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_WINDOWEVENT
			&& event.window.event == SDL_WINDOWEVENT_CLOSE)
		{
			mIsRunning = false;
			break;
		}
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
			case SDL_CONTROLLERDEVICEADDED:
				if (!KeyInputManager::GetInstance()->GetGamepad())
					KeyInputManager::GetInstance()->SetGamepad(SDL_GameControllerOpen(event.cdevice.which));
				SDL_Log("Gamepad added!");
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				SDL_GameController* controller = KeyInputManager::GetInstance()->GetGamepad();
				if (controller &&
					event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)))
				{
					SDL_GameControllerClose(controller);
					KeyInputManager::GetInstance()->
						SetGamepad(KeyInputManager::GetInstance()->FindGamepad());
					SDL_Log("Gamepad removed!");
				}
				break;
		}
#ifdef _DEBUG
		EditorLayer::GetInstance()->ProcessInput(&event);
		EditorCamera2D::GetInstance()->ProcessInput(&event);
#endif // _DEBUG
	}
	KeyInputManager::GetInstance()->DetectKeyInput();
	KeyInputManager::GetInstance()->DetectMouseInput();
	KeyInputManager::GetInstance()->DetectGamepadInput();
	SceneManager::GetInstance()->ProcessInput(KeyInputManager::GetInstance());
}

void CCore::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
#ifdef _DEBUG
	if (mIsUpdatingGame)
	{
		Physics2D::GetInstance()->Update();
		SceneManager::GetInstance()->Update(deltaTime);
		SceneManager::GetInstance()->Lateupdate(deltaTime);
		CollisionManager::GetInstance()->Update();
		ParticleSystem::GetInstance()->Update(deltaTime);
		UIManager::GetInstance()->Update(deltaTime);
	}
	else
	{
		SceneManager::GetInstance()->EditorUpdate(deltaTime);
		SceneManager::GetInstance()->EditorLateUpdate(deltaTime);
		UIManager::GetInstance()->EditorUpdate(deltaTime);
	}
	Camera2D::GetInstance()->Update(deltaTime);
	EditorCamera2D::GetInstance()->Update(deltaTime);
	EditorLayer::GetInstance()->Update(deltaTime);
#else
	SceneManager::GetInstance()->Update(deltaTime);
	SceneManager::GetInstance()->Lateupdate(deltaTime);
	CollisionManager::GetInstance()->Update();
	ParticleSystem::GetInstance()->Update(deltaTime);
	UIManager::GetInstance()->Update(deltaTime);
	Camera2D::GetInstance()->Update(deltaTime);
#endif
	mTicksCount = SDL_GetTicks();
}

void CCore::GenerateOutput()
{
#ifdef _DEBUG
	SDL_SetRenderDrawColor(
		mEditorRenderer,
		92, 92, 92, 255
	);
	SDL_RenderClear(mEditorRenderer);

	SDL_SetRenderDrawColor(
		mGameRenderer,
		92, 92, 92, 255
	);
	SDL_RenderClear(mGameRenderer);

	if (mIsUpdatingGame)
	{
		SceneManager::GetInstance()->Render(mEditorRenderer);
		Camera2D::GetInstance()->Render(mEditorRenderer);
		ParticleSystem::GetInstance()->Render(mEditorRenderer);
	}
	else
	{
		EditorLayer::GetInstance()->DisplayEditorElements(mEditorRenderer);
	}
	EditorCamera2D::GetInstance()->EditorRender(mEditorRenderer);
	EditorLayer::GetInstance()->Render(mEditorRenderer);
	CollisionManager::GetInstance()->RenderRay(mEditorRenderer);
	SDL_RenderPresent(mEditorRenderer);
	SDL_RenderPresent(mGameRenderer);

#else
	SDL_SetRenderDrawColor(
		mGameRenderer,
		0, 0, 255, 255
	);
	SDL_RenderClear(mGameRenderer);
	SceneManager::GetInstance()->Render(mGameRenderer);
	//Camera2D::GetInstance()->Render(mGameRenderer);
	CollisionManager::GetInstance()->RenderRay(mGameRenderer);
	ParticleSystem::GetInstance()->Render(mGameRenderer);
	SDL_RenderPresent(mGameRenderer);
#endif
}

void CCore::ProcessEvent()
{
	SceneManager::GetInstance()->ProcessEvent();
}

void CCore::ShutDown()
{
#ifdef _DEBUG
	//SaveEditorData();
	EditorLayer::GetInstance()->ShutDown();
	SDL_DestroyRenderer(mEditorRenderer);
	SDL_DestroyWindow(mEditorWindow);
#endif // _DEBUG
	IMG_Quit();
	SDL_DestroyRenderer(mGameRenderer);
	SDL_DestroyWindow(mGameviewWindow);
	SDL_Quit();
}

Vector2 CCore::GetEditorWindowSize() const
{
	int editorW = 0; int editorH = 0;
	SDL_GetWindowSize(mEditorWindow, &editorW, &editorH);
	return Vector2(editorW, editorH);
}

//#ifdef _DEBUG
//void CCore::SaveEditorData(std::ofstream& ofs)
//{
//	int editorW = 0; int editorH = 0;
//	SDL_GetWindowSize(mEditorWindow, &editorW, &editorH);
//	FileIOHelper::AddBasicString(ofs, mEditorResolutionKey);
//	FileIOHelper::
//		AddVector2(ofs, Vector2(editorW, editorH));
//}
//#endif // _DEBUG