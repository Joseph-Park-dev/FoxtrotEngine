#pragma once
#include <string>

namespace ChunkKeys {
// File formats
	constexpr const char* CHUNK_FILE_FORMAT = ".chunk";
	constexpr const char* EDITOR_FILE_FORMAT = ".json";

// Data Pack Names
	constexpr const char* CHUNK_DATA = "ChunkData";
	constexpr const char* ACTOR_DATA = "ActorData";
	constexpr const char* RESOURCE_DATA = "ResourceData";
	constexpr const char* CAMERA_DATA = "CameraData";

	constexpr const char* ACTOR_PROPERTIES = "ActorProperties";
	constexpr const char* TRANSFORM = "Transform";

// Scene Related
	constexpr const char* ACTOR_COUNT = "ActorCount";

// Actor Related
	// Properties
	constexpr const char* ACTOR_GROUP = "ActorGroup";

	// Transform
	constexpr const char* WORLD_POS = "WorldPosition";
	constexpr const char* LOCAL_POS = "LocalPosition";
	constexpr const char* SCALE = "Scale";
	constexpr const char* ROTATION = "Rotation";

// Camera Related
	constexpr const char* TARGET_ACTOR = "TargetActor";
	constexpr const char* RENDER_RESOLUTION = "RenderResolution";
	constexpr const char* RENDER_SCREENCENTER = "ScreenCenter";

// Component Related
	constexpr const char* COMPONENTS = "Components";
	constexpr const char* DRAW_ORDER = "DrawOrder";
	constexpr const char* UPDATE_ORDER = "DrawOrder";

// Mesh Renderer Component
	constexpr const char* MESH_KEY = "MeshKey";
	constexpr const char* TEXTURE_KEY = "TextureKey";

// Sprite Renderer Component
	constexpr const char* TEXTURE_WIDTH = "TextureWidth";
	constexpr const char* TEXTURE_HEIGHT = "TextureHeight";
	constexpr const char* CHANNEL = "Channel";
	constexpr const char* SPRITE_SCALE = "Scale";

// General Terms
	constexpr const char* NAME = "Name";
	constexpr const char* ID = "ID";
	constexpr const char* STATE = "State";

	constexpr const char* PARENT = "Parent";
	constexpr const char* CHILD = "Child";

	constexpr const char* COUNT = "Count";
	constexpr const char* LIST = "List";
}