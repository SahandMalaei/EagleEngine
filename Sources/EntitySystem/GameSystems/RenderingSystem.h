#ifndef _RENDERINGSYSTEM_H
#define _RENDERINGSYSTEM_H 1

#include "../../Graphics.h"
#include "../../ParticleSystem.h"

#include "../EntitySystem.h"

#include "../../Helpers.h"
#include "../../Timer.h"

#define RENDER_COMPONENT_TYPE_ID 20
#define TRANSFORMATION_COMPONENT_TYPE_ID 21
#define COLOR_COMPONENT_TYPE_ID 22
#define TEXTURE_COMPONENT_TYPE_ID 23
#define SPRITESHEETANIMATION_COMPONENT_TYPE_ID 24
#define PARTICLEEMITTER_COMPONENT_TYPE_ID 25

namespace ProjectEagle
{
	class RenderComponent : public GameComponent
	{
	public:
		RenderComponent();
	};

	class TransformationComponent : public GameComponent
	{
	public:
		TransformationComponent();

		Vector2 scale;
		Vector3 rotation;
		Vector3 center;
	};

	class ColorComponent : public GameComponent
	{
	public:
		ColorComponent();

		ColorValue color;
	};

	class TextureComponent : public GameComponent
	{
	public:
		TextureComponent();

		Texture *texture;

		Vector2 sourcePosition;
		Vector2 sourceDimensions;

		void loadTexture(std::string addressOnDisk);
	};

	class SpriteSheetAnimationComponent : public GameComponent
	{
	public:
		SpriteSheetAnimationComponent();

		short currentFrame;
		short currentRow;
		short frameCount;
		short animationDirection;
		short animationColumnCount;
		float frameInterval;

		float passedTime;
	};

	class ParticleEmitterComponent : public GameComponent
	{
	public:
		ParticleEmitterComponent();
		~ParticleEmitterComponent();

		void releaseData();

		Timer addTimer;
		float particleLifespan;
		float sourceLineLength;

		float direction;
		float sourceLineDirection;
		float maximumDistance;
		int maxParticleCount;

		float minimumAlpha, maximumAlpha;
		float minimumRed, minimumGreen, minimumBlue, maximumRed, maximumGreen, maximumBlue;
		float spread;
		float velocity;
		float scale;
		float addDelay;
		float alphaDecrement;
		float removalAlphaDecrement;
		int particleCount, aliveParticleCount;

		Vector2 acceleration;

		float noise;

		float frictionFactor;

		int particlesPerAdd;
		int maximumParticleCreationDistance;

		int *inactiveParticleList;
		int inactiveParticleListTopIndex;

		Vector3 position;
		Vector3 lastPosition, moveAmount, moveNormal;

		int moveLength;

		Particle *particleList;
		ParticleVertex *particleVertexList;

		void operator = (const ParticleEmitterComponent &p);
		void reset();

		void setMax(int value);

		void resetParticle(int index, Vector3 offsetPosition);
		void add(Vector3 offsetPosition);

		void setColorRange(int r0, int g0, int b0, int a0, int r1, int g1, int b1, int a1);

		Texture *texture;

		void loadTexture(std::string addressOnDisk);
	};

#ifndef PLATFORM_WP8
	class ParallelParticleEmitterUpdate
	{
		ParticleEmitterComponent *particleEmitter;
		int particleCount;
		Particle *particleList;

		float frameTime;

	public: 
		void operator() (const tbb::blocked_range<int>& r) const;

		ParallelParticleEmitterUpdate(ParticleEmitterComponent *targetParticleSystem, int numberOfParticles, Particle *particleList, float frameTime) : particleEmitter(targetParticleSystem), particleCount(numberOfParticles), particleList(particleList), frameTime(frameTime){}
	};
#endif

	class AnimationSystem : public GameSystem
	{
	public:
		AnimationSystem();

		virtual void update(float frameTime);
	};

	class RenderingSystem : public GameSystem	
	{
	private:
		virtual void onEntityRemove(int entityID);

	public:
		RenderingSystem();

		virtual void update(float frameTime);
		virtual void render();
	};
};

#endif