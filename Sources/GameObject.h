#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H 1

#include "Helpers.h"
#include "Timer.h"

#define GAMEOBJECT_DEFAULT 0x00000000
#define GAMEOBJECT_SPRITE 0x00000001
#define GAMEOBJECT_PARTICLEEMITTER 0x00000002

namespace ProjectEagle
{

	class GameObjectManagerClass;

	class GameObject
	{
		friend class GameObjectManagerClass;

	private:
		int m_id;
		Timer m_lifeSpanTimer;
		bool m_transformed;
		Vector3 m_parentLastPosition;

		void (*m_drawPointer)(GameObject *object);

		GameObject *m_parent;

	protected:
		int m_lifeSpan;

		Vector3 m_position;
		int m_objectType;

		float *m_frameTime;

		bool m_visible;
		bool m_active;
		bool m_alive;

	public:
		GameObject();
		~GameObject();

		virtual void update();
		virtual void render();
		virtual void renderTransformed();
		void transformationBasedDraw();
		void parentalUpdate();

		int getType();

		void setID(int value);
		int getID();

		void setParent(GameObject *parentObject);
		GameObject *getParent();

		Vector3 getPosition();
		void setPosition(Vector3 value);
		void setPosition(float x, float y, float z);

		bool isLifespanOver();
		int getLifetime();
		void resetLifespan(int milliseconds);
		void setLifespan(int milliseconds);

		void setVisible(bool value);
		bool isVisible();

		void setActive(bool value);
		bool isActive();

		void setAlive(bool value);
		bool isAlive();

		void setTransformed(bool value);
		bool getTransformed();
	};

	class GameObjectManagerClass
	{
	private:
		GameObject *m_gameObjectList[1];
		int m_objectCount;
		int m_highestIndex;
		int *m_deadList[1];
		int m_deadCount;

	public:
		GameObjectManagerClass();
		~GameObjectManagerClass();

		int addObjectByValue(GameObject *object);
		int addObjectByPointer(GameObject *object);
		
		GameObject *getObjectByID(int ID);

		void UpdateGameObjects();
		void drawGameObjects();
		void RemoveDeadGameObjects();
	};
};

#endif