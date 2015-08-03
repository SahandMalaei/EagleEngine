#include "Eagle.h"

namespace ProjectEagle
{
	ParticleSystem::ParticleSystem()
	{
		m_objectType = GAMEOBJECT_PARTICLEEMITTER;
		m_maxParticleCount = 100;

		m_particleCount = 0;
		aliveParticleCount = 0;

		m_particleList = new Particle[m_maxParticleCount];

		m_vertexList = new Vertex[m_maxParticleCount * 4];

		m_inactiveParticleList = new int[m_maxParticleCount];
		m_inactiveParticleListTopIndex = 0;

		m_maximumDistance = 500;
		m_direction = 0;
		m_minimumAlpha = 0;
		m_maximumAlpha = 1;
		m_minimumRed = 0;
		m_maximumRed = 1;
		m_minimumGreen = 0;
		m_maximumGreen = 1;
		m_minimumBlue = 0;
		m_maximumBlue = 1;
		m_spread = 2 * pi;
		m_velocity = 50.0f;
		m_initialVelocity = Vector2(0, 0);
		m_scale = 1.0f;
		m_addTimer.reset();
		m_addDelay = 1;
		//numImages = 1;
		m_lifespan = 1;
		m_alphaDecrement = 0.1;
		m_removalAlphaDecrement = 0.2;
		m_sourceLineLength = 1;
		m_sourceLineDirection = 0;
		//depthSpread = 1;
		//lifeSpanRandomFactor = 0;
		//m_velocityRandomFactor = 0.0;
		//vertexBuffer = eagle.getVertexBuffer();
		//d3d = eagle.getDirect3D();
		//transformed = 0;
		m_noise = 0.0;
		m_frictionFactor = 1.0;
		m_acceleration = Vector2(0, 0);
		//zDirRandomFactor = 0;
		m_particlesPerAdd = 1;
		m_maximumParticleCreationDistance = 1;
		m_lastPosition = m_position;
	}

	void ParticleSystem::operator = (const ParticleSystem &p)
	{
		m_maxParticleCount = p.m_maxParticleCount;

		m_particleCount = p.m_particleCount;
		aliveParticleCount = p.aliveParticleCount;

		setMaxParticleCount(p.m_maxParticleCount);

		m_inactiveParticleListTopIndex = p.m_inactiveParticleListTopIndex;

		for(int i = 0; i < m_maxParticleCount; ++i)
		{
			m_particleList[i] = p.m_particleList[i];

			m_vertexList[i] = p.m_vertexList[i];

			m_inactiveParticleList[i] = p.m_inactiveParticleList[i];
		}

		m_maximumDistance = p.m_maximumDistance;
		m_direction = p.m_direction;
		m_minimumAlpha = p.m_minimumAlpha;
		m_maximumAlpha = p.m_maximumAlpha;
		m_minimumRed = p.m_minimumRed;
		m_maximumRed = p.m_maximumRed;
		m_minimumGreen = p.m_minimumGreen;
		m_maximumGreen = p.m_maximumGreen;
		m_minimumBlue = p.m_minimumBlue;
		m_maximumBlue = p.m_maximumBlue;
		m_spread = p.m_spread;
		m_velocity = p.m_velocity;
		m_initialVelocity = p.m_initialVelocity;
		m_scale = p.m_scale;
		m_addTimer.reset();
		m_addDelay = p.m_addDelay;
		m_lifespan = p.m_lifespan;
		m_alphaDecrement = p.m_alphaDecrement;
		m_removalAlphaDecrement = p.m_removalAlphaDecrement;
		m_sourceLineLength = p.m_sourceLineLength;
		m_sourceLineDirection = p.m_sourceLineDirection;
		m_noise = p.m_noise;
		m_frictionFactor = p.m_frictionFactor;
		m_acceleration = p.m_acceleration;
		m_particlesPerAdd = p.m_particlesPerAdd;
		m_maximumParticleCreationDistance = 1;

		m_position = p.m_position;
		m_lastPosition = p.m_lastPosition;
		m_lifeSpan = p.m_lifeSpan;

		m_stretchFactor = 1;
	}

	void ParticleSystem::reset()
	{
		m_particleCount = 0;

		delete[](m_particleList);
		m_particleList = new Particle[m_maxParticleCount];

		delete[](m_vertexList);
		m_vertexList = new Vertex[m_maxParticleCount];

		delete[](m_inactiveParticleList);
		m_inactiveParticleList = new int[m_maxParticleCount];

		m_inactiveParticleListTopIndex = 0;
	}

	bool ParticleSystem::loadImage(char *fileName)
	{
		m_texture = resourceManager.loadTexture(fileName);

		return 1;
	}

	ParticleSystem::~ParticleSystem()
	{
		delete(m_particleList);
		delete(m_vertexList);
		delete(m_inactiveParticleList);
	}

	void ParticleSystem::update()
	{
		float frameTime = eagle.getFrameTime();

		m_moveAmount = m_lastPosition - m_position;
		m_moveNormal = m_moveAmount.normal();
		m_moveLength = m_moveAmount.length();

		if(m_addTimer.stopWatch(m_addDelay))
		{
			if(m_particleCount == 0)
			{
				m_moveAmount = Vector3(0, 0, 0);
				m_moveNormal = Vector3(0, 0, 0);
				m_moveLength = 0;
			}

			for(int j = 0; j < m_particlesPerAdd * min<int>(frameTime / EAGLE_DEFAULT_FRAME_TIME, 2); ++j)
			{
				add();
			}

			m_addTimer.reset();
		}

		aliveParticleCount = 0;
#ifndef PLATFORM_WP8
		parallel_for(tbb::blocked_range<int>(0, m_particleCount), ParallelParticleSystemUpdate(this, m_particleCount, m_particleList), tbb::auto_partitioner());
#else
		float currentTime = eagle.getTimer()->getPassedTimeSeconds();
		float frictionEffect = 1.0f - (1.0f - m_frictionFactor) * (frameTime);
		float aliveParticleAlphaDecrement = m_alphaDecrement * (frameTime);
		float deadParticleAlphaDecrement = (m_alphaDecrement + m_removalAlphaDecrement) * (frameTime);

		for(int i = 0; i < m_particleCount; ++i)
		{
			if(!m_particleList[i].enabled)
			{
				continue;
			}

			aliveParticleCount++;

			m_particleList[i].velocityX += ((float)(math.random(0, 2) - 1)) * m_noise;
			m_particleList[i].velocityY += ((float)(math.random(0, 2) - 1)) * m_noise;

			m_particleList[i].x += m_particleList[i].velocityX * (frameTime);
			m_particleList[i].y += m_particleList[i].velocityY * (frameTime);

			m_particleList[i].velocityX *= frictionEffect;
			m_particleList[i].velocityY *= frictionEffect;

			m_particleList[i].velocityX += m_particleList[i].accelerationX * (frameTime);
			m_particleList[i].velocityY += m_particleList[i].accelerationY * (frameTime);

			if(m_particleList[i].quickFade)
			{
				m_particleList[i].color.a -= deadParticleAlphaDecrement;
			}
			else
			{
				m_particleList[i].color.a -= aliveParticleAlphaDecrement;
			}

			if(m_particleList[i].color.a < EAGLE_PARTICLE_SYSTEM_MINIMUM_PARTICLE_ALPHA)
			{
				m_particleList[i].enabled = 0;
				m_particleList[i].color.a = 0;

				m_inactiveParticleListTopIndex++;

				m_inactiveParticleList[m_inactiveParticleListTopIndex] = i;

				continue;
			}
			else if(currentTime - m_particleList[i].creationTime > m_lifespan)
			{
				m_particleList[i].quickFade = 1;

				continue;
			}
			else
			{
				int deltaX = m_particleList[i].x - m_position.x;
				int deltaY = m_particleList[i].y - m_position.y;

				if((deltaX) * (deltaX) + (deltaY) * (deltaY) > m_maximumDistance * m_maximumDistance)
				{
					m_particleList[i].quickFade = 1;

					continue;
				}
			}
		}
#endif

		m_lastPosition = m_position;

		parentalUpdate();
	}

	static int CompareParticleDepths(const void *p0, const void *p1)
	{
		Particle *par0 = (Particle *)p0;
		Particle *par1 = (Particle *)p1;

		//return par0->zView - par1->zView;
		return 0;
	}

	void ParticleSystem::render()
	{
		//bool antiAliasingEnabled = graphics.isAntiAliasingEnabled();
		//graphics.disableAntiAliasing();

		graphics.setTexture(m_texture);

		float frameTime = EAGLE_DEFAULT_FRAME_TIME;

		int counter = 0;

		for(int i = 0; i < m_particleCount; ++i)
		{
			if(!m_particleList[i].enabled)
			{
				continue;
			}

			/*graphics.drawLine(m_particleList[i].x - m_particleList[i].m_velocityX * frameTime, m_particleList[i].y - m_particleList[i].m_velocityY * frameTime, m_particleList[i].x + m_particleList[i].m_velocityX * frameTime, m_particleList[i].y + m_particleList[i].m_velocityY * frameTime, scale, m_particleList[i].color, m_particleList[i].z);
			
			m_vertexList[counter * 4].x = m_particleList[i].x;
			m_vertexList[counter * 4].y = m_particleList[i].y;
			m_vertexList[counter * 4].z = m_particleList[i].z;
			m_vertexList[counter * 4].color = m_particleList[i].color;
			m_vertexList[counter * 4].tu = 0;
			m_vertexList[counter * 4].tv = 0;

			m_vertexList[counter * 4 + 1].x = m_particleList[i].x + scale;
			m_vertexList[counter * 4 + 1].y = m_particleList[i].y;
			m_vertexList[counter * 4 + 1].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 1].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 1].tu = 1;
			m_vertexList[counter * 4 + 1].tv = 0;

			m_vertexList[counter * 4 + 2].x = m_particleList[i].x;
			m_vertexList[counter * 4 + 2].y = m_particleList[i].y + scale;
			m_vertexList[counter * 4 + 2].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 2].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 2].tu = 0;
			m_vertexList[counter * 4 + 2].tv = 1;

			m_vertexList[counter * 4 + 3].x = m_particleList[i].x + scale;
			m_vertexList[counter * 4 + 3].y = m_particleList[i].y + scale;
			m_vertexList[counter * 4 + 3].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 3].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 3].tu = 1;
			m_vertexList[counter * 4 + 3].tv = 1;*/

			Vector2 velocityVector = Vector2(m_particleList[i].velocityX, m_particleList[i].velocityY);
			Vector2 normalVector = velocityVector.normal() * m_scale / 2;
			velocityVector *= frameTime * m_scale / 2 * m_stretchFactor;

			if(velocityVector.x * velocityVector.x + velocityVector.y * velocityVector.y < m_scale * m_scale / 4)
			{
				velocityVector = normalVector;
			}

			normalVector.y *= -1;

			m_vertexList[counter * 4].x = m_particleList[i].x - velocityVector.x - normalVector.y;
			m_vertexList[counter * 4].y = m_particleList[i].y - velocityVector.y - normalVector.x;
			m_vertexList[counter * 4].z = m_particleList[i].z;
			m_vertexList[counter * 4].color = m_particleList[i].color;
			m_vertexList[counter * 4].tu = 0;
			m_vertexList[counter * 4].tv = 0;
			m_vertexList[counter * 4 + 1].x = m_particleList[i].x - velocityVector.x + normalVector.y;
			m_vertexList[counter * 4 + 1].y = m_particleList[i].y - velocityVector.y + normalVector.x;
			m_vertexList[counter * 4 + 1].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 1].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 1].tu = 0;
			m_vertexList[counter * 4 + 1].tv = 1;
			m_vertexList[counter * 4 + 2].x = m_particleList[i].x + velocityVector.x - normalVector.y;
			m_vertexList[counter * 4 + 2].y = m_particleList[i].y + velocityVector.y - normalVector.x;
			m_vertexList[counter * 4 + 2].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 2].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 2].tu = 1;
			m_vertexList[counter * 4 + 2].tv = 0;
			m_vertexList[counter * 4 + 3].x = m_particleList[i].x + velocityVector.x + normalVector.y;
			m_vertexList[counter * 4 + 3].y = m_particleList[i].y + velocityVector.y + normalVector.x;
			m_vertexList[counter * 4 + 3].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 3].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 3].tu = 1;
			m_vertexList[counter * 4 + 3].tv = 1;

			counter++;
		}

		graphics.drawPrimitiveList(m_vertexList, 2, PrimitiveType_TriangleStrip, counter);
		//graphics.setAntiAliasingState(antiAliasingEnabled);

		graphics.setTexture(0);
	}

	void ParticleSystem::renderTransformed()
	{
		graphics.setTexture(m_texture);

		float frameTime = EAGLE_DEFAULT_FRAME_TIME;

		int counter = 0;

		for(int i = 0; i < m_particleCount; ++i)
		{
			if(!m_particleList[i].enabled)
			{
				continue;
			}

			/*m_vertexList[counter * 4].x = m_particleList[i].x;
			m_vertexList[counter * 4].y = m_particleList[i].y;
			m_vertexList[counter * 4].z = m_particleList[i].z;
			m_vertexList[counter * 4].color = m_particleList[i].color;
			m_vertexList[counter * 4].tu = 0;
			m_vertexList[counter * 4].tv = 0;

			m_vertexList[counter * 4 + 1].x = m_particleList[i].x + scale;
			m_vertexList[counter * 4 + 1].y = m_particleList[i].y;
			m_vertexList[counter * 4 + 1].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 1].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 1].tu = 1;
			m_vertexList[counter * 4 + 1].tv = 0;

			m_vertexList[counter * 4 + 2].x = m_particleList[i].x;
			m_vertexList[counter * 4 + 2].y = m_particleList[i].y + scale;
			m_vertexList[counter * 4 + 2].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 2].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 2].tu = 0;
			m_vertexList[counter * 4 + 2].tv = 1;

			m_vertexList[counter * 4 + 3].x = m_particleList[i].x + scale;
			m_vertexList[counter * 4 + 3].y = m_particleList[i].y + scale;
			m_vertexList[counter * 4 + 3].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 3].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 3].tu = 1;
			m_vertexList[counter * 4 + 3].tv = 1;*/

			Vector2 velocityVector = Vector2(m_particleList[i].velocityX, m_particleList[i].velocityY);
			Vector2 normalVector = velocityVector.normal() * m_scale / 2;
			velocityVector *= frameTime * m_scale / 2 * m_stretchFactor;

			if(velocityVector.x * velocityVector.x + velocityVector.y * velocityVector.y < m_scale * m_scale / 4)
			{
				velocityVector = normalVector;
			}

			normalVector.y *= -1;

			m_vertexList[counter * 4].x = m_particleList[i].x - velocityVector.x - normalVector.y;
			m_vertexList[counter * 4].y = m_particleList[i].y - velocityVector.y - normalVector.x;
			m_vertexList[counter * 4].z = m_particleList[i].z;
			m_vertexList[counter * 4].color = m_particleList[i].color;
			m_vertexList[counter * 4].tu = 0;
			m_vertexList[counter * 4].tv = 0;
			m_vertexList[counter * 4 + 1].x = m_particleList[i].x - velocityVector.x + normalVector.y;
			m_vertexList[counter * 4 + 1].y = m_particleList[i].y - velocityVector.y + normalVector.x;
			m_vertexList[counter * 4 + 1].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 1].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 1].tu = 0;
			m_vertexList[counter * 4 + 1].tv = 1;
			m_vertexList[counter * 4 + 2].x = m_particleList[i].x + velocityVector.x - normalVector.y;
			m_vertexList[counter * 4 + 2].y = m_particleList[i].y + velocityVector.y - normalVector.x;
			m_vertexList[counter * 4 + 2].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 2].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 2].tu = 1;
			m_vertexList[counter * 4 + 2].tv = 0;
			m_vertexList[counter * 4 + 3].x = m_particleList[i].x + velocityVector.x + normalVector.y;
			m_vertexList[counter * 4 + 3].y = m_particleList[i].y + velocityVector.y + normalVector.x;
			m_vertexList[counter * 4 + 3].z = m_particleList[i].z;
			m_vertexList[counter * 4 + 3].color = m_particleList[i].color;
			m_vertexList[counter * 4 + 3].tu = 1;
			m_vertexList[counter * 4 + 3].tv = 1;

			counter++;
		}

		graphics.drawTransformedPrimitiveList(m_vertexList, 2, PrimitiveType_TriangleStrip, counter);
		//graphics.setAntiAliasingState(antiAliasingEnabled);

		graphics.setTexture(0);
	}

	void ParticleSystem::setColorRange(float r0, float g0, float b0, float a0, float r1, float g1, float b1, float a1)
	{
		m_minimumRed = r0;
		m_maximumRed = r1;
		m_minimumGreen = g0; 
		m_maximumGreen = g1;
		m_minimumBlue = b0;
		m_maximumBlue = b1;
		m_minimumAlpha = a0;
		m_maximumAlpha = a1;
	}

	void ParticleSystem::add()
	{
		/*for(int i = 0; i < m_particleCount; ++i)
		{
			if(!m_particleList[i].enabled)
			{
				resetParticle(i);
				return;
			}
		}*/

		if(m_inactiveParticleListTopIndex > 0)
		{
			resetParticle(m_inactiveParticleList[m_inactiveParticleListTopIndex]);

			m_inactiveParticleListTopIndex--;

			return;
		}

		if(m_particleCount < m_maxParticleCount)
		{
			resetParticle(m_particleCount);
			m_particleCount++;
		}
		else
		{
			m_particleList[rand() % m_particleCount].quickFade = 1;
		}
	}

	void ParticleSystem::resetParticle(int index)
	{
		if(index < 0)
		{
			return;
		}

		m_particleList[index].enabled = 1;
		float variation = (rand() % (int)((m_spread + 0.01) * 100.0)) / 100.0;
		//float d = ((float)(math.random(0, 2.0 * pi * 1000.0))) / 1000.0;
		//float speedVar = 1 + ((float)math.random(-1000 * m_velocityRandomFactor, 1000 * m_velocityRandomFactor)) / 1000.0;
		float vx = cos(m_direction - m_spread / 2 + variation); // * speedVar;
		float vy = sin(m_direction - m_spread / 2 + variation); // * speedVar;
		//float vz = sin(d) * speedVar;

		float randomPointDistance = 0;

		if(m_moveLength)
		{
			randomPointDistance = math.randomFloat(m_moveLength);
		}

		Vector3 moveVector = m_moveNormal * randomPointDistance;

		int dist = math.random(-(m_sourceLineLength / 2), m_sourceLineLength / 2);
		m_particleList[index].x = m_position.x + cos(m_sourceLineDirection + pi / 2) * dist + moveVector.x - m_maximumParticleCreationDistance + rand() % (m_maximumParticleCreationDistance * 2);
		m_particleList[index].y = m_position.y + sin(m_sourceLineDirection + pi / 2) * dist + moveVector.y - m_maximumParticleCreationDistance + rand() % (m_maximumParticleCreationDistance * 2);
		m_particleList[index].z = -m_position.z + moveVector.z;
		//m_particleList[index].depth = depth + math.random(depthSpread * 2) - depthSpread;
		//m_particleList[index].dist = math.abs(dist);

		//m_particleList[index].m_velocityX = vx * m_velocity * (rand() % (int)(m_velocityRandomFactor + 1.000001)) / 10;
		//m_particleList[index].m_velocityY = vy * m_velocity * (rand() % (int)(m_velocityRandomFactor + 1.000001)) / 10;
		m_particleList[index].velocityX = vx * m_velocity + m_initialVelocity.x;
		m_particleList[index].velocityY = vy * m_velocity + m_initialVelocity.y;
		//m_particleList[index].speedZ = vz * m_velocity * 0;

		float r = math.randomFloat(m_minimumRed, m_maximumRed);
		float g = math.randomFloat(m_minimumGreen, m_maximumGreen);
		float b = math.randomFloat(m_minimumBlue, m_maximumBlue);
		float a = math.randomFloat(m_minimumAlpha, m_maximumAlpha);
		m_particleList[index].color.r = r;
		m_particleList[index].color.g = g;
		m_particleList[index].color.b = b;
		m_particleList[index].color.a = a;

		//float rfl = (rand() % (int)(lifeSpanRandomFactor + 1.000001)) - lifeSpanRandomFactor / 2;

		m_particleList[index].quickFade = 0;

		m_particleList[index].creationTime = eagle.getTimer()->getPassedTimeSeconds();

		m_particleList[index].accelerationX = m_acceleration.x;
		m_particleList[index].accelerationY = m_acceleration.y;
		//m_particleList[index].size = 0.1;
		//psize[index] = ((rand()) % 5 + 1) / 2;
	}

	/*void ParticleSystem::initializeParticles(unsigned int steps)
	{
		for(int i = 0; i < steps; ++i)
		{
			Update();
		}
	}*/

	int ParticleSystem::getMaxParticleCount()
	{
		return m_maxParticleCount;
	}

	void ParticleSystem::setMaxParticleCount(int value)
	{
		m_maxParticleCount = value;

		if(m_particleCount > value)
		{
			m_particleCount = value;
		}

		delete[](m_particleList);
		m_particleList = new Particle[value];

		delete(m_vertexList);
		m_vertexList = new Vertex[value * 4];

		delete[](m_inactiveParticleList);
		m_inactiveParticleList = new int[value];

		m_inactiveParticleListTopIndex = 0;
	}

	float ParticleSystem::getStretchFactor()
	{
		return m_stretchFactor;
	}

	void ParticleSystem::setStretchFactor(float value)
	{
		m_stretchFactor = value;
	}

	Texture *ParticleSystem::getTexture()
	{
		return m_texture;
	}

	Particle *ParticleSystem::getParticle(int index)
	{
		return &m_particleList[index];
	}

	float ParticleSystem::getSourceLineDirection()
	{
		return m_sourceLineDirection;
	}

	void ParticleSystem::setSourceLineDirection(float value)
	{
		m_sourceLineDirection = value;
	}

	float ParticleSystem::getDirection()
	{
		return m_direction;
	}

	void ParticleSystem::setDirection(float value)
	{
		m_direction = value;
	}

	int ParticleSystem::getLifespan()
	{
		return m_lifespan;
	}

	void ParticleSystem::setLifespan(float seconds)
	{
		m_lifespan = seconds;
	}

	int ParticleSystem::getSourceLineLength()
	{
		return m_sourceLineLength;
	}

	void ParticleSystem::setSourceLineLength(int m_maximumDistance)
	{
		m_sourceLineLength = m_maximumDistance;
	}

	float ParticleSystem::getMinimumAlpha()
	{
		return m_minimumAlpha;
	}

	void ParticleSystem::setMinimumAlpha(float value)
	{
		m_minimumAlpha = value;
	}

	float ParticleSystem::getMaximumAlpha()
	{
		return m_maximumAlpha;
	}

	void ParticleSystem::setMaximumAlpha(float value)
	{
		m_maximumAlpha = value;
	}

	float ParticleSystem::getMinimumRed()
	{
		return m_minimumRed;
	}

	void ParticleSystem::setMinimumRed(float value)
	{
		m_minimumRed = value;
	}

	float ParticleSystem::getMaximumRed()
	{
		return m_maximumRed;
	}

	void ParticleSystem::setMaximumRed(float value)
	{
		m_maximumRed = value;
	}

	float ParticleSystem::getMinimumGreen()
	{
		return m_minimumGreen;
	}

	void ParticleSystem::setMinimumGreen(float value)
	{
		m_minimumGreen = value;
	}

	float ParticleSystem::getMaximumGreen()
	{
		return m_maximumGreen;
	}

	void ParticleSystem::setMaximumGreen(float value)
	{
		m_maximumGreen = value;
	}

	float ParticleSystem::getMinimumBlue()
	{
		return m_minimumBlue;
	}

	void ParticleSystem::setMinimumBlue(float value)
	{
		m_minimumBlue = value;
	}

	float ParticleSystem::getMaximumBlue()
	{
		return m_maximumBlue;
	}

	void ParticleSystem::setMaximumBlue(float value)
	{
		m_maximumBlue = value;
	}

	float ParticleSystem::getSpread()
	{
		return m_spread;
	}

	void ParticleSystem::setSpread(float value)
	{
		m_spread = value;
	}

	float ParticleSystem::getMaximumDistance()
	{
		return m_maximumDistance;
	}

	void ParticleSystem::setMaximumDistance(float value)
	{
		m_maximumDistance = value;
	}

	float ParticleSystem::getVelocity()
	{
		return m_velocity;
	}

	void ParticleSystem::setVelocity(float value)
	{
		m_velocity = value;
	}

	Vector2 ParticleSystem::getAcceleration()
	{
		return m_acceleration;
	}

	void ParticleSystem::setAcceleration(Vector2 value)
	{
		m_acceleration.set(value);
	}

	Vector2 ParticleSystem::getInitialVelocity()
	{
		return m_initialVelocity;
	}

	void ParticleSystem::setInitialVelocity(Vector2 value)
	{
		m_initialVelocity = value;
	}

	float ParticleSystem::getScale()
	{
		return m_scale;
	}

	void ParticleSystem::setScale(float value)
	{
		m_scale = value;
	}

	int ParticleSystem::getAddDelay()
	{
		return m_addDelay;
	}

	void ParticleSystem::setAddDelay(float value)
	{
		m_addDelay = value;
	}

	int ParticleSystem::getParticleCount()
	{
		return m_particleCount;
	}

	void ParticleSystem::setParticleCount(int value)
	{
		m_particleCount = value;
	}

	int ParticleSystem::getAliveParticleCount()
	{
		return aliveParticleCount;
	}

	float ParticleSystem::getNoise()
	{
		return m_noise;
	}

	void ParticleSystem::setNoise(float value)
	{
		m_noise = value;
	}

	int ParticleSystem::getParticlesPerAdd()
	{
		return m_particlesPerAdd;
	}

	void ParticleSystem::setParticlesPerAdd(int value)
	{
		m_particlesPerAdd = value;
	}

	int ParticleSystem::getMaximumParticleCreationDistance()
	{
		return m_maximumParticleCreationDistance;
	}

	void ParticleSystem::setMaximumParticleCreationDistance(int value)
	{
		m_maximumParticleCreationDistance = value;
	}

	float ParticleSystem::getAlphaDecrement()
	{
		return m_alphaDecrement;
	}

	void ParticleSystem::setAlphaDecrement(float value)
	{
		m_alphaDecrement = value;
	}

	float ParticleSystem::getRemovalAlphaDecrement()
	{
		return m_removalAlphaDecrement;
	}

	void ParticleSystem::setRemovalAlphaDecrement(float value)
	{
		m_removalAlphaDecrement = value;
	}

	float ParticleSystem::getFrictionFactor()
	{
		return m_frictionFactor;
	}

	void ParticleSystem::setFrictionFactor(float value)
	{
		m_frictionFactor = value;
	}

#ifndef PLATFORM_WP8
	void ParallelParticleSystemUpdate::operator() (const tbb::blocked_range<int>& r) const
	{
		float currentTime = eagle.getTimer()->getPassedTimeSeconds();

		float frameTime = *m_particleEmitter->m_frameTime;
		float frictionEffect = 1.0f - (1.0f - m_particleEmitter->m_frictionFactor) * (frameTime);
		float aliveParticleAlphaDecrement = m_particleEmitter->m_alphaDecrement * (frameTime);
		float deadParticleAlphaDecrement = (m_particleEmitter->m_alphaDecrement + m_particleEmitter->m_removalAlphaDecrement) * (frameTime);

		for(int i = r.begin(); i != r.end(); ++i)
		{
			if(!m_particleList[i].enabled)
			{
				continue;
			}

			m_particleEmitter->aliveParticleCount++;

			//float lv = sqrt(m_particleList[i].m_velocityX * m_particleList[i].m_velocityX + m_particleList[i].m_velocityY * m_particleList[i].m_velocityY);
			/*float theta = atan2(m_particleList[i].m_velocityY, m_particleList[i].m_velocityX) + ((float)(math.random(0, 2) - 1)) * particleEmitter->m_noise;
			m_particleList[i].m_velocityX = cos(theta) * lv;
			m_particleList[i].m_velocityY = sin(theta) * lv;*/

			m_particleList[i].velocityX += ((float)(math.random(0, 2) - 1)) * m_particleEmitter->m_noise;
			m_particleList[i].velocityY += ((float)(math.random(0, 2) - 1)) * m_particleEmitter->m_noise;

			m_particleList[i].x += m_particleList[i].velocityX * (frameTime);
			m_particleList[i].y += m_particleList[i].velocityY * (frameTime);

			m_particleList[i].velocityX *= frictionEffect;
			m_particleList[i].velocityY *= frictionEffect;
			//m_particleList[i].z += m_particleList[i].speedZ;

			m_particleList[i].velocityX += m_particleList[i].accelerationX * (frameTime);
			m_particleList[i].velocityY += m_particleList[i].accelerationY * (frameTime);

			//colors[i].a = math.abs(colors[i].a);

			if(m_particleList[i].quickFade)
			{
				m_particleList[i].color.a -= deadParticleAlphaDecrement;
			}
			else
			{
				m_particleList[i].color.a -= aliveParticleAlphaDecrement;
			}

			//if((m_particleList[i].quickFade && m_particleList[i].color.a < 10) || m_particleList[i].color.a < 10)

			if(m_particleList[i].color.a < EAGLE_PARTICLE_SYSTEM_MINIMUM_PARTICLE_ALPHA)
			{
				m_particleList[i].enabled = 0;
				m_particleList[i].color.a = 0;

				m_particleEmitter->m_inactiveParticleListTopIndex++;

				m_particleEmitter->m_inactiveParticleList[m_particleEmitter->m_inactiveParticleListTopIndex] = i;

				continue;
			}
			else if(currentTime - m_particleList[i].creationTime > m_particleEmitter->m_lifespan)
			{
				m_particleList[i].quickFade = 1;

				continue;
			}
			else
			{
				int deltaX = m_particleList[i].x - m_particleEmitter->m_position.x;
				int deltaY = m_particleList[i].y - m_particleEmitter->m_position.y;

				if((deltaX) * (deltaX) + (deltaY) * (deltaY) > m_particleEmitter->m_maximumDistance * m_particleEmitter->m_maximumDistance)
				{
					m_particleList[i].quickFade = 1;

					continue;
				}
			}
		}
	}
#endif
}