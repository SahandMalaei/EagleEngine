#ifndef _SPRITE_H
#define _SPRITE_H 1

#include "Eagle.h"

#define SPRITE_MAX_BLUR_LEVELS 50
#define SPRITE_MAX_BLUR_OFFSET 20

ProjectEagle::Vector2 rotatePoint(float x, float y, float centerX, float centerY, float angle);

namespace ProjectEagle
{
	struct SpriteHistoryElement
	{
		Vector3 rotation;
		int x, y, z;
		ColorValue color;
		short frame, row;
	};

	class Sprite : public GameObject
	{
	protected:
		float m_rotation;

		Vector3 m_velocity;

		Vector2 m_center;

		Vector2	m_scale;

		ColorValue m_color;

		int m_frameStart;

		Texture *m_texture;

		short m_currentFrame, m_frameCount, m_animationDirection, m_animationRow, m_animationColumnCount, m_frameInterval;

		float m_width, m_height;

		short m_textureWidth, m_textureHeight;

		short m_animationStartX, m_animationStartY;

		bool m_imageLoaded;

		bool m_visible;

		void transform();
		void transformBillboard();

	public:
		Sprite();
		virtual ~Sprite();

		// Position

		Vector3 getPosition();
		void setPosition(Vector3 value);
		void setPosition(float x, float y, float z);

		Vector2 getRotatedPosition();

		// Rotation

		float getRotation();
		void setRotation(float value);

		void rotate(float value);

		// Velocity and Acceleration

		Vector3 getVelocity();
		void setVelocity(Vector3 value);
		void setVelocity(float x, float y, float z);

		// Center

		Vector2 getCenter();
		void setCenter(Vector2 value);
		void setCenter(float x, float y);

		// Dimensions

		void setDimensions(float width, float height);

		float getWidth();
		void setWidth(float w);
		float getHeight();
		void setHeight(float h);

		int getTextureWidth();
		int getTextureHeight();

		// Visibility

		bool getVisible();
		void setVisible(bool v);

		// Life state

		bool getAlive();
		void setAlive(bool a);

		// Animation

		int getAnimationColumnCount();
		void setAnimationColumnCount(int value);

		int getFrameInterval();
		void setFrameInterval(int value);
		void setAnimationInterval(int value);

		int getCurrentFrame();
		void setCurrentFrame(int c);

		int getFrameCount();
		void setFrameCount(int t);

		short getAnimationDirection();
		void setAnimationDirection(short value);

		int getAnimationRow();
		void setAnimationRow(short value);

		// Scale

		Vector2 getScale();
		void setScale(float value);
		void setScale(Vector2 value);

		void updateCenterPosition();

		// Color

		ColorValue getColor();
		void setColor(DWORD c);
		void setColor(ColorValue c);

		// Texture

		bool loadImage(char *fileName);

		bool isImageLoaded();

		Texture *getTexture();

		// Main functions

		void move();
		void animate();
		virtual void update();
		virtual void render();
		virtual void renderTransformed();
	};
};

#endif