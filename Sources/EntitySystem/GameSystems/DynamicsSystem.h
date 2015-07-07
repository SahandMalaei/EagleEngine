#ifndef _DYNAMICSSYSTEM_H
#define _DYNAMICSSYSTEM_H 1

#include "../../Eagle.h"

#define POSITION_COMPONENT_TYPE_ID 10
#define MOVEMENT_COMPONENT_TYPE_ID 11
#define PHYSICS_COMPONENT_TYPE_ID 12

namespace ProjectEagle
{
	class PositionComponent : public GameComponent
	{
	public:
		PositionComponent();

		Vector3 position;
	};

	class MovementComponent : public GameComponent
	{
	public:
		MovementComponent();

		Vector3 velocity;
		Vector3 acceleration;
	};

	class PhysicsComponent : public GameComponent
	{
	public:
		PhysicsComponent();
		~PhysicsComponent();

		bool isDynamic;

		float width;
		float height;

		float density;
		float restitution;
		float friction;

		b2Body *body;
	};

	class DynamicsSystem : public GameSystem
	{
		friend class ParallelDynamicsSystemUpdate;

	public:
		DynamicsSystem();

		virtual void update(float frameTime);
	};

#ifndef PLATFORM_WP8
	class ParallelDynamicsSystemUpdate
	{
		DynamicsSystem *dynamicsSystem;
		float frameTime;

	public: 
		void operator() (const tbb::blocked_range<int>& r) const;

		ParallelDynamicsSystemUpdate(DynamicsSystem *targetDynamicsSystem, float frameTime) : dynamicsSystem(targetDynamicsSystem), frameTime(frameTime){}
	};
#endif

	class PhysicsSystem : public GameSystem
	{
	private:
		virtual void onEntityAddComponent(int entityID, int componentTypeID);
		virtual void onEntityRemove(int entityID);

		Timer updateTimer;

	public:
		PhysicsSystem();

		virtual void update(float frameTime);

		void setGravity(Vector2 value);
	};
};

#endif