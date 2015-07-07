#ifndef _ENTITYSYSTEM_H
#define _ENTITYSYSTEM_H 1

#include "../Eagle.h"

#define EVENT_MESSAGE_NULL -1

namespace ProjectEagle
{
	enum GameComponentVariableType
	{
		GameComponentVariable_Bool = 0,
		GameComponentVariable_Char = 1,
		GameComponentVariable_Short = 2,
		GameComponentVariable_Int = 3,
		GameComponentVariable_Float = 4
	};

	class GameComponent
	{
		friend class GameEntitySystem;
		friend class GameSystem;

	protected:
		int componentTypeID;

	public:
		int getType(){return componentTypeID;}
	};

	typedef int Entity;

	typedef std::vector<GameComponent *> GameComponentList;
	typedef std::unordered_map<int, GameComponentList> GameEntityList;
	typedef std::unordered_map<int, GameComponentList *> GameEntityPointerList;

	class GameEntityGroup
	{
	public:
		GameEntityGroup();
		~GameEntityGroup();

		GameEntityPointerList entityList;
	};

	typedef std::unordered_map<int, GameEntityGroup> GameEntityGroupList;

	class GameSystem
	{
		friend class GameEntitySystem;

	protected:
		GameSystem();

		GameComponent *getComponent(GameComponentList *componentList, int componentTypeID);
		GameEntityList *getGroup(int groupID);

		void sendMessage(int messageCode, int lifeSpan, int delay);
		bool isMessagePresent(int messageCode);

		virtual void onEntityAddComponent(int entityID, int componentTypeID){};
		virtual void onEntityRemove(int entityID);

	public:
		virtual void update(float frameTime){};
		virtual void render(){};

		GameEntityList *entityList;
		GameEntityGroupList *entityGroupList;
	};

	typedef std::vector<GameSystem *> GameSystemList;

	typedef std::unordered_map<int, int> GameComponentSizeList;

	enum GameComponentVariableType;

	class GameComponentScriptInterface;

	class GameEntitySystem
	{
	private:
		class GameScene
		{
		public:
			GameScene();
			~GameScene();

			GameEntityList entityList;
			GameEntityGroupList entityGroupList;

			int highestEntityIndex;
			std::list<int> availableIDList;

			std::list<int> deadList;

			int entityCount;
		};

		typedef std::vector<GameScene> GameSceneList;

		GameSystemList systemList;
		GameEntityList *entityList;
		GameEntityGroupList *entityGroupList;
		GameComponentSizeList componentSizeList;
		GameEntityList entityPresetList;

		GameSceneList sceneList;

		int *highestEntityIndex;
		int highestEntityPresetIndex;

		void removeEntity(int entityID, GameComponentList *componentList);

		void removeEntityComponentList(int entityID);
		void removeEntityComponentList(GameComponentList *componentList);
		void removePresetComponentList(int presetID);

		std::list<int> *availableIDList;
		std::list<int> availablePresetIDList;

		std::list<int> *deadList;

		int *entityCount;
		int entityPresetCount;

		bool isInUpdateLoop;

		class EventMessage;

		std::list<EventMessage> messageList;

		void updateMessageList();

		std::unordered_map<std::string, GameComponentScriptInterface> componentInterfaceList;

	public:
		GameEntitySystem();
		~GameEntitySystem();

		int addEntity();
		void removeEntity(int entityID);
		void removeAllEntities();

		void addEntityToGroup(int entityID, int groupID);
		void removeEntityFromGroup(int entityID, int groupID);
		void removeAllEntitiesInGroup(int groupID);
		void removeGroup(int groupID);

		bool isEntityInGroup(int entityID, int groupID);

		void addComponent(int entityID, GameComponent *component, int componentSize);
		GameComponent *getComponent(int entityID, int componentTypeID);

		int saveEntityAsPreset(int entityID);
		int addEntityFromPreset(int presetID);
		void removePreset(int presetID);
		void removeAllPresets();

		void addSystem(GameSystem *gameSystem);

		int getEntityCount();
		int getEntityPresetCount();

		int getHighestEntityID();
		int getDeadEntityCount();

		bool isEntityPresent(int entityID);

		int addScene();
		int getSceneCount();
		
		void setCurrentScene(int sceneIndex);
		int getCurrentSceneIndex();

		void removeScene(int sceneIndex);

		void update(float frameTime);
		void render();
		void removeDeadEntities();

		void addMessage(int messageCode, int lifeSpan, int delay);
		bool isMessagePresent(int messageCode);

		void addScriptComponentInterface(GameComponent *component, int componentSize, std::string componentName);
		void addScriptComponentVariable(std::string componentName, GameComponentVariableType variableType, std::string variableName);
		void scriptInterfaceAddComponent(int entityID, std::string componentName);

		GameComponentVariableType getComponentVariableType(std::string componentName, std::string variableName);

		void scriptInterfaceSetComponentVariableBool(std::string componentName, std::string variableName, bool value);
		void scriptInterfaceSetComponentVariableChar(std::string componentName, std::string variableName, char value);
		void scriptInterfaceSetComponentVariableShort(std::string componentName, std::string variableName, short value);
		void scriptInterfaceSetComponentVariableInt(std::string componentName, std::string variableName, int value);
		void scriptInterfaceSetComponentVariableFloat(std::string componentName, std::string variableName, float value);

		void scriptInterfaceSetEntityComponentVariableBool(int entityID, std::string componentName, std::string variableName, bool value);
		void scriptInterfaceSetEntityComponentVariableChar(int entityID, std::string componentName, std::string variableName, char value);
		void scriptInterfaceSetEntityComponentVariableShort(int entityID, std::string componentName, std::string variableName, short value);
		void scriptInterfaceSetEntityComponentVariableInt(int entityID, std::string componentName, std::string variableName, int value);
		void scriptInterfaceSetEntityComponentVariableFloat(int entityID, std::string componentName, std::string variableName, float value);

		bool scriptInterfaceGetComponentVariableBool(std::string componentName, std::string variableName);
		char scriptInterfaceGetComponentVariableChar(std::string componentName, std::string variableName);
		short scriptInterfaceGetComponentVariableShort(std::string componentName, std::string variableName);
		int scriptInterfaceGetComponentVariableInt(std::string componentName, std::string variableName);
		float scriptInterfaceGetComponentVariableFloat(std::string componentName, std::string variableName);

		bool scriptInterfaceGetEntityComponentVariableBool(int entityID, std::string componentName, std::string variableName);
		char scriptInterfaceGetEntityComponentVariableChar(int entityID, std::string componentName, std::string variableName);
		short scriptInterfaceGetEntityComponentVariableShort(int entityID, std::string componentName, std::string variableName);
		int scriptInterfaceGetEntityComponentVariableInt(int entityID, std::string componentName, std::string variableName);
		float scriptInterfaceGetEntityComponentVariableFloat(int entityID, std::string componentName, std::string variableName);
	};
};

#endif