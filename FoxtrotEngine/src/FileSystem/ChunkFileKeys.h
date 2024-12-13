#pragma once
#include <string>

#define VALUE_NOT_ASSIGNED -1

namespace ChunkKeys {
// File formats
	constexpr const char* GAMEDATA_FILE_FORMAT = ".gdpack";
	constexpr const char* CHUNK_FILE_FORMAT = ".chunk";
	constexpr const char* EDITOR_FILE_FORMAT = ".json";
	constexpr const char* PREMADE_FILE_FORMAT = ".premade";

// Data Pack Names
	constexpr const char* CHUNK_DATA = "Chunk Data";
	constexpr const char* ACTOR_DATA = "Actor Data";
	constexpr const char* RESOURCE_DATA = "Resource Data";
	constexpr const char* CAMERA_DATA = "Camera Data";

	constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
	constexpr const char* TRANSFORM = "Transform";

	constexpr const char* GAME_DATA = "Game Data";
	constexpr const char* CHUNK_LIST = "Chunk List";
	constexpr const char* CHUNK_LIST_PATH = "Chunk List Path";

// Scene Related
	constexpr const char* ACTOR_COUNT = "Actor Count";

// Actor Related
	// Properties
	constexpr const char* ACTOR_GROUP = "Actor Group";

	// Transform
	constexpr const char* WORLD_POS = "World Position";
	constexpr const char* LOCAL_POS = "Local Position";
	constexpr const char* SCALE = "Scale";
	constexpr const char* ROTATION = "Rotation";

// Camera Related
	constexpr const char* TARGET_ACTOR = "Target Actor";
	constexpr const char* RENDER_RESOLUTION = "Render Resolution";
	constexpr const char* RENDER_SCREENCENTER = "Screen Center";

// Resource Related
	// Group Save Keys
	constexpr const char* FTTEXTURE_GROUP = "FTTexture Group";
	constexpr const char* FTTILEMAP_GROUP = "FTTileMap Group";
	constexpr const char* FTPREMADE_GROUP = "FTPremade Group";
	constexpr const char* FT_SPRITE_ANIMATION_GROUP = "FTSpriteAnimation Group";

	// FTTexture
	constexpr const char* FTTEXTURE = "FTTexture";
	constexpr const char* TEXTURE_WIDTH = "Texture Width";
	constexpr const char* TEXTURE_HEIGHT = "Texture Height";

	// FTTileMap
	constexpr const char* FTTILEMAP = "FTTileMap";
	constexpr const char* TILEMAP_SCREEN_WIDTH = "Tile Width On Screen";
	constexpr const char* TILEMAP_SCREEN_HEIGHT = "Tile Height On Screen";
	constexpr const char* TILEMAP_MAP_MAX_COUNT_X = "Max Count On Map X";
	constexpr const char* TILEMAP_MAP_MAX_COUNT_Y = "Max Count On Map Y";

	// Primitive Geometries
	constexpr const UINT PRIMITIVE_SQUARE = 0;

	// Premades
	constexpr const char* FTPREMADE = "FTPremade";

	// FT SPRITE ANIMATION
	constexpr const char* FT_SPRITE_ANIMATION = "FTSpriteAnimation";
	constexpr const char* ANIM_FPS			= "Animation FPS";
	constexpr const char* IS_REPEATED		= "Is Repeated";
	constexpr const char* MAX_FRAME_INDEX = "Max Frame Index";

// Component Related
	constexpr const char* COMPONENTS = "Components";
	constexpr const char* DRAW_ORDER = "DrawOrder";
	constexpr const char* UPDATE_ORDER = "UpdateOrder";

	// Mesh Renderer Component
	constexpr const char* MESH_KEY = "MeshKey";
	constexpr const char* TEXTURE_KEY = "TextureKey";

	// Sprite Renderer Component
	constexpr const char* CHANNEL = "Channel";
	constexpr const char* SPRITE_SCALE = "Scale";

	// Rigidbody2DComponent
	constexpr const char* ALLOW_FAST_ROTATION = "Allow Fast Rotation";
	constexpr const char* ANGULAR_DAMPING	  = "Angular Damping";
	constexpr const char* ANGULAR_VELOCITY	  = "Angular Velocity";
	constexpr const char* ENABLE_SLEEP		  = "Enable Sleep";
	constexpr const char* FIXED_ROTATION	  = "Fixed Rotation";
	constexpr const char* GRAVITY_SCALE		  = "Gravity Scale";
	constexpr const char* IS_AWAKE			  = "Is Awake";
	constexpr const char* IS_BULLET			  = "Is Bullet";
	constexpr const char* IS_ENABLED		  = "Is Enabled";
	constexpr const char* INITIAL_VELOCITY	  = "Initial Velocity";
	constexpr const char* SLEEP_THRESHOLD	  = "Sleep Threshold";

	constexpr const char* BODY_TYPE = "Body Type";

	constexpr const int BODY_TYPE_STATIC = 0;
	constexpr const int BODY_TYPE_KINEMATIC = 1;
	constexpr const int BODY_TYPE_DYNAMIC = 2;

	// Tile Map Component
	constexpr const char* TILEMAP_KEY = "TileMapKey";

	// Collider Component
	constexpr const char* OFFSET = "Offset";
	constexpr const char* SHOW_DEBUG_SHAPE = "Show Debug Shape";

	// Box Collider Component
	constexpr const char* COLLIDER_SCALE = "Scale";

	// Animator Component
	constexpr const char* LOADED_KEYS = "Loaded Keys";

// General Terms
	constexpr const char* NAME = "Name";
	constexpr const char* ID = "ID";
	constexpr const char* STATE = "State";
	constexpr const char* KEY = "Key";
	constexpr const char* FILE_NAME = "FileName";
	constexpr const char* PATH = "Path";
	constexpr const char* RELATIVE_PATH = "RelativePath";

	constexpr const char* PARENT = "Parent";
	constexpr const char* CHILD = "Child";

	constexpr const char* COUNT = "Count";
	constexpr const char* LIST = "List";

// Buffer Sizes
	constexpr size_t MAX_BUFFER_INPUT_TEXT = 50;
}