#include "DynamicsSystem.h"

#include "Common.h"
#include "RenderingSystem.h"

#define EAGLE_DEFAULT_FRAME_RATE 60

static ProjectEagle::Timer timer;

namespace ProjectEagle
{
	PositionComponent::PositionComponent()
	{
		componentTypeID = POSITION_COMPONENT_TYPE_ID;

		position.set(0, 0, 0);
	}

	MovementComponent::MovementComponent()
	{
		componentTypeID = MOVEMENT_COMPONENT_TYPE_ID;

		velocity.set(0, 0, 0);
		acceleration.set(0, 0, 0);
	}

	b2World *physicsWorld;

	PhysicsComponent::PhysicsComponent()
	{
		componentTypeID = PHYSICS_COMPONENT_TYPE_ID;

		isDynamic = 1;

		width = 10;
		height = 10;

		density = 1;
		restitution = 0.5;
		friction = 0.5;
	}

	PhysicsComponent::~PhysicsComponent()
	{
		//if(body) physicsWorld->DestroyBody(body);
	}

	DynamicsSystem::DynamicsSystem()
	{
	}

	void DynamicsSystem::update(float frameTime)
	{
#ifndef PLATFORM_WP8
		parallel_for(tbb::blocked_range<int>(0, entityList->size(), 1000), ParallelDynamicsSystemUpdate(this, frameTime));
#else
		GameEntityList::iterator listIterator = entityList->begin();

		for(; listIterator != entityList->end(); ++listIterator)
		{
			PositionComponent *position = (PositionComponent *)getComponent(&listIterator->second, POSITION_COMPONENT_TYPE_ID);
			MovementComponent *movement = (MovementComponent *)getComponent(&listIterator->second, MOVEMENT_COMPONENT_TYPE_ID);

			if(position && movement)
			{
				movement->velocity += movement->acceleration * frameTime;
				position->position += movement->velocity * frameTime;
			}
		}
#endif
	}

#ifndef PLATFORM_WP8
	void ParallelDynamicsSystemUpdate::operator() (const tbb::blocked_range<int>& r) const
	{
		DWORD currentTime = timer.getPassedTimeMilliseconds();

		//for(int i = r.begin(); i != r.end(); ++i)

		int counter = 0;

		GameEntityList::iterator listIterator = dynamicsSystem->entityList->begin();

		int rangeStart = r.begin();
		int rangeEnd = r.end();

		while(counter < rangeStart)
		{
			listIterator++;

			counter++;
		}

		for(; listIterator != dynamicsSystem->entityList->end(); ++listIterator)
		{
			PositionComponent *position = (PositionComponent *)dynamicsSystem->getComponent(&listIterator->second, POSITION_COMPONENT_TYPE_ID);
			MovementComponent *movement = (MovementComponent *)dynamicsSystem->getComponent(&listIterator->second, MOVEMENT_COMPONENT_TYPE_ID);

			if(position && movement)
			{
				movement->velocity += movement->acceleration * frameTime;
				position->position += movement->velocity * frameTime;
			}

			counter++;

			if(counter > rangeEnd - 1) break;
		}
	}
#endif

	PhysicsSystem::PhysicsSystem()
	{
		b2Vec2 gravityVector(0, 10);
		bool doSleep = 0;

		physicsWorld = new b2World(gravityVector);
		physicsWorld->SetAllowSleeping(doSleep);

		updateTimer.reset();
	}

	void PhysicsSystem::update(float frameTime)
	{
		//if(!updateTimer.stopWatch(1000.0 / EAGLE_DEFAULT_FRAME_RATE * 1.5)) return;

		updateTimer.reset();

#ifdef PLATFORM_WP8
		setGravity(Vector2(input.getInclinometerData().z * 10, input.getInclinometerData().y * 10));
#endif

		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			PositionComponent *position = (PositionComponent *)getComponent(&listIterator->second, POSITION_COMPONENT_TYPE_ID);
			PhysicsComponent *physics = (PhysicsComponent *)getComponent(&listIterator->second, PHYSICS_COMPONENT_TYPE_ID);
			WidthHeightComponent *dimensions = (WidthHeightComponent *)getComponent(&listIterator->second, WIDTHHEIGHT_COMPONENT_TYPE_ID);
			TransformationComponent *transformationComponent = (TransformationComponent *)getComponent(&listIterator->second, TRANSFORMATION_COMPONENT_TYPE_ID);

			if(physics)
			{
				if(position && transformationComponent)
				{
					//physics->body->SetTransform(b2Vec2(position->position.x / 100 + (physics->width / 100) / 2, position->position.y / 100 + (physics->height / 100) / 2), transformationComponent->rotation.z);
					physics->body->SetTransform(b2Vec2(position->position.x / 100 + (physics->width / 100) / 2, position->position.y / 100 + (physics->height / 100) / 2), transformationComponent->rotation.z);
				}
				else if(position)
				{
					physics->body->SetTransform(b2Vec2(position->position.x / 100 + (physics->width / 100) / 2, position->position.y / 100 + (physics->height / 100) / 2), physics->body->GetAngle());
				}
			}
		}

		//physicsWorld->Step(1.0 / EAGLE_DEFAULT_FRAME_RATE, 8, 3);
		physicsWorld->Step(1.0 / EAGLE_DEFAULT_FRAME_RATE, 8, 3);

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			PositionComponent *position = (PositionComponent *)getComponent(&listIterator->second, POSITION_COMPONENT_TYPE_ID);
			PhysicsComponent *physics = (PhysicsComponent *)getComponent(&listIterator->second, PHYSICS_COMPONENT_TYPE_ID);
			TransformationComponent *transformationComponent = (TransformationComponent *)getComponent(&listIterator->second, TRANSFORMATION_COMPONENT_TYPE_ID);

			if(physics && position)
			{
				b2Vec2 positionVector = physics->body->GetPosition();
				//position->position.set(positionVector.x * 100 - physics->width / 2, positionVector.y * 100 - physics->height / 2, position->position.z);
				position->position.set(positionVector.x * 100 - physics->width / 2, positionVector.y * 100 - physics->height / 2, position->position.z);

				if(transformationComponent) transformationComponent->rotation.z = physics->body->GetAngle();
			}
		}
	}

	void PhysicsSystem::onEntityAddComponent(int entityID, int componentTypeID)
	{
		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			if(listIterator->first != entityID) continue;

			if(componentTypeID == PHYSICS_COMPONENT_TYPE_ID)
			{
				PhysicsComponent *physicsComponent = (PhysicsComponent *)getComponent(&listIterator->second, PHYSICS_COMPONENT_TYPE_ID);

				if(physicsComponent)
				{
					b2BodyDef bodyDef;
					if(physicsComponent->isDynamic) bodyDef.type = b2_dynamicBody;
					bodyDef.position.Set(0, 0);

					physicsComponent->body = physicsWorld->CreateBody(&bodyDef);

					b2PolygonShape dynamicBox;
					dynamicBox.SetAsBox(physicsComponent->width / 100.0 / 2.0, physicsComponent->height / 100.0 / 2.0);
					//dynamicBox.m_radius = 0.01;

					b2FixtureDef fixtureDef;
					fixtureDef.shape = &dynamicBox;
					if(physicsComponent->isDynamic) fixtureDef.density = physicsComponent->density;
					else fixtureDef.density = 0;
					fixtureDef.friction = physicsComponent->friction;
					fixtureDef.restitution = physicsComponent->restitution;

					physicsComponent->body->CreateFixture(&fixtureDef);
				}
			}

			return;
		}
	}

	void PhysicsSystem::onEntityRemove(int entityID)
	{
		GameEntityList::iterator listIterator;

		for(listIterator = entityList->begin(); listIterator != entityList->end(); ++listIterator)
		{
			if(listIterator->first != entityID) continue;

			PhysicsComponent *physics = (PhysicsComponent *)getComponent(&listIterator->second, PHYSICS_COMPONENT_TYPE_ID);

			if(physics)
			{
				physicsWorld->DestroyBody(physics->body);
			}
		}
	}

	void PhysicsSystem::setGravity(Vector2 value)
	{
		b2Vec2 gravityVector(value.x, value.y);

		physicsWorld->SetGravity(gravityVector);
	}
};