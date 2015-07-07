#ifndef _CONSOLE_H
#define _CONSOLE_H 1

#include "Eagle.h"

namespace ProjectEagle
{
	enum ConsoleOutputType
	{
		ConsoleOutput_Default = 0,
		ConsoleOutput_Success = 1,
		ConsoleOutput_Warning = 2
	};

	class Console
    {
		friend class EagleEngine;
		friend class GraphicsSystem;

    private:
		bool initialized;

		bool showing;

		Vector2 position;
		Vector2 informationPanelPosition;

		Vector2 scale;

		Vector2 fontOffset;

		int height;
		int informationPanelHeight;

		BitmapFont font;

		int lineCount;

		std::vector<std::string> outputStringList;
		std::vector<ConsoleOutputType> outputTypeList;

		Timer updateTimer;

		ColorValue topColor, bottomColor, informationPanelBottomColor;

		TextBox textBox;

		bool mainPanelLocked;
		bool informationPanelLocked;

		short selectedEntryIndex;
		std::vector<std::string> entryList;

		std::string commandBuffer;

		void handleCommand(std::string commandText);

		bool initialize();
		void update();
		void draw();

	public:
		Console();
		~Console();

		void clear();
		void print(std::string text, ConsoleOutputType = ConsoleOutput_Default, bool formatting = 1);

		bool isInitialized(){return initialized;}

		bool isShowing(){return showing;}
		void show(){showing = 1;}
		void hide(){showing = 0;}
		void setShowing(bool s){showing = s;}
		void toggle(){showing = !showing;}

		//void setPanelColorRGB(int r, int g, int b){panel->setColorRGB(r, g, b);}
		//void setPanelColorRGBA(int r, int g, int b, int a){panel->setColorRGBA(r, g, b, a);}
		void setPanelColor(int value){topColor = value; bottomColor = value;}
		void setPanelTopColor(int value){topColor = value;}
		void setPanelBottomColor(int value){bottomColor = value;}

		ColorValue getPanelTopColor(){return topColor;}
		ColorValue getPanelBottomColor(){return bottomColor;}

		//void setFontColorRGB(int r, int g, int b){font.setColorRGB(r, g, b);}
		//void setFontColorRGBA(int r, int g, int b, int a){font.setColorRGBA(r, g, b, a);}
		void setFontColor(int value){font.setColor(value);}

		//void setPanelAlpha(float a){panel->setAlpha(a);}
		//void setFontAlpha(float a){font.setAlpha(a);}
		//void setAlpha(float a){panel->setAlpha(a); font.setAlpha(a);}

		void printEngineVersion();
		void printFrameRate();
		void printFrameTime();
		void printDisplayAdapterName();
		void printAudioDriverName();

		//Sprite *getPanel(){return panel;}
		BitmapFont *getFont(){return &font;}

		void lock(){mainPanelLocked = 1; informationPanelLocked = 1;}
		void unlock(){mainPanelLocked = 0; informationPanelLocked = 0;}
		void setLockState(bool state){mainPanelLocked = state; informationPanelLocked = state;}

		bool isMainPanelLocked(){return mainPanelLocked;}
		bool isInformationPanelLocked(){return informationPanelLocked;}

		void lockMainPanel(){mainPanelLocked = 1;}
		void unlockMainPanel(){mainPanelLocked = 0;}
		void setMainPanelLockState(bool state){mainPanelLocked = state;}

		void lockInformationPanel(){informationPanelLocked = 1;}
		void unlockInformationPanel(){informationPanelLocked = 0;}
		void setInformationPanelLockState(bool state){informationPanelLocked = state;}
	};
};

#endif