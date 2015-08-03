#include "Vertex.h"

#define COLOR_GET_B(color) (int)(((DWORD)color) & 0x0000ff)
#define COLOR_GET_G(color) (int)((((DWORD)color) & 0x00ff00) / 0x000100)
#define COLOR_GET_R(color) (int)((((DWORD)color) & 0xff0000) / 0x010000)
#define COLOR_GET_A(color) (int)((((DWORD)color) & 0xff000000) / 0x01000000)

namespace ProjectEagle
{
	ColorValue::ColorValue()
	{
		r = 1.0f;
		g = 1.0f;
		b = 1.0f;
		a = 1.0f;
	}

	ColorValue::ColorValue(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	ColorValue::ColorValue(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 1.0f;
	}

	ColorValue::ColorValue(DWORD c)
	{
		a = COLOR_GET_A(c) / 255.0;
		r = COLOR_GET_R(c) / 255.0;
		g = COLOR_GET_G(c) / 255.0;
		b = COLOR_GET_B(c) / 255.0;
	}

	void ColorValue::set(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	void ColorValue::set(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 1.0f;
	}

	ColorValue& ColorValue::operator = (const DWORD &c)
	{
		a = COLOR_GET_A(c) / 255.0;
		r = COLOR_GET_R(c) / 255.0;
		g = COLOR_GET_G(c) / 255.0;
		b = COLOR_GET_B(c) / 255.0;

		return *this;
	}

	/*DWORD ColorValue::getDWORD()
	{
		//return COLOR_RGBA_DWORD((int)(r * 255.0), (int)(g * 255.0), (int)(b * 255.0), (int)(a * 255.0));
		return r * 255 + g * 255 * 256 + b * 255 * 256 * 256 + a * 255 * 256 * 256 * 256;
	}*/
}