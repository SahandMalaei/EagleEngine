#ifndef _PARTICLEEMITTER_H
#define _PARTICLEEMITTER_H 1

#ifndef PLATFORM_WP8
	#include <WinSock2.h>
	#include <Windows.h>

	#include <Include/tbb/tbb.h>
#endif

#include "Graphics.h"
#include "GameObject.h"

using namespace std;

#define EAGLE_PARTICLE_SYSTEM_MINIMUM_PARTICLE_ALPHA 0.01

namespace ProjectEagle
{
	struct Particle
	{
		float x, y, z, velocityX, velocityY, size;
		float accelerationX, accelerationY;
		ColorValue color;
		float creationTime;
		bool quickFade;
		bool enabled;
	};

	class ParticleSystem : public GameObject
	{
		friend class ParallelParticleSystemUpdate;

	private:
		Particle *m_particleList;
		Vertex *m_vertexList;

		Texture *m_texture;

		Timer m_addTimer;
		float m_lifespan;
		float m_sourceLineLength;
		float m_direction;
		float m_sourceLineDirection;
		float m_maximumDistance;
		int m_maxParticleCount;
		float m_minimumAlpha, m_maximumAlpha;
		float m_minimumRed, m_minimumGreen, m_minimumBlue, m_maximumRed, m_maximumGreen, m_maximumBlue;
		float m_spread;
		float m_velocity;
		float m_scale;
		float m_addDelay;
		float m_alphaDecrement;
		float m_removalAlphaDecrement;
		int m_particleCount, aliveParticleCount;
		Vector2 m_acceleration;
		Vector2 m_initialVelocity;
		float m_noise;
		float m_frictionFactor;
		int m_particlesPerAdd;
		int m_maximumParticleCreationDistance;

		int *m_inactiveParticleList;
		int m_inactiveParticleListTopIndex;

		Vector3 m_lastPosition, m_moveAmount, m_moveNormal;
		float m_moveLength;

		float m_stretchFactor;

	public:
		ParticleSystem();
		virtual ~ParticleSystem();

		void operator = (const ParticleSystem &p);
		void reset();

		bool loadImage(char *fileName);
		Texture *getTexture();

		Particle *getParticle(int index);

		float getSourceLineDirection();
		void setSourceLineDirection(float value);
		
		float getDirection();
		void setDirection(float value);

		int getLifespan();
		void setLifespan(float seconds);

		int getSourceLineLength();
		void setSourceLineLength(int maximumDistance);

		int getMaxParticleCount();
		void setMaxParticleCount(int value);

		float getMinimumAlpha();
		void setMinimumAlpha(float value);

		float getMaximumAlpha();
		void setMaximumAlpha(float value);

		void setColorRange(float r0, float g0, float b0, float a0, float r1, float g1, float b1, float a1);

		float getMinimumRed();
		void setMinimumRed(float value);

		float getMaximumRed();
		void setMaximumRed(float value);

		float getMinimumGreen();
		void setMinimumGreen(float value);
		
		float getMaximumGreen();
		void setMaximumGreen(float value);

		float getMinimumBlue();
		void setMinimumBlue(float value);

		float getMaximumBlue();
		void setMaximumBlue(float value);

		float getSpread();
		void setSpread(float value);
		
		float getMaximumDistance();
		void setMaximumDistance(float value);

		float getVelocity();
		void setVelocity(float value);

		Vector2 getAcceleration();
		void setAcceleration(Vector2 value);

		Vector2 getInitialVelocity();
		void setInitialVelocity(Vector2 value);
		
		float getScale();
		void setScale(float value);

		int getAddDelay();
		void setAddDelay(float seconds);

		int getParticleCount();
		void setParticleCount(int value);
		
		int getAliveParticleCount();

		float getNoise();
		void setNoise(float value);

		int getParticlesPerAdd();
		void setParticlesPerAdd(int value);

		int getMaximumParticleCreationDistance();
		void setMaximumParticleCreationDistance(int value);

		float getAlphaDecrement();
		void setAlphaDecrement(float value);

		float getRemovalAlphaDecrement();
		void setRemovalAlphaDecrement(float value);

		float getFrictionFactor();
		void setFrictionFactor(float value);

		void setStretchFactor(float value);
		float getStretchFactor();

		void render();
		void renderTransformed();
		void update();
		void add();
		void resetParticle(int index);
	};

#ifndef PLATFORM_WP8
	class ParallelParticleSystemUpdate
	{
		ParticleSystem *m_particleEmitter;
		int m_particleCount;
		Particle *m_particleList;

	public: 
		void operator() (const tbb::blocked_range<int>& r) const;

		ParallelParticleSystemUpdate(ParticleSystem *targetParticleSystem, int numberOfParticles, Particle *particleList) : m_particleEmitter(targetParticleSystem), m_particleCount(numberOfParticles), m_particleList(particleList){}
	};
#endif
};

#endif