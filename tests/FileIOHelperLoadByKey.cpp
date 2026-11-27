#include "FileSystem/FileIOHelper.h"

#include <Windows.h>
#include <crtdbg.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

namespace
{
	int gFailures = 0;

	void Check(bool condition, const char* message)
	{
		if (condition)
			return;
		std::fprintf(stderr, "FAIL %s\n", message);
		++gFailures;
	}

	void WriteChunk(const std::string& path, const char* text)
	{
		std::ofstream out(path.c_str(), std::ios::binary);
		out << text;
	}

	std::string TempPath(const char* name)
	{
		char directory[MAX_PATH] = {};
		GetTempPathA(MAX_PATH, directory);
		return std::string(directory) + name;
	}

	void Open(const std::string& path, std::ifstream& stream)
	{
		stream.open(path.c_str(), std::ios::binary);
		Check(stream.is_open(), "chunk file did not open");
	}
}

static void ExpectAssert(const char* exe, const char* mode)
{
	std::string command = std::string("\"") + exe + "\" " + mode;
	const int code = std::system(command.c_str());
	Check(code != 0, mode);
}

static void TestReorderAndStreamPosition()
{
	const std::string path = TempPath("foxtrot-keyed-reorder.chunk");
	WriteChunk(
		path,
		"Camera Data<4>\n"
		"\tOffset[Vector3]\n"
		"\t(0.000000,1.000000,2.000000)\n"
		"\tPosition[Vector3]\n"
		"\t(3.000000,4.000000,5.000000)\n"
		"\tTarget Actor[string]\n"
		"\tNULL OBJECT\n"
		"\tZoom[float]\n"
		"\t2.500000\n"
		"Next<0>\n");

	std::ifstream stream;
	Open(path, stream);
	Common::FTDS::String packName("Camera Data");
	std::pair<size_t, Common::FTDS::String> opened =
		Common::FileIOHelper::BeginDataPackLoad(stream, packName);
	Check(opened.first == 4, "camera entry count");
	Check(opened.second == "Camera Data", "camera pack name");

	Common::FTDS::String nextHeader;
	Check(Common::FileIOHelper::GetLine(stream, nextHeader), "next header was consumed with the pack");
	Check(nextHeader == "Next<0>", "buffered pack leaves the stream on the following header");

	float zoom = 0.0f;
	Math::FTVector3 position;
	Common::FTDS::String target;
	Math::FTVector3 offset;
	Common::FTDS::String zoomKey("Zoom");
	Common::FTDS::String positionKey("Position");
	Common::FTDS::String targetKey("Target Actor");
	Common::FTDS::String offsetKey("Offset");
	Common::FileIOHelper::LoadFloat(stream, zoomKey, zoom);
	Common::FileIOHelper::LoadVector3(stream, positionKey, position);
	Common::FileIOHelper::LoadBasicString(stream, targetKey, target);
	Common::FileIOHelper::LoadVector3(stream, offsetKey, offset);

	Check(zoom == 2.5f, "zoom follows its key");
	Check(position.x == 3.0f && position.y == 4.0f && position.z == 5.0f, "position follows its key");
	Check(target == "NULL OBJECT", "target follows its key");
	Check(offset.x == 0.0f && offset.y == 1.0f && offset.z == 2.0f, "offset follows its key");
	Check(!Common::FileIOHelper::GetLine(stream, nextHeader), "camera pack popped after its fields were consumed");
}

static void TestDuplicateFields()
{
	const std::string path = TempPath("foxtrot-keyed-dll.chunk");
	WriteChunk(
		path,
		"DLL List<5>\n"
		"\tDLL[string]\n"
		"\tFoxtrotD3D11\n"
		"\tDLL[string]\n"
		"\tUI\n"
		"\tDLL[string]\n"
		"\tPhysics2D\n"
		"\tDLL[string]\n"
		"\tSound\n"
		"\tDLL[string]\n"
		"\tGame\n");

	std::ifstream stream;
	Open(path, stream);
	Common::FTDS::String packName("DLL List");
	Common::FileIOHelper::BeginDataPackLoad(stream, packName);
	const char* expected[] = { "FoxtrotD3D11", "UI", "Physics2D", "Sound", "Game" };
	Common::FTDS::String key("DLL");
	for (const char* name : expected)
	{
		Common::FTDS::String value;
		Common::FileIOHelper::LoadBasicString(stream, key, value);
		Check(value == name, "duplicate DLL keys stay in file order");
	}
}

static void TestDuplicatePacksAndChildNames()
{
	const std::string path = TempPath("foxtrot-keyed-packs.chunk");
	WriteChunk(
		path,
		"Components<3>\n"
		"\tBoxCollider2D<1>\n"
		"\t\tX[int]\n"
		"\t\t1\n"
		"\tSprite<0>\n"
		"\tBoxCollider2D<1>\n"
		"\t\tX[int]\n"
		"\t\t2\n");

	std::ifstream stream;
	Open(path, stream);
	Common::FTDS::String packName("Components");
	Common::FileIOHelper::BeginDataPackLoad(stream, packName);

	Common::FTDS::DynamicArray<Common::FTDS::String> names;
	Common::FileIOHelper::CopyChildDataPackNames(stream, names);
	Check(names.GetSize() == 3, "child pack name count");
	Check(names.At(0) == "BoxCollider2D" && names.At(1) == "Sprite" && names.At(2) == "BoxCollider2D",
		"child pack names keep file order and duplicates");

	Common::FTDS::String box("BoxCollider2D");
	Common::FTDS::String field("X");
	int first = 0;
	int second = 0;
	Common::FileIOHelper::BeginDataPackLoad(stream, box);
	Common::FileIOHelper::LoadInt(stream, field, first);
	Common::FileIOHelper::BeginDataPackLoad(stream, box);
	Common::FileIOHelper::LoadInt(stream, field, second);
	Check(first == 1 && second == 2, "duplicate pack names are consumed in file order");

	Common::FileIOHelper::CopyChildDataPackNames(stream, names);
	Check(names.GetSize() == 3, "copying child names does not consume packs");

	Common::FTDS::String sprite("Sprite");
	Common::FileIOHelper::BeginDataPackLoad(stream, sprite);
}

static void TestNestedBeforeSibling()
{
	const std::string path = TempPath("foxtrot-keyed-nested.chunk");
	WriteChunk(
		path,
		"Actor Properties<3>\n"
		"\tName[string]\n"
		"\tHero\n"
		"\tTransform<2>\n"
		"\t\tPosition[Vector3]\n"
		"\t\t(9.000000,8.000000,7.000000)\n"
		"\t\tRotation[float]\n"
		"\t\t1.500000\n"
		"\tActive[bool]\n"
		"\ttrue\n"
		"After<1>\n"
		"\tMarker[int]\n"
		"\t4\n");

	std::ifstream stream;
	Open(path, stream);
	Common::FTDS::String actor("Actor Properties");
	Common::FTDS::String transform("Transform");
	Common::FileIOHelper::BeginDataPackLoad(stream, actor);
	Common::FileIOHelper::BeginDataPackLoad(stream, transform);

	float rotation = 0.0f;
	Math::FTVector3 position;
	Common::FTDS::String rotationKey("Rotation");
	Common::FTDS::String positionKey("Position");
	Common::FileIOHelper::LoadFloat(stream, rotationKey, rotation);
	Common::FileIOHelper::LoadVector3(stream, positionKey, position);
	Check(rotation == 1.5f, "nested rotation");
	Check(position.x == 9.0f && position.y == 8.0f && position.z == 7.0f, "nested position");

	bool active = false;
	Common::FTDS::String activeKey("Active");
	Common::FTDS::String nameKey("Name");
	Common::FTDS::String name;
	Common::FileIOHelper::LoadBool(stream, activeKey, active);
	Common::FileIOHelper::LoadBasicString(stream, nameKey, name);
	Check(active, "sibling bool is read after the nested pack");
	Check(name == "Hero", "sibling string is read after the nested pack");

	Common::FTDS::String after("After");
	Common::FTDS::String markerKey("Marker");
	int marker = 0;
	Common::FileIOHelper::BeginDataPackLoad(stream, after);
	Common::FileIOHelper::LoadInt(stream, markerKey, marker);
	Check(marker == 4, "parent pops after its nested pack and sibling fields are consumed");
}

static void TestSeparateStreams()
{
	const std::string chunkPath = TempPath("foxtrot-keyed-chunk.chunk");
	const std::string listPath = TempPath("foxtrot-keyed-list.ftconfig");
	WriteChunk(chunkPath, "Components<1>\n\tCount[int]\n\t3\n");
	WriteChunk(listPath, "DLL List<1>\n\tDLL[string]\n\tSound\n");

	std::ifstream chunk;
	std::ifstream list;
	Open(chunkPath, chunk);
	Open(listPath, list);
	Common::FTDS::String components("Components");
	Common::FTDS::String dllList("DLL List");
	Common::FileIOHelper::BeginDataPackLoad(chunk, components);
	Common::FileIOHelper::BeginDataPackLoad(list, dllList);

	Common::FTDS::String dllKey("DLL");
	Common::FTDS::String dll;
	Common::FileIOHelper::LoadBasicString(list, dllKey, dll);
	int count = 0;
	Common::FTDS::String countKey("Count");
	Common::FileIOHelper::LoadInt(chunk, countKey, count);
	Check(dll == "Sound" && count == 3, "two streams keep separate packs");
}

static void TestLegacyStreamLoads()
{
	const std::string path = TempPath("foxtrot-keyed-legacy.chunk");
	WriteChunk(path, "Legacy<1>\n\tValue[int]\n\t7\nAfter<0>\n");

	std::ifstream byPointer;
	Open(path, byPointer);
	Common::FileIOHelper::BeginDataPackLoad(byPointer, "Legacy");
	int value = 0;
	Common::FileIOHelper::LoadInt(byPointer, value);
	Check(value == 7, "const char* BeginDataPackLoad still reads the next field from the stream");
	Common::FTDS::String header;
	Check(Common::FileIOHelper::GetLine(byPointer, header), "legacy load leaves the following header");
	Check(header == "After<0>", "const char* BeginDataPackLoad does not buffer the pack");

	std::ifstream byTemporary;
	Open(path, byTemporary);
	Common::FileIOHelper::BeginDataPackLoad(byTemporary, Common::FTDS::String("Legacy"));
	value = 0;
	Common::FileIOHelper::LoadInt(byTemporary, value);
	Check(value == 7, "String temporaries still read the next field from the stream");
}

static void TestEmptyPackPops()
{
	const std::string path = TempPath("foxtrot-keyed-empty.chunk");
	WriteChunk(path, "CSV<0>\nJSON<1>\n\tN[int]\n\t4\n");
	std::ifstream stream;
	Open(path, stream);
	Common::FTDS::String csv("CSV");
	Common::FTDS::String json("JSON");
	std::pair<size_t, Common::FTDS::String> opened = Common::FileIOHelper::BeginDataPackLoad(stream, csv);
	Check(opened.first == 0, "empty pack count");
	opened = Common::FileIOHelper::BeginDataPackLoad(stream, json);
	int number = 0;
	Common::FTDS::String key("N");
	Common::FileIOHelper::LoadInt(stream, key, number);
	Check(opened.second == "JSON" && number == 4, "empty pack pops before the next root pack");
}

static void TestRemainingScalars()
{
	const std::string path = TempPath("foxtrot-keyed-scalars.chunk");
	WriteChunk(
		path,
		"Values<5>\n"
		"\tU[unsigned int]\n"
		"\t12\n"
		"\tS[size]\n"
		"\t30\n"
		"\tB[bool]\n"
		"\tfalse\n"
		"\tV2[Vector2]\n"
		"\t(1.000000,2.000000)\n"
		"\tV4[Vector4]\n"
		"\t(1.000000,2.000000,3.000000,4.000000)\n");
	std::ifstream stream;
	Open(path, stream);
	Common::FTDS::String pack("Values");
	Common::FileIOHelper::BeginDataPackLoad(stream, pack);

	Math::FTVector4 vector4;
	Common::FTDS::String vector4Key("V4");
	Common::FileIOHelper::LoadVector4(stream, vector4Key, vector4);
	bool flag = true;
	Common::FTDS::String boolKey("B");
	Common::FileIOHelper::LoadBool(stream, boolKey, flag);
	size_t sizeValue = 0;
	Common::FTDS::String sizeKey("S");
	Common::FileIOHelper::LoadSize(stream, sizeKey, sizeValue);
	unsigned int unsignedValue = 0;
	Common::FTDS::String unsignedKey("U");
	Common::FileIOHelper::LoadUnsignedInt(stream, unsignedKey, unsignedValue);
	Math::FTVector2 vector2;
	Common::FTDS::String vector2Key("V2");
	Common::FileIOHelper::LoadVector2(stream, vector2Key, vector2);

	Check(unsignedValue == 12u, "unsigned int");
	Check(sizeValue == 30u, "size");
	Check(flag == false, "bool");
	Check(vector2.x == 1.0f && vector2.y == 2.0f, "vector2");
	Check(vector4.x == 1.0f && vector4.y == 2.0f, "vector4");
}

static void AssertMissingKey()
{
	const std::string path = TempPath("foxtrot-keyed-missing.chunk");
	WriteChunk(path, "Pack<1>\n\tA[int]\n\t1\n");
	std::ifstream stream;
	stream.open(path.c_str(), std::ios::binary);
	Common::FTDS::String pack("Pack");
	Common::FileIOHelper::BeginDataPackLoad(stream, pack);
	int value = 42;
	Common::FTDS::String missing("Missing");
	Common::FileIOHelper::LoadInt(stream, missing, value);
}

static void AssertTypeMismatch()
{
	const std::string path = TempPath("foxtrot-keyed-type.chunk");
	WriteChunk(path, "Pack<1>\n\tA[int]\n\t1\n");
	std::ifstream stream;
	stream.open(path.c_str(), std::ios::binary);
	Common::FTDS::String pack("Pack");
	Common::FileIOHelper::BeginDataPackLoad(stream, pack);
	float value = 42.0f;
	Common::FTDS::String key("A");
	Common::FileIOHelper::LoadFloat(stream, key, value);
}

static void AssertParentNotSearched()
{
	const std::string path = TempPath("foxtrot-keyed-parent.chunk");
	WriteChunk(
		path,
		"Parent<2>\n"
		"\tA[int]\n"
		"\t5\n"
		"\tChild<1>\n"
		"\t\tB[int]\n"
		"\t\t6\n");
	std::ifstream stream;
	stream.open(path.c_str(), std::ios::binary);
	Common::FTDS::String parent("Parent");
	Common::FTDS::String child("Child");
	Common::FileIOHelper::BeginDataPackLoad(stream, parent);
	Common::FileIOHelper::BeginDataPackLoad(stream, child);
	int value = 42;
	Common::FTDS::String key("A");
	Common::FileIOHelper::LoadInt(stream, key, value);
}

static void AssertCopyWithoutPack()
{
	const std::string path = TempPath("foxtrot-keyed-closed.chunk");
	WriteChunk(path, "CSV<0>\n");
	std::ifstream stream;
	stream.open(path.c_str(), std::ios::binary);
	Common::FTDS::String csv("CSV");
	Common::FileIOHelper::BeginDataPackLoad(stream, csv);
	Common::FTDS::DynamicArray<Common::FTDS::String> names;
	Common::FileIOHelper::CopyChildDataPackNames(stream, names);
}

int main(int argc, char** argv)
{
	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

	if (argc > 1 && std::strcmp(argv[1], "missing") == 0)
	{
		AssertMissingKey();
		return 0;
	}
	if (argc > 1 && std::strcmp(argv[1], "type") == 0)
	{
		AssertTypeMismatch();
		return 0;
	}
	if (argc > 1 && std::strcmp(argv[1], "parent") == 0)
	{
		AssertParentNotSearched();
		return 0;
	}
	if (argc > 1 && std::strcmp(argv[1], "copy") == 0)
	{
		AssertCopyWithoutPack();
		return 0;
	}

	TestReorderAndStreamPosition();
	TestDuplicateFields();
	TestDuplicatePacksAndChildNames();
	TestNestedBeforeSibling();
	TestSeparateStreams();
	TestLegacyStreamLoads();
	TestEmptyPackPops();
	TestRemainingScalars();
	ExpectAssert(argv[0], "missing");
	ExpectAssert(argv[0], "type");
	ExpectAssert(argv[0], "parent");
	ExpectAssert(argv[0], "copy");

	if (gFailures != 0)
	{
		std::fprintf(stderr, "%d failure(s)\n", gFailures);
		return 1;
	}
	std::puts("PASS keyed chunk load");
	return 0;
}
