#ifndef _FONT_H
#define _FONT_H 1

#include "Eagle.h"

namespace ProjectEagle
{
	class BitmapFont
	{
	private:
		Texture *m_texture;
		ColorValue m_color;

		Vector3 m_position;
		Vector2 m_scale;

		float m_width, m_height;

		short m_textureWidth, m_textureHeight;

		int m_columnCount;
		int m_currentFrame;

		float m_characterSpacing, m_lineSpacing;
		float m_widthList[256];

	public:
		BitmapFont();
		virtual ~BitmapFont(){}

		bool loadImage(std::string fileAddress);

		void print(Vector2 position, float depth, std::string text);
		void print(float x, float y, float depth, std::string text);
		void printTransformed(Vector2 position, float depth, std::string text);
		void printTransformed(float x, float y, float depth, std::string text);

		float getCharacterWidth();
		float getCharacterHeight();
		void setCharacterWidth(float value);
		void setCharacterHeight(float value);
		void setCharacterDimensions(float width, float height);

		void detectCharacterDimensionsFromTexture();

		bool loadWidthData(std::string fileName);

		int getPrintedTextLength(std::string text);

		float getCharacterSpacing();
		void setCharacterSpacing(float value);

		float getLineSpacing();
		void setLineSpacing(float value);

		Texture *getTexture();

		ColorValue getColor();
		void setColor(ColorValue value);

		Vector3 getPosition();
		void setPosition(Vector3 value);
		void setPosition(float x, float y, float z);

		Vector2 getScale();
		void setScale(float value);
		void setScale(Vector2 value);
	};
};

#endif