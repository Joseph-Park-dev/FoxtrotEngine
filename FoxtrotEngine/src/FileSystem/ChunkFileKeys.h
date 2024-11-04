#pragma once
#include <string>

namespace ChunkKeys {
// File formats
	constexpr const char* CHUNK_FILE_FORMAT = ".chunk";
	constexpr const char* EDITOR_FILE_FORMAT = ".json";

// Data Pack Names
	constexpr const char* CHUNK_DATA = "Chunk Data";
	constexpr const char* ACTOR_DATA = "Actor Data";
	constexpr const char* RESOURCE_DATA = "Resource Data";
	constexpr const char* CAMERA_DATA = "Camera Data";

	constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
	constexpr const char* TRANSFORM = "Transform";

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

// Tile Map Component
	constexpr const char* TILEMAP_KEY = "TileMapKey";

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
}