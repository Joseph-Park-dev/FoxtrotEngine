#pragma once
#include <DirectXMath.h>

struct TextVertex
{
	TextVertex() { }

	TextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h)
		: color(r, g, b, a), texCoord(u, v, tw, th), pos(x, y, w, h) {}

	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 texCoord;
};