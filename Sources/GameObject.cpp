#include "GameObject.h"

#include <cstdlib>

namespace ProjectEagle
{
	static void drawGameObjectTransformed(GameObject *object)
	{
		object->renderTransformed();
	}

	static void drawGameObject(GameObject *object)
	{
		object->render();
	}

	// GameObject

	GameObject::GameObject()
	{
		m_id = -1;
		m_parent = 0;
		m_visible = 1;
		m_active = 1;
		m_alive = 1;
		m_transformed = 0;
		m_lifeSpan = INT_MAX;
		m_lifeSpanTimer.reset();
		m_position.set(0, 0, 0);
		m_parentLastPosition.set(0, 0, 0);
		m_drawPointer = drawGameObject;
		m_objectType = GAMEOBJECT_DEFAULT;
	}

	GameObject::~GameObject()
	{
	}

	void GameObject::update()
	{
	}

	void GameObject::render()
	{
	}

	void GameObject::renderTransformed()
	{
	}

	void GameObject::transformationBasedDraw()
	{
		m_drawPointer(this);
	}

	void GameObject::setTransformed(bool value)
	{
		m_transformed = value; value ? m_drawPointer = drawGameObjectTransformed : m_drawPointer = drawGameObject;
	}

	void GameObject::resetLifespan(int milliseconds)
	{
		m_lifeSpan = milliseconds;
		m_lifeSpanTimer.reset();
	}

	void GameObject::parentalUpdate()
	{
		if(!m_parent)
		{
			return;
		}

		m_position += m_parent->getPosition() - m_parentLastPosition;
		m_parentLastPosition = m_parent->getPosition();
	}

	int GameObject::getType()
	{
		return m_objectType;
	}

	void GameObject::setID(int value)
	{
		m_id = value;
	}

	int GameObject::getID()
	{
		return m_id;
	}

	void GameObject::setParent(GameObject *parentObject)
	{
		m_parent = parentObject;
		m_parentLastPosition.set(m_parent->getPosition());
	}

	GameObject *GameObject::getParent()
	{
		return m_parent;
	}

	Vector3 GameObject::getPosition()
	{
		return m_position;
	}

	void GameObject::setPosition(Vector3 value)
	{
		m_position = value;
	}

	void GameObject::setPosition(float x, float y, float z)
	{
		m_position.set(x, y, z);
	}

	bool GameObject::isLifespanOver()
	{
		return m_lifeSpanTimer.stopWatch(m_lifeSpan);
	}

	int GameObject::getLifetime()
	{
		return m_lifeSpan;
	}

	void GameObject::setLifespan(int milliseconds)
	{
		m_lifeSpan = milliseconds;
	}

	void GameObject::setVisible(bool value)
	{
		m_visible = value;
	}

	bool GameObject::isVisible()
	{
		return m_visible;
	}

	void GameObject::setActive(bool value)
	{
		m_active = value;
	}

	bool GameObject::isActive()
	{
		return m_active;
	}

	void GameObject::setAlive(bool value)
	{
		m_alive = value;
	}

	bool GameObject::isAlive()
	{
		return m_alive;
	}

	bool GameObject::getTransformed()
	{
		return m_transformed;
	}

	// GameObjectManager

	GameObjectManagerClass::GameObjectManagerClass()
	{
		m_objectCount = 0;
		m_highestIndex = -1;
		m_deadCount = 0;
	}
	
	GameObjectManagerClass::~GameObjectManagerClass()
	{
		for(int i = 0; i < m_highestIndex + 1; ++i)
		{
			if(m_gameObjectList[i] != 0)
			{
				delete m_gameObjectList[i];
			}
		}
	}

	/*int GameObjectManagerClass::addObjectByValue(GameObject *object)
	{
		int returnValue = 0;

		if(m_deadCount)
		{
			//m_gameObjectList[*deadList[deadCount - 1]] = new GameObject(object);
			//memcpy(m_gameObjectList[*deadList[deadCount - 1]], &object, sizeof(*object));
			returnValue = *m_deadList[m_deadCount - 1];
			//deadCount--;

			switch(object->getType())
			{
			case GAMEOBJECT_DEFAULT:
				m_gameObjectList[*m_deadList[m_deadCount - 1]] = new GameObject(*object);
				break;
			case GAMEOBJECT_SPRITE:
				m_gameObjectList[*m_deadList[m_deadCount - 1]] = new Sprite(*((Sprite *)object));
				break;
			case GAMEOBJECT_PARTICLEEMITTER:
				//m_gameObjectList[highestIndex] = new ParticleSystem(*((ParticleSystem *)object));
				m_gameObjectList[*m_deadList[m_deadCount - 1]] = new ParticleSystem();
				ParticleSystem *particleSystem = (ParticleSystem *)m_gameObjectList[*m_deadList[m_deadCount - 1]];
				*particleSystem = *((ParticleSystem *)object);

				break;
			}

			m_deadCount--;
		}
		else
		{
			m_highestIndex++;
			//m_gameObjectList[highestIndex] = new GameObject(*object);
			//Sprite *spriteObject;
			//eagle.message(INT_TO_STRING(object->getType()));
			switch(object->getType())
			{
			case GAMEOBJECT_DEFAULT:
				m_gameObjectList[m_highestIndex] = new GameObject(*object);
				break;
			case GAMEOBJECT_SPRITE:
				m_gameObjectList[m_highestIndex] = new Sprite(*((Sprite *)object));
				break;
			case GAMEOBJECT_PARTICLEEMITTER:
				//m_gameObjectList[highestIndex] = new ParticleSystem(*((ParticleSystem *)object));
				m_gameObjectList[m_highestIndex] = new ParticleSystem();
				ParticleSystem *particleSystem = (ParticleSystem *)m_gameObjectList[m_highestIndex];
				*particleSystem = *((ParticleSystem *)object);

				break;
			}

			returnValue = m_highestIndex;
		}

		m_objectCount++;
		m_gameObjectList[returnValue]->setID(returnValue);

		return returnValue;
	}

	int GameObjectManagerClass::addObjectByPointer(GameObject *object)
	{
		int returnValue = 0;

		if(m_deadCount)
		{
			m_gameObjectList[*m_deadList[m_deadCount - 1]] = object;
			returnValue = *m_deadList[m_deadCount - 1];
			m_deadCount--;
		}
		else
		{
			m_gameObjectList[++m_highestIndex] = object;
			returnValue = m_highestIndex;
		}

		m_objectCount++;
		m_gameObjectList[returnValue]->setID(returnValue);

		return returnValue;
	}*/

	void GameObjectManagerClass::UpdateGameObjects()
	{
		//return;
		for(int i = 0; i < m_highestIndex + 1; ++i)
		{
			if(m_gameObjectList[i] == 0 || !m_gameObjectList[i]->m_alive)
			{
				continue;
			}

			m_gameObjectList[i]->update();
			//m_gameObjectList[i]->parentalUpdate();
		}
	}

	void GameObjectManagerClass::drawGameObjects()
	{
		for(int i = 0; i < m_highestIndex + 1; ++i)
		{
			if(m_gameObjectList[i] == 0 || !m_gameObjectList[i]->m_alive) continue;

			//m_gameObjectList[i]->TransformationBaseddraw();
			m_gameObjectList[i]->render();
			//Sprite *aa = (Sprite *)m_gameObjectList[i];
			//aa->draw();
			//eagle.message("!");
		}
	}

	void GameObjectManagerClass::RemoveDeadGameObjects()
	{
		//return;
		/*for(int i = 0; i < m_highestIndex + 1; ++i)
		{
			if(m_gameObjectList[i] == 0) continue;

			if(m_gameObjectList[i]->isLifespanOver())
			{
				m_gameObjectList[i]->setAlive(0);
			}

			if(!m_gameObjectList[i]->isAlive())
			{
				switch(m_gameObjectList[i]->getType())
				{
				case GAMEOBJECT_DEFAULT:
					{
						delete(m_gameObjectList[i]);

						break;
					}

				case GAMEOBJECT_SPRITE:
					{
						Sprite *sprite = (Sprite *)m_gameObjectList[i];
						delete(sprite);

						break;
					}
				case GAMEOBJECT_PARTICLEEMITTER:
					{
						ParticleSystem *particleSystem = (ParticleSystem *)m_gameObjectList[i];
						delete(particleSystem);

						break;
					}
				}

				m_gameObjectList[i] = 0;
				m_deadList[m_deadCount++] = new int(i);

				m_objectCount--;
			}
		}*/
	}

	GameObject *GameObjectManagerClass::getObjectByID(int ID)
	{
		return m_gameObjectList[ID];
	}
};