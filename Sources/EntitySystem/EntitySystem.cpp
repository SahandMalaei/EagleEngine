#include "EntitySystem.h"

#include "GameSystems/Common.h"
#include "GameSystems/DynamicsSystem.h"
#include "GameSystems/RenderingSystem.h"

namespace ProjectEagle
{
	class GameComponentVariableInformation
	{
	public:
		GameComponentVariableInformation();

		GameComponentVariableType type;

		void *pointer;
	};

	class GameComponentScriptInterface
	{
	public:
		GameComponentScriptInterface();

		int size;
		char *defaultValue;
		int totalRegisteredVariableSize;

		std::unordered_map<std::string, GameComponentVariableInformation> variableList;
	};

	GameComponentVariableInformation::GameComponentVariableInformation()
	{
		type = GameComponentVariable_Bool;

		pointer = 0;
	}

	GameComponentScriptInterface::GameComponentScriptInterface()
	{
		size = 0;
		defaultValue = 0;
		totalRegisteredVariableSize = 0;
	}

	GameEntityGroup::GameEntityGroup()
	{
	}

	GameEntityGroup::~GameEntityGroup()
	{
	}

	GameEntitySystem::GameEntitySystem()
	{
		highestEntityPresetIndex = 0;

		entityPresetCount = 0;

		GameScene newScene;
		sceneList.push_back(newScene);

		entityList = &sceneList[0].entityList;
		entityGroupList = &sceneList[0].entityGroupList;
		highestEntityIndex = &sceneList[0].highestEntityIndex;
		availableIDList = &sceneList[0].availableIDList;
		entityCount = &sceneList[0].entityCount;
		deadList = &sceneList[0].deadList;

		isInUpdateLoop = 0;

		entityList->reserve(1024);

		LifeSystem *lifeSystem = new LifeSystem();
		addSystem(lifeSystem);

		DynamicsSystem *dynamicsSystem = new DynamicsSystem();
		addSystem(dynamicsSystem);

		PhysicsSystem *physicsSystem = new PhysicsSystem();
		addSystem(physicsSystem);

		AnimationSystem *animationSystem = new AnimationSystem();
		addSystem(animationSystem);

		RenderingSystem *renderingSystem = new RenderingSystem();
		addSystem(renderingSystem);

		PositionComponent positionComponent;
		addScriptComponentInterface(&positionComponent, sizeof(positionComponent), "position");
		addScriptComponentVariable("position", GameComponentVariable_Float, "x");
		addScriptComponentVariable("position", GameComponentVariable_Float, "y");
		addScriptComponentVariable("position", GameComponentVariable_Float, "z");

		MovementComponent movementComponent;
		addScriptComponentInterface(&movementComponent, sizeof(movementComponent), "movement");
		addScriptComponentVariable("movement", GameComponentVariable_Float, "velocityX");
		addScriptComponentVariable("movement", GameComponentVariable_Float, "velocityY");
		addScriptComponentVariable("movement", GameComponentVariable_Float, "velocityZ");
		addScriptComponentVariable("movement", GameComponentVariable_Float, "accelerationX");
		addScriptComponentVariable("movement", GameComponentVariable_Float, "accelerationY");
		addScriptComponentVariable("movement", GameComponentVariable_Float, "accelerationZ");

		PhysicsComponent physicsComponent;
		addScriptComponentInterface(&physicsComponent, sizeof(physicsComponent), "physics");
		addScriptComponentVariable("physics", GameComponentVariable_Bool, "isDynamic");
		addScriptComponentVariable("physics", GameComponentVariable_Float, "width");
		addScriptComponentVariable("physics", GameComponentVariable_Float, "height");
		addScriptComponentVariable("physics", GameComponentVariable_Float, "density");
		addScriptComponentVariable("physics", GameComponentVariable_Float, "restitution");
		addScriptComponentVariable("physics", GameComponentVariable_Float, "friction");

		WidthHeightComponent widthHeightComponent;
		addScriptComponentInterface(&widthHeightComponent, sizeof(widthHeightComponent), "dimensions");
		addScriptComponentVariable("dimensions", GameComponentVariable_Float, "width");
		addScriptComponentVariable("dimensions", GameComponentVariable_Float, "height");

		LifeSpanComponent lifeSpanComponent;
		addScriptComponentInterface(&lifeSpanComponent, sizeof(lifeSpanComponent), "lifeSpan");
		addScriptComponentVariable("lifeSpan", GameComponentVariable_Float, "length");

		RenderComponent renderComponent;
		addScriptComponentInterface(&renderComponent, sizeof(renderComponent), "render");

		TransformationComponent transformationComponent;
		addScriptComponentInterface(&transformationComponent, sizeof(transformationComponent), "transformation");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "scaleX");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "scaleY");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "rotationX");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "rotationY");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "rotationZ");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "centerX");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "centerY");
		addScriptComponentVariable("transformation", GameComponentVariable_Float, "centerZ");

		ColorComponent colorComponent;
		addScriptComponentInterface(&colorComponent, sizeof(colorComponent), "color");
		addScriptComponentVariable("color", GameComponentVariable_Float, "r");
		addScriptComponentVariable("color", GameComponentVariable_Float, "g");
		addScriptComponentVariable("color", GameComponentVariable_Float, "b");
		addScriptComponentVariable("color", GameComponentVariable_Float, "a");

		TextureComponent textureComponent;
		addScriptComponentInterface(&textureComponent, sizeof(textureComponent), "texture");
		addScriptComponentVariable("texture", GameComponentVariable_Int, "texturePointer");
		addScriptComponentVariable("texture", GameComponentVariable_Float, "sourcePositionX");
		addScriptComponentVariable("texture", GameComponentVariable_Float, "sourcePositionY");
		addScriptComponentVariable("texture", GameComponentVariable_Float, "sourceWidth");
		addScriptComponentVariable("texture", GameComponentVariable_Float, "sourceHeight");

		SpriteSheetAnimationComponent spriteSheetAnimationComponent;
		addScriptComponentInterface(&spriteSheetAnimationComponent, sizeof(spriteSheetAnimationComponent), "spriteSheetAnimation");
		addScriptComponentVariable("spriteSheetAnimation", GameComponentVariable_Short, "currentFrame");
		addScriptComponentVariable("spriteSheetAnimation", GameComponentVariable_Short, "currentRow");
		addScriptComponentVariable("spriteSheetAnimation", GameComponentVariable_Short, "frameCount");
		addScriptComponentVariable("spriteSheetAnimation", GameComponentVariable_Short, "direction");
		addScriptComponentVariable("spriteSheetAnimation", GameComponentVariable_Short, "columnCount");
		addScriptComponentVariable("spriteSheetAnimation", GameComponentVariable_Float, "frameInterval");
		addScriptComponentVariable("spriteSheetAnimation", GameComponentVariable_Float, "passedTime");
	}

	GameEntitySystem::~GameEntitySystem()
	{
	}

	int GameEntitySystem::addEntity()
	{
		std::pair<int, GameComponentList> newPair;

		if(availableIDList->size())
		{
			newPair.first = *availableIDList->begin();
			availableIDList->erase(availableIDList->begin());
		}
		else
		{
			newPair.first = *highestEntityIndex;

			(*highestEntityIndex)++;
		}

		entityList->insert(newPair);

		(*entityCount)++;

		return newPair.first;
	}

	void GameEntitySystem::removeEntity(int entityID)
	{
		std::pair<int, GameComponentList> currentPair = *entityList->find(entityID);
		if(currentPair == (std::pair<int, GameComponentList>)*entityList->end()) return;

		if(isInUpdateLoop)
		{
			deadList->push_back(entityID);

			return;
		}

		std::vector<GameSystem *>::iterator listIterator;
		for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
		{
			(*listIterator)->entityList = entityList;

			(*listIterator)->onEntityRemove(entityID);
		}

		//removeEntityComponentList(entityID);
		removeEntityComponentList(&currentPair.second);

		entityList->erase(entityID);

		for(pair<int, GameEntityGroup> element : *entityGroupList)
		{
			element.second.entityList.erase(entityID);
		}

		availableIDList->push_back(entityID);

		(*entityCount)--;
	}

	void GameEntitySystem::removeEntity(int entityID, GameComponentList *componentList)
	{
		//std::pair<int, GameComponentList> currentPair = *entityList->find(entityID);
		//if(currentPair == (std::pair<int, GameComponentList>)*entityList->end()) return;

		if(isInUpdateLoop)
		{
			deadList->push_back(entityID);

			return;
		}

		std::vector<GameSystem *>::iterator listIterator;
		for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
		{
			(*listIterator)->entityList = entityList;

			(*listIterator)->onEntityRemove(entityID);
		}

		//removeEntityComponentList(entityID);
		removeEntityComponentList(componentList);

		entityList->erase(entityID);

		for(pair<int, GameEntityGroup> element : *entityGroupList)
		{
			element.second.entityList.erase(entityID);
		}

		availableIDList->push_back(entityID);

		(*entityCount)--;
	}

	void GameEntitySystem::removeEntityComponentList(int entityID)
	{
		std::vector<GameComponent *> *componentList = &entityList->find(entityID)->second;

		std::vector<GameComponent *>::iterator listIterator;
		for(listIterator = componentList->begin(); listIterator != componentList->end(); ++listIterator)
		{
			delete((*listIterator));

			//componentList->erase(listIterator);
		}

		componentList->clear();
	}

	void GameEntitySystem::removeEntityComponentList(GameComponentList *componentList)
	{
		std::vector<GameComponent *>::iterator listIterator;
		for(listIterator = componentList->begin(); listIterator != componentList->end(); ++listIterator)
		{
			delete((*listIterator));

			//componentList->erase(listIterator);
		}

		componentList->clear();
	}

	void GameEntitySystem::removeAllEntities()
	{
		if(entityList->size() < 1) return;

		for(pair<int, std::vector<GameComponent *>> element : *entityList)
		{
			std::vector<GameSystem *>::iterator listIterator;
			for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
			{
				(*listIterator)->entityList = entityList;

				(*listIterator)->onEntityRemove(element.first);
			}

			removeEntityComponentList(&element.second);

			(*entityCount)--;
		}

		entityList->clear();

		entityGroupList->clear();

		availableIDList->clear();
		*highestEntityIndex = 0;
	}

	void GameEntitySystem::addEntityToGroup(int entityID, int groupID)
	{
		if(entityList->find(entityID) == entityList->end()) return;

		if(entityGroupList->find(groupID) == entityGroupList->end())
		{
			std::pair<int, GameEntityGroup> newPair;
			newPair.first = groupID;
			GameComponentList *componentList = &entityList->find(entityID)->second;
			newPair.second.entityList.insert(std::pair<int, GameComponentList *>(entityID, componentList));

			entityGroupList->insert(newPair);
		}
		else
		{
			GameEntityPointerList *groupEntityList = &entityGroupList->find(groupID)->second.entityList;
			GameComponentList *componentList = &entityList->find(entityID)->second;
			groupEntityList->insert(std::pair<int, GameComponentList *>(entityID, componentList));
		}
	}

	void GameEntitySystem::removeEntityFromGroup(int entityID, int groupID)
	{
		if(entityGroupList->find(groupID) == entityGroupList->end()) return;
		if(entityList->find(entityID) == entityList->end()) return;

		entityGroupList->find(groupID)->second.entityList.erase(entityID);
	}

	void GameEntitySystem::removeAllEntitiesInGroup(int groupID)
	{
		if(entityGroupList->find(groupID) == entityGroupList->end()) return;

		GameEntityGroup *currentGroup = &entityGroupList->find(groupID)->second;

		if(isInUpdateLoop)
		{
			for(pair<int, GameComponentList *> element : currentGroup->entityList)
			{
				deadList->push_back(element.first);
			}

			return;
		}

		std::vector<GameSystem *>::iterator listIterator;
		for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
		{
			(*listIterator)->entityList = entityList;

			for(pair<int, GameComponentList *> element : currentGroup->entityList)
			{
				(*listIterator)->onEntityRemove(element.first);
			}
		}

		for(pair<int, GameComponentList *> element : currentGroup->entityList)
		{
			//removeEntity(element.first);
			//deadList->push_back(element.first);

			//removeEntityComponentList(entityID);
			removeEntityComponentList(element.second);

			entityList->erase(element.first);

			availableIDList->push_back(element.first);

			(*entityCount)--;
		}

		for(pair<int, GameEntityGroup> groupElement : *entityGroupList)
		{
			for(pair<int, GameComponentList *> element : currentGroup->entityList)
			{
				groupElement.second.entityList.erase(element.first);
			}
		}

		currentGroup->entityList.clear();
	}

	void GameEntitySystem::removeGroup(int groupID)
	{
		entityGroupList->erase(groupID);
	}

	bool GameEntitySystem::isEntityInGroup(int entityID, int groupID)
	{
		if(entityGroupList->find(groupID) == entityGroupList->end()) return 0;
		//if(entityList->find(entityID) == entityList->end()) return 0;

		GameEntityGroup *currentGroup = &entityGroupList->find(groupID)->second;

		if(currentGroup->entityList.find(entityID) != currentGroup->entityList.end()) return 1;

		return 0;
	}

	void GameEntitySystem::addComponent(int entityID, GameComponent *component, int componentSize)
	{
		if(entityList->find(entityID) == entityList->end() || !component || componentSize < 1) return;

		std::vector<GameComponent *> *componentList = &entityList->find(entityID)->second;

		void *newComponent = new char[componentSize];
		memcpy(newComponent, component, componentSize);

		componentSizeList[component->getType()] = componentSize;

		componentList->push_back((GameComponent *)newComponent);

		std::vector<GameSystem *>::iterator listIterator;
		for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
		{
			(*listIterator)->entityList = entityList;

			(*listIterator)->onEntityAddComponent(entityID, component->getType());
		}
	}

	GameComponent *GameEntitySystem::getComponent(int entityID, int componentTypeID)
	{
		std::vector<GameComponent *> *componentList = &entityList->find(entityID)->second;

		std::vector<GameComponent *>::iterator listIterator;
		for(listIterator = componentList->begin(); listIterator != componentList->end(); ++listIterator)
		{
			if((*listIterator)->getType() == componentTypeID) return *listIterator._Ptr;
		}

		return 0;
	}

	void GameEntitySystem::addSystem(GameSystem *gameSystem)
	{
		systemList.push_back(gameSystem);
	}

	void GameEntitySystem::update(float frameTime)
	{
		updateMessageList();

		isInUpdateLoop = 1;

		std::vector<GameSystem *>::iterator listIterator;
		for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
		{
			(*listIterator)->entityList = entityList;
			(*listIterator)->entityGroupList = entityGroupList;

			(*listIterator)->update(frameTime);
		}

		isInUpdateLoop = 0;
	}

	void GameEntitySystem::render()
	{
		isInUpdateLoop = 1;

		std::vector<GameSystem *>::iterator listIterator;
		for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
		{
			(*listIterator)->entityList = entityList;
			(*listIterator)->entityGroupList = entityGroupList;

			(*listIterator)->render();
		}

		isInUpdateLoop = 0;
	}

	void GameEntitySystem::removeDeadEntities()
	{
		std::list<int>::iterator listIterator;
		for(listIterator = deadList->begin(); listIterator != deadList->end(); ++listIterator)
		{
			removeEntity((*listIterator));
		}

		deadList->clear();
	}

	GameSystem::GameSystem()
	{
	}

	GameComponent *GameSystem::getComponent(std::vector<GameComponent *> *componentList, int componentTypeID)
	{
		/*std::vector<GameComponent *>::iterator listIterator;

		for(listIterator = componentList->begin(); listIterator != componentList->end(); ++listIterator)
		{
			if((*listIterator)->getType() == componentTypeID) return *listIterator._Ptr;
		}*/

		int arraySize = componentList->size();

		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID) return (*componentList)[i];
		}

		return 0;
	}

	void GameSystem::onEntityRemove(int entityID)
	{
	}

	int GameEntitySystem::getEntityCount()
	{
		return *entityCount;
	}

	int GameEntitySystem::getEntityPresetCount()
	{
		return entityPresetCount;
	}

	int GameEntitySystem::getHighestEntityID()
	{
		return *highestEntityIndex;
	}

	int GameEntitySystem::getDeadEntityCount()
	{
		return availableIDList->size();
	}

	bool GameEntitySystem::isEntityPresent(int entityID)
	{
		if(entityList->find(entityID) == entityList->end()) return 0;

		return 1;
	}

	int GameEntitySystem::saveEntityAsPreset(int entityID)
	{
		std::pair<int, GameComponentList> newPair;

		if(availablePresetIDList.size())
		{
			newPair.first = *availablePresetIDList.begin();
			availableIDList->erase(availablePresetIDList.begin());
		}
		else
		{
			newPair.first = highestEntityPresetIndex;

			highestEntityPresetIndex++;
		}

		GameComponentList *componentList = &entityList->find(entityID)->second;

		GameComponentList *presetComponentList = &newPair.second;

		GameComponentList::iterator listIterator;
		for(listIterator = componentList->begin(); listIterator != componentList->end(); ++listIterator)
		{
			int componentSize = componentSizeList.find((*listIterator)->getType())->second;

			void *newComponent = new char[componentSize];
			memcpy(newComponent, (*listIterator), componentSize);

			presetComponentList->push_back((GameComponent *)newComponent);
		}

		entityPresetList.insert(newPair);

		entityPresetCount++;

		return newPair.first;
	}

	int GameEntitySystem::addEntityFromPreset(int presetID)
	{
		if(entityPresetList.find(presetID)->first != presetID) return -1;

		int index = addEntity();
		GameComponentList *componentList = &entityPresetList.find(presetID)->second;
		GameComponentList *newComponentList = &entityList->find(index)->second;

		GameComponentList::iterator listIterator;
		for(listIterator = componentList->begin(); listIterator != componentList->end(); ++listIterator)
		{
			int componentSize = componentSizeList.find((*listIterator)->getType())->second;

			void *newComponent = new char[componentSize];
			memcpy(newComponent, (*listIterator), componentSize);

			newComponentList->push_back((GameComponent *)newComponent);

			std::vector<GameSystem *>::iterator listIterator;
			for(listIterator = systemList.begin(); listIterator != systemList.end(); ++listIterator)
			{
				(*listIterator)->entityList = entityList;

				(*listIterator)->onEntityAddComponent(index, ((GameComponent *)newComponent)->getType());
			}
		}

		return index;
	}

	void GameEntitySystem::removePreset(int presetID)
	{
		if(entityPresetList.find(presetID)->first != presetID) return;

		removePresetComponentList(presetID);

		entityPresetList.erase(presetID);

		availablePresetIDList.push_back(presetID);

		entityPresetCount--;
	}

	void GameEntitySystem::removePresetComponentList(int presetID)
	{
		std::vector<GameComponent *> *componentList = &entityPresetList.find(presetID)->second;

		std::vector<GameComponent *>::iterator listIterator;
		for(listIterator = componentList->begin(); listIterator != componentList->end(); ++listIterator)
		{
			delete((*listIterator));

			//componentList->erase(listIterator);
		}

		componentList->clear();
	}

	void GameEntitySystem::removeAllPresets()
	{
		for(pair<int, std::vector<GameComponent *>> element : entityPresetList)
		{
			removePresetComponentList(element.first);
		}

		entityPresetList.clear();

		availablePresetIDList.clear();
		highestEntityPresetIndex = 0;

		entityPresetCount = 0;
	}

	int GameEntitySystem::addScene()
	{
		GameScene newScene;
		sceneList.push_back(newScene);

		//setCurrentScene(sceneList.size() - 1);

		return sceneList.size() - 1;
	}

	void GameEntitySystem::setCurrentScene(int sceneIndex)
	{
		if(sceneIndex < 0 || sceneIndex > sceneList.size() - 1) sceneIndex = 0;

		entityList = &sceneList[sceneIndex].entityList;
		entityGroupList = &sceneList[sceneIndex].entityGroupList;
		highestEntityIndex = &sceneList[sceneIndex].highestEntityIndex;
		availableIDList = &sceneList[sceneIndex].availableIDList;
		entityCount = &sceneList[sceneIndex].entityCount;
		deadList = &sceneList[sceneIndex].deadList;

		entityList->reserve(1024);
	}

	void GameEntitySystem::removeScene(int sceneIndex)
	{
		if(sceneIndex < 0 || sceneIndex > sceneList.size() - 1 || sceneList.size() < 2) return;

		int currentSceneIndex = getCurrentSceneIndex();

		if(currentSceneIndex == -1)
		{
			setCurrentScene(0);

			currentSceneIndex = 0;
		}

		int counter = 0;

		GameSceneList::iterator listIterator;

		for(listIterator = sceneList.begin(); listIterator != sceneList.end(); ++listIterator)
		{
			if(counter == sceneIndex)
			{
				entityList = &(*listIterator).entityList;

				removeAllEntities();

				sceneList.erase(listIterator);
				
				break;
			}

			counter++;
		}

		if(sceneIndex < currentSceneIndex)
		{
			setCurrentScene(currentSceneIndex - 1);
		}
		else
		{
			setCurrentScene(currentSceneIndex);
		}
	}

	int GameEntitySystem::getSceneCount()
	{
		return sceneList.size();
	}

	int GameEntitySystem::getCurrentSceneIndex()
	{
		GameSceneList::iterator listIterator;
		int counter = 0;

		for(listIterator = sceneList.begin(); listIterator != sceneList.end(); ++listIterator)
		{
			if(entityList == &(*listIterator).entityList)
			{
				return counter;
			}

			counter++;
		}

		return -1;
	}

	GameEntitySystem::GameScene::GameScene()
	{
		highestEntityIndex = 0;

		entityCount = 0;
	}

	GameEntitySystem::GameScene::~GameScene()
	{
	}

	Timer timer;

	int getCurrentTimeMilliseconds()
	{
		return timer.getPassedTimeMilliseconds();
	}

	class GameEntitySystem::EventMessage
	{
	public:
		int message;

		int lifeSpan;
		int delay;
		int creationTime;

		EventMessage();
		~EventMessage();
	};

	GameEntitySystem::EventMessage::EventMessage()
	{
		message = EVENT_MESSAGE_NULL;

		lifeSpan = 0;
		delay = 0;
		creationTime = 0;
	}

	GameEntitySystem::EventMessage::~EventMessage()
	{
		message = EVENT_MESSAGE_NULL;

		lifeSpan = 0;
		delay = 0;
		creationTime = 0;
	}

	void GameEntitySystem::addMessage(int messageCode, int lifeSpan, int delay)
	{
		EventMessage newMessage;
		newMessage.message = messageCode;
		newMessage.lifeSpan = lifeSpan;
		newMessage.delay = delay;
		newMessage.creationTime = getCurrentTimeMilliseconds();

		messageList.push_back(newMessage);
	}

	bool GameEntitySystem::isMessagePresent(int messageCode)
	{
		int currentTime = getCurrentTimeMilliseconds();

		std::list<EventMessage>::iterator listIterator;

		for(listIterator = messageList.begin(); listIterator != messageList.end(); ++listIterator)
		{
			if((*listIterator).message == messageCode)
			{
				int elapsedTime = currentTime - (*listIterator).creationTime;

				if(elapsedTime > (*listIterator).delay)
				{
					return 1;
				}
			}
		}

		return 0;
	}

	void GameEntitySystem::updateMessageList()
	{
		int currentTime = getCurrentTimeMilliseconds();

		std::list<EventMessage>::iterator listIterator;

		for(listIterator = messageList.begin(); listIterator != messageList.end(); ++listIterator)
		{
			int elapsedTime = currentTime - (*listIterator).creationTime - (*listIterator).delay;

			if(elapsedTime > (*listIterator).lifeSpan)
			{
				messageList.erase(listIterator);

				listIterator--;
			}
		}
	}

	void GameEntitySystem::addScriptComponentInterface(GameComponent *component, int componentSize, std::string componentName)
	{
		if(!component || componentSize < 1 || componentName.length() < 1 || componentInterfaceList.find(componentName) != componentInterfaceList.end()) return;

		std::pair<std::string, GameComponentScriptInterface> newPair;

		newPair.first = componentName;
		newPair.second.defaultValue = new char[componentSize];
		memcpy(newPair.second.defaultValue, component, componentSize);
		newPair.second.size = componentSize;

		componentInterfaceList.insert(newPair);
	}

	void GameEntitySystem::addScriptComponentVariable(std::string componentName, GameComponentVariableType variableType, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		if(componentInterface->variableList.find(variableName) != componentInterface->variableList.end()) return;

		std::pair<std::string, GameComponentVariableInformation> newPair;

		newPair.first = variableName;
		newPair.second.type = variableType;

		switch(variableType)
		{
		case GameComponentVariable_Bool:
			newPair.second.pointer = (void *)(sizeof(int) + componentInterface->totalRegisteredVariableSize);
			componentInterface->totalRegisteredVariableSize += sizeof(bool);

			break;

		case GameComponentVariable_Char:
			newPair.second.pointer = (void *)(sizeof(int) + componentInterface->totalRegisteredVariableSize);
			componentInterface->totalRegisteredVariableSize += sizeof(char);

			break;

		case GameComponentVariable_Short:
			if(componentInterface->totalRegisteredVariableSize % 2 != 0)
			{
				componentInterface->totalRegisteredVariableSize = ((int)(componentInterface->totalRegisteredVariableSize / 2.0 + 0.5)) * 2;
			}

			newPair.second.pointer = (void *)(sizeof(int) + componentInterface->totalRegisteredVariableSize);
			componentInterface->totalRegisteredVariableSize += sizeof(short);

			break;

		case GameComponentVariable_Int:
			if(componentInterface->totalRegisteredVariableSize % 4 != 0)
			{
				componentInterface->totalRegisteredVariableSize = ((int)(componentInterface->totalRegisteredVariableSize / 4.0 + 0.75)) * 4;
			}

			newPair.second.pointer = (void *)(sizeof(int) + componentInterface->totalRegisteredVariableSize);
			componentInterface->totalRegisteredVariableSize += sizeof(int);

			break;

		case GameComponentVariable_Float:
			if(componentInterface->totalRegisteredVariableSize % 4 != 0)
			{
				componentInterface->totalRegisteredVariableSize = ((int)(componentInterface->totalRegisteredVariableSize / 4.0 + 0.75)) * 4;
			}

			newPair.second.pointer = (void *)(sizeof(int) + componentInterface->totalRegisteredVariableSize);
			componentInterface->totalRegisteredVariableSize += sizeof(float);

			break;
		}

		componentInterface->variableList.insert(newPair);
	}

	void GameEntitySystem::scriptInterfaceAddComponent(int entityID, std::string componentName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		addComponent(entityID, (GameComponent *)componentInterface->defaultValue, componentInterfaceList.find(componentName)->second.size);
	}

	void GameEntitySystem::scriptInterfaceSetComponentVariableBool(std::string componentName, std::string variableName, bool value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		bool *variablePointer = (bool *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		*variablePointer = value;
	}

	void GameEntitySystem::scriptInterfaceSetComponentVariableChar(std::string componentName, std::string variableName, char value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		char *variablePointer = (char *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		*variablePointer = value;
	}

	void GameEntitySystem::scriptInterfaceSetComponentVariableShort(std::string componentName, std::string variableName, short value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		short *variablePointer = (short *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		*variablePointer = value;
	}

	void GameEntitySystem::scriptInterfaceSetComponentVariableInt(std::string componentName, std::string variableName, int value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		int *variablePointer = (int *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		*variablePointer = value;
	}

	void GameEntitySystem::scriptInterfaceSetComponentVariableFloat(std::string componentName, std::string variableName, float value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		float *variablePointer = (float *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		*variablePointer = value;
	}

	void GameEntitySystem::scriptInterfaceSetEntityComponentVariableBool(int entityID, std::string componentName, std::string variableName, bool value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				bool *variablePointer = (bool *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				*variablePointer = value;
			}
		}
	}

	void GameEntitySystem::scriptInterfaceSetEntityComponentVariableChar(int entityID, std::string componentName, std::string variableName, char value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				char *variablePointer = (char *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				*variablePointer = value;
			}
		}
	}

	void GameEntitySystem::scriptInterfaceSetEntityComponentVariableShort(int entityID, std::string componentName, std::string variableName, short value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				short *variablePointer = (short *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				*variablePointer = value;
			}
		}
	}

	void GameEntitySystem::scriptInterfaceSetEntityComponentVariableInt(int entityID, std::string componentName, std::string variableName, int value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				int *variablePointer = (int *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				*variablePointer = value;
			}
		}
	}

	void GameEntitySystem::scriptInterfaceSetEntityComponentVariableFloat(int entityID, std::string componentName, std::string variableName, float value)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				float *variablePointer = (float *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				*variablePointer = value;
			}
		}
	}

	bool GameEntitySystem::scriptInterfaceGetComponentVariableBool(std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		bool *variablePointer = (bool *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		return *variablePointer;
	}

	char GameEntitySystem::scriptInterfaceGetComponentVariableChar(std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		char *variablePointer = (char *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		return *variablePointer;
	}

	short GameEntitySystem::scriptInterfaceGetComponentVariableShort(std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		short *variablePointer = (short *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		return *variablePointer;
	}

	int GameEntitySystem::scriptInterfaceGetComponentVariableInt(std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		int *variablePointer = (int *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		return *variablePointer;
	}

	float GameEntitySystem::scriptInterfaceGetComponentVariableFloat(std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		GameComponent *component = (GameComponent *)componentInterface->defaultValue;

		float *variablePointer = (float *)((int)(component) + (int)componentInterface->variableList.find(variableName)->second.pointer);
		return *variablePointer;
	}

	bool GameEntitySystem::scriptInterfaceGetEntityComponentVariableBool(int entityID, std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				bool *variablePointer = (bool *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				return *variablePointer;
			}
		}

		return 0;
	}

	char GameEntitySystem::scriptInterfaceGetEntityComponentVariableChar(int entityID, std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				char *variablePointer = (char *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				return *variablePointer;
			}
		}

		return 0;
	}

	short GameEntitySystem::scriptInterfaceGetEntityComponentVariableShort(int entityID, std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				short *variablePointer = (short *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				return *variablePointer;
			}
		}

		return 0;
	}

	int GameEntitySystem::scriptInterfaceGetEntityComponentVariableInt(int entityID, std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				int *variablePointer = (int *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				return *variablePointer;
			}
		}

		return 0;
	}

	float GameEntitySystem::scriptInterfaceGetEntityComponentVariableFloat(int entityID, std::string componentName, std::string variableName)
	{
		auto foundComponentInterface = componentInterfaceList.find(componentName);
		if(foundComponentInterface == componentInterfaceList.end()) return 0;

		GameComponentScriptInterface *componentInterface = &foundComponentInterface->second;

		int componentTypeID = ((GameComponent *)componentInterface->defaultValue)->getType();

		std::vector<GameComponent*> *componentList = &entityList->find(entityID)->second;

		int arraySize = componentList->size();
		for(int i = 0; i < arraySize; ++i)
		{
			if((*componentList)[i]->getType() == componentTypeID)
			{
				float *variablePointer = (float *)((int)((*componentList)[i]) + (int)componentInterface->variableList.find(variableName)->second.pointer);
				return *variablePointer;
			}
		}

		return 0;
	}

	GameComponentVariableType GameEntitySystem::getComponentVariableType(std::string componentName, std::string variableName)
	{
		return componentInterfaceList.find(componentName)->second.variableList.find(variableName)->second.type;
	}

	void GameSystem::sendMessage(int messageCode, int lifeSpan, int delay)
	{
		entitySystem.addMessage(messageCode, lifeSpan, delay);
	}

	bool GameSystem::isMessagePresent(int messageCode)
	{
		return entitySystem.isMessagePresent(messageCode);
	}
};