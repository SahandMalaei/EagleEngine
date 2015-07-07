#ifndef _HELPERS_H
#define _HELPERS_H 1

#include "Eagle.h"

#define FLOAT_TO_STRING(s) to_string((long float)s)
#define LONG_TO_STRING(s) to_string((long)s)
#define INT_TO_STRING(s) to_string((int)s)

#define IS_STRING_VALID(s) (std::string(s).length())

namespace ProjectEagle
{
	class Vector2
	{
	public:
		float x, y;

		Vector2();
		Vector2(const Vector2 &v);
		Vector2(float x, float y);
		void set(float x1, float y1);
		void set(const Vector2 &v);
		void move(Vector2 value){x += value.x; y += value.y;}
		void move(float x, float y){this->x += x; this->y += y;}
		float getX(){return x;}
		void setX(float v){x = v;}
		float getY(){return y;}
		void setY(float v){y = v;}

		void operator += (const Vector2 &v);
		void operator -= (const Vector2 &v);
		void operator *= (const Vector2 &v);
		void operator /= (const Vector2 &v);
		bool operator == (const Vector2 &v);
		bool operator != (const Vector2 &v);

		Vector2 operator * (const float &d);
		Vector2 operator / (const float &d);

		Vector2 operator * (const Vector2 &d);
		Vector2 operator / (const Vector2 &d);

		Vector2 operator + (const Vector2 &d);
		Vector2 operator - (const Vector2 &d);

		void operator *= (const float &d);
		void operator /= (const float &d);

		Vector2 operator = (const Vector2 &v);
		float distance(const Vector2 &v);
		float length();
		float dotProduct(const Vector2 &v);
		//Vector2 crossProduct(const Vector2 &v);
		Vector2 normal();
		void normalize();
		Vector2 getProjected(Vector2 axis);
		void project(Vector2 axis);
	};

	class Vector3
	{
	public:
		float x, y, z;
		Vector3();
		Vector3(const Vector3 &v);
		Vector3(float x, float y, float z);
		void set(float x1, float y1, float z1);
		void set(const Vector3 &v);
		void move(Vector3 value){x += value.x; y += value.y; z += value.z;}
		void move(float x, float y, float z){this->x += x; this->y += y; this->z += z;}
		float getX(){return x;}
		void setX(float v){x = v;}
		float getY(){return y;}
		void setY(float v){y = v;}
		float getZ(){return z;}
		void setZ(float v){z = v;}

		void operator += (const Vector3 &v);
		void operator -= (const Vector3 &v);
		void operator *= (const Vector3 &v);
		void operator /= (const Vector3 &v);
		bool operator == (const Vector3 &v);
		bool operator != (const Vector3 &v);

		Vector3 operator * (const float &d);
		Vector3 operator / (const float &d);

		Vector3 operator * (const Vector3 &d);
		Vector3 operator / (const Vector3 &d);

		Vector3 operator + (const Vector3 &d);
		Vector3 operator - (const Vector3 &d);

		void operator *= (const float &d);
		void operator /= (const float &d);

		Vector3 operator = (const Vector3 &v);

		float distance(const Vector3 &v);
		float length();
		Vector3 normal();
	};

	class Rectangle
	{
	public:
		float left, top, width, height;
		Rectangle();
		Rectangle(const Rectangle &rectangle);
		Rectangle(Vector2 topLeft, Vector2 dimensions);
		Rectangle(float left, float top, float width, float height);
		void set(Vector2 topLeft, Vector2 dimensions);
		void set(float left, float top, float width, float height);

		void setPosition(Vector2 position);
		void setPosition(float x, float y);

		void move(Vector2 movementVector);
		void move(float x, float y);

		bool isRectangleIntersecting(Rectangle rectangle);
		bool isPointIntersecting(Vector2 v);
		bool isPointIntersecting(float x, float y);
	};

	class RotatedRectangle
	{
	public:
		float left, top, width, height, centerX, centerY, rotation;
		RotatedRectangle();
		RotatedRectangle(const RotatedRectangle &rectangle);
		RotatedRectangle(const Rectangle &rectangle, Vector2 center, float rotation);
		RotatedRectangle(float left, float top, float width, float height, float centerX, float centerY, float rotation);
		void set(Vector2 topLeft, Vector2 dimensions, Vector2 center, float rotation);
		void set(float left, float top, float width, float height, float centerX, float centerY, float rotation);

		void setPosition(Vector2 position);
		void setPosition(float x, float y);

		void move(Vector2 movementVector);
		void move(float x, float y);

		bool isRectangleIntersecting(const RotatedRectangle &rectangle);
		bool isPointIntersecting(Vector2 v);
		bool isPointIntersecting(float x, float y);
	};

	enum Color
	{
		Color_AliceBlue = 0xFFF0F8FF,
		Color_AntiqueWhite = 0xFFFAEBD7,
		Color_Aqua = 0xFF00FFFF,
		Color_Aquamarine = 0xFF7FFFD4,
		Color_Azure = 0xFFF0FFFF,
		Color_Beige = 0xFFF5F5DC,
		Color_Bisque = 0xFFFFE4C4,
		Color_Black = 0xFF000000,
		Color_BlanchedAlmond = 0xFFFFEBCD,
		Color_Blue = 0xFF0000FF,
		Color_BlueViolet = 0xFF8A2BE2,
		Color_Brown = 0xFFA52A2A,
		Color_BurlyWood = 0xFFDEB887,
		Color_CadetBlue = 0xFF5F9EA0,
		Color_Chartreuse = 0xFF7FFF00,
		Color_Chocolate = 0xFFD2691E,
		Color_Coral = 0xFFFF7F50,
		Color_CornflowerBlue = 0xFF6495ED,
		Color_Cornsilk = 0xFFFFF8DC,
		Color_Crimson = 0xFFDC143C,
		Color_Cyan = 0xFF00FFFF,
		Color_DarkBlue = 0xFF00008B,
		Color_DarkCyan = 0xFF008B8B,
		Color_DarkGoldenrod = 0xFFB8860B,
		Color_DarkGray = 0xFFA9A9A9,
		Color_DarkGreen = 0xFF006400,
		Color_DarkKhaki = 0xFFBDB76B,
		Color_DarkMagenta = 0xFF8B008B,
		Color_DarkOliveGreen = 0xFF556B2F,
		Color_DarkOrange = 0xFFFF8C00,
		Color_DarkOrchid = 0xFF9932CC,
		Color_DarkRed = 0xFF8B0000,
		Color_DarkSalmon = 0xFFE9967A,
		Color_DarkSeaGreen = 0xFF8FBC8F,
		Color_DarkSlateBlue = 0xFF483D8B,
		Color_DarkSlateGray = 0xFF2F4F4F,
		Color_DarkTurquoise = 0xFF00CED1,
		Color_DarkViolet = 0xFF9400D3,
		Color_DeepPink = 0xFFFF1493,
		Color_DeepSkyBlue = 0xFF00BFFF,
		Color_DimGray = 0xFF696969,
		Color_DodgerBlue = 0xFF1E90FF,
		Color_Firebrick = 0xFFB22222,
		Color_FloralWhite = 0xFFFFFAF0,
		Color_ForestGreen = 0xFF228B22,
		Color_Fuchsia = 0xFFFF00FF,
		Color_Gainsboro = 0xFFDCDCDC,
		Color_GhostWhite = 0xFFF8F8FF,
		Color_Gold = 0xFFFFD700,
		Color_Goldenrod = 0xFFDAA520,
		Color_Gray = 0xFF808080,
		Color_Green = 0xFF008000,
		Color_GreenYellow = 0xFFADFF2F,
		Color_Honeydew = 0xFFF0FFF0,
		Color_HotPink = 0xFFFF69B4,
		Color_IndianRed = 0xFFCD5C5C,
		Color_Indigo = 0xFF4B0082,
		Color_Ivory = 0xFFFFFFF0,
		Color_Khaki = 0xFFF0E68C,
		Color_Lavender = 0xFFE6E6FA,
		Color_LavenderBlush = 0xFFFFF0F5,
		Color_LawnGreen = 0xFF7CFC00,
		Color_LemonChiffon = 0xFFFFFACD,
		Color_LightBlue = 0xFFADD8E6,
		Color_LightCoral = 0xFFF08080,
		Color_LightCyan = 0xFFE0FFFF,
		Color_LightGoldenrodYellow = 0xFFFAFAD2,
		Color_LightGreen = 0xFF90EE90,
		Color_LightGray = 0xFFD3D3D3,
		Color_LightPink = 0xFFFFB6C1,
		Color_LightSalmon = 0xFFFFA07A,
		Color_LightSeaGreen = 0xFF20B2AA,
		Color_LightSkyBlue = 0xFF87CEFA,
		Color_LightSlateGray = 0xFF778899,
		Color_LightSteelBlue = 0xFFB0C4DE,
		Color_LightYellow = 0xFFFFFFE0,
		Color_Lime = 0xFF00FF00,
		Color_LimeGreen = 0xFF32CD32,
		Color_Linen = 0xFFFAF0E6,
		Color_Magenta = 0xFFFF00FF,
		Color_Maroon = 0xFF800000,
		Color_MediumAquamarine = 0xFF66CDAA,
		Color_MediumBlue = 0xFF0000CD,
		Color_MediumOrchid = 0xFFBA55D3,
		Color_MediumPurple = 0xFF9370DB,
		Color_MediumSeaGreen = 0xFF3CB371,
		Color_MediumSlateBlue = 0xFF7B68EE,
		Color_MediumSpringGreen = 0xFF00FA9A,
		Color_MediumTurquoise = 0xFF48D1CC,
		Color_MediumVioletRed = 0xFFC71585,
		Color_MidnightBlue = 0xFF191970,
		Color_MintCream = 0xFFF5FFFA,
		Color_MistyRose = 0xFFFFE4E1,
		Color_Moccasin = 0xFFFFE4B5,
		Color_NavajoWhite = 0xFFFFDEAD,
		Color_Navy = 0xFF000080,
		Color_OldLace = 0xFFFDF5E6,
		Color_Olive = 0xFF808000,
		Color_OliveDrab = 0xFF6B8E23,
		Color_Orange = 0xFFFFA500,
		Color_OrangeRed = 0xFFFF4500,
		Color_Orchid = 0xFFDA70D6,
		Color_PaleGoldenrod = 0xFFEEE8AA,
		Color_PaleGreen = 0xFF98FB98,
		Color_PaleTurquoise = 0xFFAFEEEE,
		Color_PaleVioletRed = 0xFFDB7093,
		Color_PapayaWhip = 0xFFFFEFD5,
		Color_PeachPuff = 0xFFFFDAB9,
		Color_Peru = 0xFFCD853F,
		Color_Pink = 0xFFFFC0CB,
		Color_Plum = 0xFFDDA0DD,
		Color_PowderBlue = 0xFFB0E0E6,
		Color_Purple = 0xFF800080,
		Color_Red = 0xFFFF0000,
		Color_RosyBrown = 0xFFBC8F8F,
		Color_RoyalBlue = 0xFF4169E1,
		Color_SaddleBrown = 0xFF8B4513,
		Color_Salmon = 0xFFFA8072,
		Color_SandyBrown = 0xFFF4A460,
		Color_SeaGreen = 0xFF2E8B57,
		Color_SeaShell = 0xFFFFF5EE,
		Color_Sienna = 0xFFA0522D,
		Color_Silver = 0xFFC0C0C0,
		Color_SkyBlue = 0xFF87CEEB,
		Color_SlateBlue = 0xFF6A5ACD,
		Color_SlateGray = 0xFF708090,
		Color_Snow = 0xFFFFFAFA,
		Color_SpringGreen = 0xFF00FF7F,
		Color_SteelBlue = 0xFF4682B4,
		Color_Tan = 0xFFD2B48C,
		Color_Teal = 0xFF008080,
		Color_Thistle = 0xFFD8BFD8,
		Color_Tomato = 0xFFFF6347,
		Color_Turquoise = 0xFF40E0D0,
		Color_Violet = 0xFFEE82EE,
		Color_Wheat = 0xFFF5DEB3,
		Color_White = 0xFFFFFFFF,
		Color_WhiteSmoke = 0xFFF5F5F5,
		Color_Yellow = 0xFFFFFF00,
		Color_YellowGreen = 0xFF9ACD32
	};

	bool isCharNumeric(char c);
	bool isCharAlphabetic(char c);
	bool isCharWhitespace(char c);
	bool isCharDelimiter(char c);

	bool isStringInt(std::string s);
	bool isStringFloat(std::string s);
	bool isStringAlphabetic(std::string s);
	bool isStringWhitespace(std::string s);
	bool isStringValidIdentifier(std::string s);

	bool floatEquality(float number0, float number1);

	BOOL WINAPI AnsiToUnicode(LPSTR ansiString, LPWSTR unicodeBuffer, DWORD unicodeBufferSize);
	BOOL WINAPI UnicodeToAnsi(LPWSTR unicodeString, LPSTR  ansiBuffer, DWORD ansiBufferSize);

	//int asciiHexToInt(string hexString);
};

#endif