#include "Font.h"

#include <fstream>

const float EAGLE_FONT_CHARACTER_SPACING = 1.0 / 17.0;
const float EAGLE_FONT_LINE_SPACING = 1.0 / 17.0;

extern ProjectEagle::ResourceManagerClass resourceManager;

namespace ProjectEagle
{
	BitmapFont::BitmapFont()
	{
		memset(&m_widthList, 0, sizeof(m_widthList));
		m_characterSpacing = EAGLE_FONT_CHARACTER_SPACING;
		m_lineSpacing = EAGLE_FONT_LINE_SPACING;

		m_columnCount = 16;

		m_color = COLOR_RGBA(1, 1, 1, 1);
	}

	bool BitmapFont::loadImage(std::string fileAddress)
	{
		m_texture = resourceManager.loadTexture(fileAddress);

		m_width = m_texture->getWidth();
		m_height = m_texture->getHeight();
		m_textureWidth = m_texture->getWidth();
		m_textureHeight = m_texture->getHeight();

		detectCharacterDimensionsFromTexture();

		return 1;
	}

	void BitmapFont::print(Vector2 position, float depth, std::string text)
	{
		if(text.length() == 0)
		{
			return;
		}

		m_position = Vector3(position.x, position.y, depth);

		graphics.setTexture(m_texture);

		int textLength = text.length();
		for(int i = 0; i < textLength; ++i)
		{
			if(text[i] == '\n')
			{
				m_position.x = position.x;
				m_position.y += m_height * (m_lineSpacing + 1) * m_scale.y;

				continue;
			}

			Vertex v[4];

			m_currentFrame = (int)text[i];
			float offsetX = (m_width - m_widthList[m_currentFrame]) * m_scale.x / 2;
			m_position.x -= offsetX;

			float fx = (m_currentFrame % m_columnCount) * m_width;
			float fy = ((m_currentFrame / m_columnCount)) * m_height;

			v[0].x = m_position.x;
			v[0].y = m_position.y;
			v[0].z = m_position.z;
			v[0].color = m_color;
			v[0].tu = (fx) / m_textureWidth;
			v[0].tv = (fy) / m_textureHeight;

			v[1].x = m_position.x + m_width * m_scale.x;
			v[1].y = m_position.y;
			v[1].z = m_position.z;
			v[1].color = m_color;
			v[1].tu = (fx + m_width) / m_textureWidth;
			v[1].tv = (fy) / m_textureHeight;

			v[2].x = m_position.x;
			v[2].y = m_position.y + m_height * m_scale.y;
			v[2].z = m_position.z;
			v[2].color = m_color;
			v[2].tu = (fx) / m_textureWidth;
			v[2].tv =  (fy + m_height) / m_textureHeight;

			v[3].x = m_position.x + m_width * m_scale.x;
			v[3].y = m_position.y + m_height * m_scale.y;
			v[3].z = m_position.z;
			v[3].color = m_color;
			v[3].tu = (fx + m_width) / m_textureWidth;
			v[3].tv = (fy + m_height) / m_textureHeight;

			graphics.drawPrimitive(v, 2, PrimitiveType_TriangleStrip);

			m_position.x += offsetX;
			m_position.x += m_widthList[m_currentFrame] * (m_characterSpacing + 1) * m_scale.x;
		}

		graphics.setTexture(0);
	}

	void BitmapFont::print(float x, float y, float depth, std::string text)
	{
		print(Vector2(x, y), depth, text);
	}

	void BitmapFont::printTransformed(Vector2 position, float depth, std::string text)
	{
		if(text.length() == 0)
		{
			return;
		}

		m_position = Vector3(position.x, position.y, depth);

		graphics.setTexture(m_texture);

		int textLength = text.length();
		for(int i = 0; i < textLength; ++i)
		{
			if(text[i] == '\n')
			{
				m_position.x = position.x;
				m_position.y += m_height * (m_lineSpacing + 1) * m_scale.y;

				continue;
			}

			Vertex v[4];

			m_currentFrame = (int)text[i];
			float offsetX = (m_width - m_widthList[m_currentFrame]) * m_scale.x / 2;
			m_position.x -= offsetX;

			float fx = (m_currentFrame % m_columnCount) * m_width;
			float fy = ((m_currentFrame / m_columnCount)) * m_height;

			v[0].x = m_position.x;
			v[0].y = m_position.y;
			v[0].z = m_position.z;
			v[0].color = m_color;
			v[0].tu = (fx) / m_textureWidth;
			v[0].tv = (fy) / m_textureHeight;

			v[1].x = m_position.x + m_width * m_scale.x;
			v[1].y = m_position.y;
			v[1].z = m_position.z;
			v[1].color = m_color;
			v[1].tu = (fx + m_width) / m_textureWidth;
			v[1].tv = (fy) / m_textureHeight;

			v[2].x = m_position.x;
			v[2].y = m_position.y + m_height * m_scale.y;
			v[2].z = m_position.z;
			v[2].color = m_color;
			v[2].tu = (fx) / m_textureWidth;
			v[2].tv =  (fy + m_height) / m_textureHeight;

			v[3].x = m_position.x + m_width * m_scale.x;
			v[3].y = m_position.y + m_height * m_scale.y;
			v[3].z = m_position.z;
			v[3].color = m_color;
			v[3].tu = (fx + m_width) / m_textureWidth;
			v[3].tv = (fy + m_height) / m_textureHeight;

			graphics.drawTransformedPrimitive(v, 2, PrimitiveType_TriangleStrip);

			m_position.x += offsetX;
			m_position.x += m_widthList[m_currentFrame] * (m_characterSpacing + 1) * m_scale.x;
		}

		graphics.setTexture(0);
	}

	void BitmapFont::printTransformed(float x, float y, float depth, std::string text)
	{
		printTransformed(Vector2(x, y), depth, text);
	}

	bool BitmapFont::loadWidthData(std::string fileName)
	{
		unsigned char buffer[512];
		std::ifstream inFile;

		inFile.open(fileName.c_str(), std::ios::binary);
		if(!inFile) return 0;

		inFile.read((char *)(&buffer), 512);
		if(inFile.bad()) return 0;

		inFile.clear();

		for(int i = 0; i < 256; ++i)
		{
			m_widthList[i] = (int)buffer[i*2];
		}

		return 1;
	}

	int BitmapFont::getPrintedTextLength(std::string text)
	{
		float length = 0;

		if(!text.length()) return 0;

		for(int i = 0; i < text.length(); ++i)
		{
			length += m_widthList[text[i]] * (m_characterSpacing + 1) * m_scale.x;
		}

		return length;
	}

	void BitmapFont::detectCharacterDimensionsFromTexture()
	{
		setCharacterDimensions(m_texture->getWidth() / 16.0, m_texture->getHeight() / 16.0);
	}

	float BitmapFont::getCharacterWidth()
	{
		return m_width;
	}

	float BitmapFont::getCharacterHeight()
	{
		return m_height;
	}

	void BitmapFont::setCharacterWidth(float value)
	{
		m_width = value;
	}

	void BitmapFont::setCharacterHeight(float value)
	{
		m_height = value;
	}

	void BitmapFont::setCharacterDimensions(float width, float height)
	{
		m_width = width;
		m_height = height;
	}

	float BitmapFont::getCharacterSpacing()
	{
		return m_characterSpacing;
	}

	void BitmapFont::setCharacterSpacing(float value)
	{
		m_characterSpacing = value;
	}

	float BitmapFont::getLineSpacing()
	{
		return m_lineSpacing;
	}

	void BitmapFont::setLineSpacing(float value)
	{
		m_lineSpacing = value;
	}

	Texture *BitmapFont::getTexture()
	{
		return m_texture;
	}

	ColorValue BitmapFont::getColor()
	{
		return m_color;
	}

	void BitmapFont::setColor(ColorValue value)
	{
		m_color = value;
	}

	Vector3 BitmapFont::getPosition()
	{
		return m_position;
	}

	void BitmapFont::setPosition(Vector3 value)
	{
		m_position = value;
	}

	void BitmapFont::setPosition(float x, float y, float z)
	{
		m_position = Vector3(x, y, z);
	}

	Vector2 BitmapFont::getScale()
	{
		return m_scale;
	}

	void BitmapFont::setScale(float value)
	{
		m_scale = Vector2(value, value);
	}

	void BitmapFont::setScale(Vector2 value)
	{
		m_scale = value;
	}
};