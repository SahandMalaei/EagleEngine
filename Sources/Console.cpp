#include "Eagle.h"

const float EAGLE_CONSOLE_DEPTH = 0.01;
const float EAGLE_CONSOLE_HEIGHT = 240;
const float EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT = 30;
const float EAGLE_CONSOLE_CHARACTER_HEIGHT = 17;
const float EAGLE_FONT_LINE_SPACING = 1.0 / 17.0;
const float EAGLE_CONSOLE_FONT_DEPTH_OFFSET = 0.00;

#define EAGLE_CONSOLE_RESTORE_SPEED (2 * EAGLE_CONSOLE_HEIGHT * 2 * (*eagle.getFrameTimePointer()))
#define EAGLE_CONSOLE_INFORMATION_PANEL_RESTORE_SPEED (2 * EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT * 2 * (*eagle.getFrameTimePointer()));

const int CONSOLE_MAX_LINE_COUNT = 256;

const ColorValue CONSOLE_OUTPUT_COLOR_DEFAULT = COLOR_RGBA(1, 1, 1, 1);
const ColorValue CONSOLE_OUTPUT_COLOR_SUCCESS = COLOR_RGBA(0, 1, 0, 1);
const ColorValue CONSOLE_OUTPUT_COLOR_WARNING = COLOR_RGBA(1, 0, 0, 1);

namespace ProjectEagle
{
	Script commandHandler;

	Console::Console()
	{
		initialized = 0;

		showing = 0;

		mainPanelLocked = 0;
		informationPanelLocked = 0;
	}

	Console::~Console()
	{
	}

	bool Console::initialize()
	{
		topColor = COLOR_RGBA(0.1, 0.1, 0.1, 0.9);
		bottomColor = COLOR_RGBA(0.1, 0.1, 0.1, 0.9);
		informationPanelBottomColor = COLOR_RGBA(0.1, 0.1, 0.1, 0.8);
		position.y = graphics.getScreenHeight();
		informationPanelPosition.y = -EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT;
		height = EAGLE_CONSOLE_HEIGHT;
		informationPanelHeight = EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT;
		fontOffset.set(5, 5);

#ifndef PLATFORM_WP8
		if(!font.loadImage("Data/Fonts/Arial/Arial.png"))
		{
			return 0;
		}
#else
		if(!font.loadImage("Data/Fonts/Arial/Arial.dds"))
		{
			return 0;
		}
#endif
		font.detectCharacterDimensionsFromTexture();

		if(!font.loadWidthData("Data/Fonts/Arial/Arial.dat"))
		{
			return 0;
		}

#ifndef PLATFORM_WP8
		font.setScale((float)(EAGLE_CONSOLE_CHARACTER_HEIGHT) / font.getCharacterHeight());
#else
		font.setScale((float)(EAGLE_CONSOLE_CHARACTER_HEIGHT) / font.getCharacterHeight() * 1.5);
#endif

#ifndef PLATFORM_WP8
		if(!textBox.getFont()->loadImage("Data/Fonts/Arial/Arial.png"))
		{
			return 0;
		}
#else
		if(!textBox.getFont()->loadImage("Data/Fonts/Arial/Arial.dds")) 
		{
			return 0;
		}
#endif
		textBox.getFont()->detectCharacterDimensionsFromTexture();

		if(!textBox.getFont()->loadWidthData("Data/Fonts/Arial/Arial.dat"))
		{
			return 0;
		}

#ifndef PLATFORM_WP8
		textBox.getFont()->setScale((float)(EAGLE_CONSOLE_CHARACTER_HEIGHT) / font.getCharacterHeight() * 1.2);
#else
		textBox.getFont()->setScale((float)(EAGLE_CONSOLE_CHARACTER_HEIGHT) / font.getCharacterHeight() * 1.7);
#endif

		lineCount = (EAGLE_CONSOLE_HEIGHT * 0.9) / ((font.getCharacterHeight() * font.getScale().x + EAGLE_FONT_LINE_SPACING)) - 1;
		
		if(lineCount > CONSOLE_MAX_LINE_COUNT)
		{
			lineCount = CONSOLE_MAX_LINE_COUNT;
		}

		for(int i = 0; i < lineCount; ++i)
		{
			outputStringList.push_back("");
			outputTypeList.push_back(ConsoleOutput_Default);
		}

		textBox.setMaxLength(128);
		textBox.setOffset(0, 0);

		font.setColor(COLOR_RGBA(1, 1, 1, 1));
		textBox.getFont()->setColor(COLOR_RGBA(1, 1, 1, 1));

		selectedEntryIndex = 0;

		initialized = 1;

		return 1;
	}
	
	void Console::update()
	{
		if(input.isKeyPressed(Keyboard_Grave) || input.isKeyPressed(Keyboard_F12))
		{
			showing = !showing;
		}

		if(!showing || mainPanelLocked)
		{
			textBox.setActive(0);
		}

		textBox.setPosition(Vector3(fontOffset.x, position.y + height - font.getCharacterHeight() * font.getScale().y * 2, 0));

		textBox.update();

		if(showing && !mainPanelLocked)
		{
			textBox.setActive(1);
		}

		if(textBox.isActive())
		{
			while(textBox.getFont()->getPrintedTextLength(textBox.getText()) > graphics.getScreenWidth() - fontOffset.x * 2)
			{
				textBox.setText(textBox.getText().substr(0, textBox.getText().length() - 1));
			}
		}

		if(textBox.isActive() && input.isKeyPressed(Keyboard_Return) && textBox.getText().length() > 0)
		{
			string commandText = textBox.getText();

			if(commandText.length() > 2 && commandText.substr(commandText.length() - 2, 2) == "//")
			{
				commandText = commandText.substr(0, commandText.length() - 2);

				print(commandText, ConsoleOutput_Success, 0);

				commandBuffer += commandText;
			}
			else
			{
				commandBuffer += commandText;

				print(commandText, ConsoleOutput_Success, 0);

				handleCommand(commandBuffer);

				commandBuffer = "";
			}

			if(entryList.size() > 0)
			{
				if(commandText != entryList[entryList.size() - 1])
				{
					entryList.push_back(commandText);

					selectedEntryIndex = entryList.size();
				}
			}
			else
			{
				entryList.push_back(commandText);

				selectedEntryIndex = entryList.size();
			}

			textBox.setText("");
		}

		if(textBox.isActive() && input.isKeyPressed(Keyboard_Up) && selectedEntryIndex > 0)
		{
			selectedEntryIndex--;

			textBox.setText(entryList[selectedEntryIndex]);
		}
		else if(textBox.isActive() && input.isKeyPressed(Keyboard_Down) && selectedEntryIndex < entryList.size())
		{
			selectedEntryIndex++;

			if(selectedEntryIndex == entryList.size())
			{
				textBox.setText("");
			}
			else
			{
				textBox.setText(entryList[selectedEntryIndex]);
			}
		}

		if(!updateTimer.stopWatch(1.0 / eagle.getPictureFrameRate()))
		{
			return;
		}
		else
		{
			updateTimer.reset();
		}

		if(!showing) 
		{
			if(position.y < graphics.getScreenHeight())
			{
				position.y += EAGLE_CONSOLE_RESTORE_SPEED;

				if(position.y > graphics.getScreenHeight())
				{
					position.y = graphics.getScreenHeight();
				}
			}
			else
			{
				position.y = graphics.getScreenHeight();
			}

			if(informationPanelPosition.y > - EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT - 1)
			{
				informationPanelPosition.y -= EAGLE_CONSOLE_INFORMATION_PANEL_RESTORE_SPEED;

				if(informationPanelPosition.y < -EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT - 1)
				{
					informationPanelPosition.y = -EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT - 1;
				}
			}
			else
			{
				informationPanelPosition.y = -EAGLE_CONSOLE_INFORMATION_PANEL_HEIGHT - 1;
			}
		}
		else
		{
			if(!mainPanelLocked)
			{
				if(position.y > graphics.getScreenHeight() - EAGLE_CONSOLE_HEIGHT)
				{
					position.y -= EAGLE_CONSOLE_RESTORE_SPEED;

					if(position.y < graphics.getScreenHeight() - EAGLE_CONSOLE_HEIGHT)
					{
						position.y = graphics.getScreenHeight() - EAGLE_CONSOLE_HEIGHT;
					}
				}
				else
				{
					position.y = graphics.getScreenHeight() - EAGLE_CONSOLE_HEIGHT;
				}
			}

			if(!informationPanelLocked)
			{
				if(informationPanelPosition.y < 0)
				{
					informationPanelPosition.y += EAGLE_CONSOLE_INFORMATION_PANEL_RESTORE_SPEED;

					if(informationPanelPosition.y > 0)
					{
						informationPanelPosition.y = 0;
					}
				}
				else
				{
					informationPanelPosition.y = 0;
				}
			}
		}

		//position.set(position.x, position.y);
	}

	void Console::draw()
	{
		if(math.approximateEquals(position.y, -EAGLE_CONSOLE_HEIGHT - 1, 1))
		{
			return;
		}

#ifndef PLATFORM_WP8
		font.setScale((float)(EAGLE_CONSOLE_CHARACTER_HEIGHT) / font.getCharacterHeight());
#else
		font.setScale((float)(EAGLE_CONSOLE_CHARACTER_HEIGHT) / font.getCharacterHeight() * 1.5);
#endif

		graphics.setTexture(0);
		graphics.fillQuadrilateralTransformed(0, informationPanelPosition.y, graphics.getScreenWidth(), informationPanelPosition.y, 0, informationPanelPosition.y + informationPanelHeight, graphics.getScreenWidth(), informationPanelPosition.y + informationPanelHeight, (ColorValue)bottomColor, (ColorValue)bottomColor, (ColorValue)informationPanelBottomColor, (ColorValue)informationPanelBottomColor, 0);
		graphics.fillQuadrilateralTransformed(0, position.y, graphics.getScreenWidth(), position.y, 0, position.y + height, graphics.getScreenWidth(), position.y + height, (ColorValue)topColor, (ColorValue)topColor, (ColorValue)bottomColor, (ColorValue)bottomColor, 0);
		//graphics.fillRectangleTransformed(0, position.y + height - font.getCharacterHeight() * font.getScale().y * 2.1, graphics.getScreenWidth(), position.y + height, COLOR_RGB(0.1, 0.1, 0.1), 0);
		//graphics.fillRectangleTransformed(0, position.y + height - font.getCharacterHeight() * font.getScale().y * 1.2, graphics.getScreenWidth(), position.y + height, COLOR_RGB(0.9, 0.9, 0.9), 0);

		for(int i = 0; i < lineCount; ++i)
		{
			if(outputStringList[i].length() == 0) continue;

			switch(outputTypeList[i])
			{
			case ConsoleOutput_Default:
				font.setColor(CONSOLE_OUTPUT_COLOR_DEFAULT);

				break;

			case ConsoleOutput_Success:
				font.setColor(CONSOLE_OUTPUT_COLOR_SUCCESS);

				break;

			case ConsoleOutput_Warning:
				font.setColor(CONSOLE_OUTPUT_COLOR_WARNING);

				break;
			}

			font.printTransformed(fontOffset.x, fontOffset.y + position.y + (i * EAGLE_FONT_LINE_SPACING + i) * font.getCharacterHeight() * font.getScale().y, 0 * EAGLE_CONSOLE_DEPTH - EAGLE_CONSOLE_FONT_DEPTH_OFFSET, outputStringList[i]);
		}

		font.setColor(CONSOLE_OUTPUT_COLOR_DEFAULT);

		std::string fpsString = FLOAT_TO_STRING(eagle.getCoreFrameRate()).substr(0, 5) + " fps";

		font.printTransformed(graphics.getScreenWidth() - font.getPrintedTextLength(fpsString) - fontOffset.x * 1.1, informationPanelPosition.y + informationPanelHeight - font.getCharacterHeight() * font.getScale().y * 1.2, 0, fpsString);

		textBox.drawTransformed();

		graphics.setTexture(0);
	}

	void Console::print(std::string text, ConsoleOutputType outputType, bool formatting)
	{
		if(!initialized)
		{
			return;
		}

		int textLineCount = 1;
		int textLength = text.length();

		if(formatting)
		{
			for(int i = 0; i < textLength; ++i)
			{
				if(text[i] == '\n')
				{
					textLineCount++;

					continue;
				}

				if(i < textLength - 1)
				{
					if(text[i] == '\\' && text[i + 1] == 'n')
					{
						text.erase(i, 1);
						text[i] = '\n';

						textLineCount++;

						continue;
					}
				}
			}
		}

		for(int i = 0; i < textLineCount; ++i)
		{
			for(int j = 0; j < lineCount - 1; ++j)
			{
				outputStringList[j] = outputStringList[j + 1];
				outputTypeList[j] = outputTypeList[j + 1];
			}
		}

		int stringStartIndex = 0;
		int currentLineIndex = 0;

		for(int i = 0; i < textLength; ++i)
		{
			if(text[i] == '\n' || i == textLength - 1)
			{
				outputStringList[lineCount - (textLineCount - currentLineIndex)] = text.substr(stringStartIndex, i - stringStartIndex + 1);
				outputTypeList[lineCount - (textLineCount - currentLineIndex)] = outputType;

				currentLineIndex++;
				stringStartIndex = i + 1;
			}
		}
	}

	void Console::clear()
	{
		if(!initialized)
		{
			return;
		}

		for(int i = 0; i < lineCount; ++i)
		{
			outputStringList[i] = "";
			outputTypeList[i] = ConsoleOutput_Default;
		}
	}

	void Console::printEngineVersion()
	{
		print(eagle.getVersionText());
	}

	void Console::printFrameRate()
	{
		print(INT_TO_STRING(eagle.getCoreFrameRate()) + " fps");
	}

	void Console::printFrameTime()
	{
		print("Frame time : " + FLOAT_TO_STRING((*eagle.getFrameTimePointer()) * 1000) + " ms");
	}

	void Console::printDisplayAdapterName()
	{
		print(graphics.getDisplayAdapterName());
	}

	void Console::printAudioDriverName()
	{
		char *name = audio.getDriverName();

		print(name);

		free(name);
	}

	void Console::handleCommand(std::string commandText)
	{
		commandText.insert(0, "function initialize(){");
		commandText += ";\n}";

		commandHandler.loadEagleScriptFromString(commandText);
	}
};