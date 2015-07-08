#include "Eagle.h"

Vector2 rotatePoint(float x, float y, float centerX, float centerY, float angle)
{
	float s = sin(angle);
	float c = cos(angle);

	x -= centerX;
	y -= centerY;

	Vector2 p;
	p.x = x * c - y * s + centerX;
	p.y = x * s + y * c + centerY;

	return p;
}

namespace ProjectEagle
{
	Sprite::Sprite()
	{
		m_objectType = GAMEOBJECT_SPRITE;

		m_texture = 0;
		m_imageLoaded = 0;

		m_position.set(0.0f, 0.0f, 0.0f);

		m_velocity.set(0.0f, 0.0f, 0.0f);

		m_width = 1;
		m_height = 1;

		m_currentFrame = 0;
		m_animationRow = 0;
		m_frameCount = 1;
		m_animationDirection = 1;
		m_animationColumnCount = 1;
		m_frameStart = eagle.getTimer()->getPassedTimeMilliseconds();
		m_frameInterval = 0;
		m_animationStartX = 0;
		m_animationStartY = 0;

		m_rotation = 0.0;

		setScale(1.0f);

		m_color = 0xFFFFFFFF;

		m_visible = 1;
		m_alive = 1;
	}

	Sprite::~Sprite()
	{
		//if(imageLoaded) delete image;
	}

	bool Sprite::loadImage(char *fileName)
	{
		m_texture = resourceManager.loadTexture(fileName);

		setWidth(m_texture->getWidth());
		setHeight(m_texture->getHeight());
		m_textureWidth = m_texture->getWidth();
		m_textureHeight = m_texture->getHeight();

		switch(graphics.getGraphicsAPIType())
		{
		case GraphicsAPI_Direct3D11:
			{

				break;
			}
		}

		updateCenterPosition();

		m_imageLoaded = 1;

		return 1;
	}

	void Sprite::transform()
	{
		switch(graphics.getGraphicsAPIType())
		{
		case GraphicsAPI_Direct3D11:
			{
				break;
			}
		}
	}

	void Sprite::transformBillboard()
	{
	}

	void Sprite::update()
	{
		move();
		animate();
		parentalUpdate();
	}

	void Sprite::render()
	{
		if(!m_visible)
		{
			return;
		}

		float fx = (m_currentFrame % m_animationColumnCount) * m_width;
		float fy = ((m_currentFrame / m_animationColumnCount) + m_animationRow) * m_height;

		transform();

		Vertex v[4];

		Vector2 pos0 = math.rotatePoint(0, 0, m_center.x, m_center.y, m_rotation);
		v[0].x = m_position.x + pos0.x * m_scale.x;
		v[0].y = m_position.y + pos0.y * m_scale.y;
		v[0].z = m_position.z;
		v[0].color = m_color;
		v[0].tu = (fx) / m_textureWidth;
		v[0].tv = (fy) / m_textureHeight;

		Vector2 pos1 = math.rotatePoint(m_width, 0, m_center.x, m_center.y, m_rotation);
		v[1].x = m_position.x + pos1.x * m_scale.x;
		v[1].y = m_position.y + pos1.y * m_scale.y;
		v[1].z = m_position.z;
		v[1].color = m_color;
		v[1].tu = (fx + m_width/* - 1*/) / m_textureWidth;
		v[1].tv = (fy) / m_textureHeight;

		Vector2 pos2 = math.rotatePoint(0, m_height, m_center.x, m_center.y, m_rotation);
		v[2].x = m_position.x + pos2.x * m_scale.x;
		v[2].y = m_position.y + pos2.y * m_scale.y;
		v[2].z = m_position.z;
		v[2].color = m_color;
		v[2].tu = (fx) / m_textureWidth;
		v[2].tv =  (fy + m_height/* - 1*/) / m_textureHeight;

		Vector2 pos3 = math.rotatePoint(m_width, m_height, m_center.x, m_center.y, m_rotation);
		v[3].x = m_position.x + pos3.x * m_scale.x;
		v[3].y = m_position.y + pos3.y * m_scale.y;
		v[3].z = m_position.z;
		v[3].color = m_color;
		v[3].tu = (fx + m_width/* - 1*/) / m_textureWidth;
		v[3].tv = (fy + m_height/* - 1*/) / m_textureHeight;

		graphics.setTexture(m_texture);
		graphics.drawPrimitive(v, 2, PrimitiveType_TriangleStrip);
		graphics.setTexture(0);

		graphics.setWorldIdentity();

		return;
	}

	void Sprite::renderTransformed()
	{
		if(!m_visible)
		{
			return;
		}

		float fx = (m_currentFrame % m_animationColumnCount) * m_width;
		float fy = ((m_currentFrame / m_animationColumnCount) + m_animationRow) * m_height;

		if(math.approximateEquals(m_rotation, 0))
		{
			Vertex v[4];

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

			graphics.setTexture(m_texture);
			graphics.drawTransformedPrimitive(v, 2, PrimitiveType_TriangleStrip);
		}
		else
		{
			Vertex v[4];

			Vector2 pos0 = math.rotatePoint(0, 0, m_center.x, m_center.y, m_rotation);
			v[0].x = m_position.x + pos0.x * m_scale.x;
			v[0].y = m_position.y + pos0.y * m_scale.y;
			v[0].z = m_position.z;
			v[0].color = m_color;
			v[0].tu = (fx) / m_textureWidth;
			v[0].tv = (fy) / m_textureHeight;

			Vector2 pos1 = math.rotatePoint(m_width, 0, m_center.x, m_center.y, m_rotation);
			v[1].x = m_position.x + pos1.x * m_scale.x;
			v[1].y = m_position.y + pos1.y * m_scale.y;
			v[1].z = m_position.z;
			v[1].color = m_color;
			v[1].tu = (fx + m_width/* - 1*/) / m_textureWidth;
			v[1].tv = (fy) / m_textureHeight;

			Vector2 pos2 = math.rotatePoint(0, m_height, m_center.x, m_center.y, m_rotation);
			v[2].x = m_position.x + pos2.x * m_scale.x;
			v[2].y = m_position.y + pos2.y * m_scale.y;
			v[2].z = m_position.z;
			v[2].color = m_color;
			v[2].tu = (fx) / m_textureWidth;
			v[2].tv =  (fy + m_height/* - 1*/) / m_textureHeight;

			Vector2 pos3 = math.rotatePoint(m_width, m_height, m_center.x, m_center.y, m_rotation);
			v[3].x = m_position.x + pos3.x * m_scale.x;
			v[3].y = m_position.y + pos3.y * m_scale.y;
			v[3].z = m_position.z;
			v[3].color = m_color;
			v[3].tu = (fx + m_width/* - 1*/) / m_textureWidth;
			v[3].tv = (fy + m_height/* - 1*/) / m_textureHeight;

			graphics.setTexture(m_texture);
			graphics.drawTransformedPrimitive(v, 2, PrimitiveType_TriangleStrip);
		}

		graphics.setTexture(0);
	}

	/*void Sprite::drawWithout3DTransformation()
	{
	}*/

	/*void Sprite::drawBillboard()
	{
		if(!m_visible)
		{
			return;
		}

		float fx = (m_currentFrame % m_animationColumnCount) * m_width;
		float fy = ((m_currentFrame / m_animationColumnCount) + m_animationRow) * m_height;

		transformBillboard();

		Vertex v[4];
		v[0].x = 0;
		v[0].y = 0;
		v[0].z = 0;
		v[0].color = m_color;
		//v[0].weight = 1;
		v[0].tu = (fx) / m_textureWidth;
		v[0].tv = (fy) / m_textureHeight;
		v[1].x = m_width;
		v[1].y = 0;
		v[1].z = 0;
		v[1].color = m_color;
		//v[1].weight = 1;
		v[1].tu = (fx + m_width - 1) / m_textureWidth;
		v[1].tv = (fy) / m_textureHeight;
		v[2].x = 0;
		v[2].y = m_height;
		v[2].z = 0;
		v[2].color = m_color;
		//v[2].weight = 1;
		v[2].tu = (fx) / m_textureWidth;
		v[2].tv =  (fy + m_height - 1) / m_textureHeight;
		v[3].x = m_width;
		v[3].y = m_height;
		v[3].z = 0;
		v[3].color = m_color;
		//v[3].weight = 1;
		v[3].tu = (fx + m_width - 1) / m_textureWidth;
		v[3].tv = (fy + m_height - 1) / m_textureHeight;

		graphics.setWorldIdentity();

		m_position.z *= -1;

		return;
	}*/

	void Sprite::move()
	{
		m_position += m_velocity * (*m_frameTime);
	}

	void Sprite::animate()
	{
		if(m_frameInterval > 0)
		{
			DWORD currentTime = eagle.getTimer()->getPassedTimeMilliseconds();

			if(currentTime > m_frameStart + m_frameInterval)
			{
				m_frameStart = currentTime;

				m_currentFrame += m_animationDirection;

				if(m_currentFrame < 0) m_currentFrame = m_frameCount - 1;
				if(m_currentFrame > m_frameCount - 1) m_currentFrame = 0;
			}
		}
		else
		{
			m_currentFrame += m_animationDirection;

			if(m_currentFrame < 0) m_currentFrame = m_frameCount - 1;
			if(m_currentFrame > m_frameCount - 1) m_currentFrame = 0;
		}
	}

	Vector3 Sprite::getPosition()
	{
		return m_position;
	}

	void Sprite::setPosition(Vector3 value)
	{
		m_position = value;
	}

	void Sprite::setPosition(float x, float y, float z)
	{
		m_position.set(x, y, z);
	}
	
	Vector2 Sprite::getRotatedPosition()
	{
		return rotatePoint(m_position.x, m_position.y, m_center.x, m_center.y, m_rotation);
	}

	float Sprite::getRotation()
	{
		return m_rotation;
	}

	void Sprite::setRotation(float value)
	{
		m_rotation = value;
	}

	void Sprite::rotate(float value)
	{
		m_rotation += value;
	}

	Vector3 Sprite::getVelocity()
	{
		return m_velocity;
	}

	void Sprite::setVelocity(Vector3 value)
	{
		m_velocity = value;
	}

	void Sprite::setVelocity(float x, float y, float z)
	{
		m_velocity.set(x, y, z);
	}

	Vector2 Sprite::getCenter()
	{
		return m_center;
	}

	void Sprite::setCenter(Vector2 value)
	{
		m_center = value;
	}

	void Sprite::setCenter(float x, float y)
	{
		m_center.set(x, y);
	}

	void Sprite::setDimensions(float width, float height)
	{
		m_width = width; m_height = height;
	}

	float Sprite::getWidth()
	{
		return m_width;
	}

	void Sprite::setWidth(float w)
	{
		m_width = w;
	}

	float Sprite::getHeight()
	{
		return m_height;
	}

	void Sprite::setHeight(float h)
	{
		m_height = h;
	}

	int Sprite::getTextureWidth()
	{
		return m_textureWidth;
	}

	int Sprite::getTextureHeight()
	{
		return m_textureHeight;
	}

	bool Sprite::getVisible()
	{
		return m_visible;
	}

	void Sprite::setVisible(bool v)
	{
		m_visible = v;
	}

	int Sprite::getAnimationColumnCount()
	{
		return m_animationColumnCount;
	}

	void Sprite::setAnimationColumnCount(int value)
	{
		m_animationColumnCount = value;
	}

	int Sprite::getFrameInterval()
	{
		return m_frameInterval;
	}

	void Sprite::setFrameInterval(int value)
	{
		m_frameInterval = value;
	}

	void Sprite::setAnimationInterval(int value)
	{
		m_frameInterval = value;
	}

	int Sprite::getCurrentFrame()
	{
		return m_currentFrame;
	}

	void Sprite::setCurrentFrame(int c)
	{
		m_currentFrame = c;
	}

	int Sprite::getFrameCount()
	{
		return m_frameCount;
	}

	void Sprite::setFrameCount(int t)
	{
		m_frameCount = t;
	}

	short Sprite::getAnimationDirection()
	{
		return m_animationDirection;
	}

	void Sprite::setAnimationDirection(short value)
	{
		m_animationDirection = value;
	}

	int Sprite::getAnimationRow()
	{
		return m_animationRow;
	}

	void Sprite::setAnimationRow(short value)
	{
		m_animationRow = value;
	}

	Vector2 Sprite::getScale()
	{
		return m_scale;
	}

	void Sprite::setScale(float value)
	{
		m_scale.set(value, value);
	}

	void Sprite::setScale(Vector2 value)
	{
		m_scale = value;
	}

	void Sprite::updateCenterPosition()
	{
		setCenter((m_width * m_scale.x) / 2, (m_height * m_scale.y) / 2);
	}

	ColorValue Sprite::getColor()
	{
		return m_color;
	}

	void Sprite::setColor(DWORD c)
	{
		m_color = c;
	}

	void Sprite::setColor(ColorValue c)
	{
		m_color = c;
	}

	bool Sprite::isImageLoaded()
	{
		return m_imageLoaded;
	}

	Texture *Sprite::getTexture()
	{
		return m_texture;
	}
};