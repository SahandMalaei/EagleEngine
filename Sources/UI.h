#ifndef _UI_H
#define _UI_H 1

#include "Eagle.h"

#define BUTTON_UP 0
#define BUTTON_OVER 1
#define BUTTON_DOWN 2

namespace ProjectEagle
{
	class Button
	{
	private:
		int state;
		Vector3 position;
		bool pressed, released;

	public:
		Button();
		~Button();

		Sprite *up, *over, *down;

		void loadImageForUp(char *address){up->loadImage(address);}
		void loadImageForOver(char *address){over->loadImage(address);}
		void loadImageForDown(char *address){down->loadImage(address);}

		void setPosition(Vector3 value){position = value;}
		void setPosition(int x, int y, int z){position.set(x, y, z);}
		Vector3 getPosition(){return position;}

		void setRotation(Vector3 value){up->setRotation(value); over->setRotation(value); down->setRotation(value);}
		void setRotation(float x, float y, float z){up->setRotation(x, y, z); over->setRotation(x, y, z); down->setRotation(x, y, z);}

		//void setAlpha(int a){up->setAlpha(a); over->setAlpha(a); down->setAlpha(a);}
		void setColor(DWORD c){up->setColor(c); over->setColor(c); down->setColor(c);}

		void setDepth(float d){Vector3 newPosition = up->getPosition(); newPosition.z = d; setPosition(newPosition);}

		void Update();
		void draw();
		void drawTransformed();

		bool isUp();
		bool isMouseOverIt();
		bool isDown();
		bool isPressed();
		bool isReleased();
	};

	class TextBox
	{
	private:
		bool m_isActive;
		BitmapFont m_font;
		std::string m_text;
		Vector3 m_position;
		float m_width, m_height;
		int m_caretPosition;
		Vector2 m_visualCaretPosition;
		Vector2 m_targetCaretPosition;
		Timer m_caretTimer;
		float m_caretInterval;
		bool m_showCaret;
		Vector2 m_offset;
		unsigned int m_maxLength;
		Timer m_keyEnterInterval;
		short m_currentHoldingButtonIndex;

		void moveCaret();
		void resetCaretTarget();

#ifdef PLATFORM_WP8
		Windows::UI::Core::CoreWindow^ parentWindow;
		Windows::Phone::UI::Core::KeyboardInputBuffer^ inputBuffer;

		Windows::Phone::UI::Core::CoreInputScope inputScope;
#endif

	protected:
#ifdef PLATFORM_WP8
		void onTextChanged(Windows::Phone::UI::Core::KeyboardInputBuffer^ sender, Windows::Phone::UI::Core::CoreTextChangedEventArgs^ args);
#endif

	public:
		TextBox();
		virtual ~TextBox();
		
		void update();
		void updateTransformed();
		void draw();
		void drawTransformed();

		float getWidth();
		void setWidth(float w);

		float getHeight();
		void setHeight(float h);

		Vector2 getDimensions();

		void setDimensions(float w, float h);
		void setDimensions(Vector2 d);

		Vector3 getPosition();
		void setPosition(Vector3 position);

		void setText(std::string s);
		std::string getText();

		float getCaretInterval();
		void setCaretInterval(float seconds);

		bool isActive();
		void setActive(bool state);

		Vector2 getOffset();
		void setOffset(int x, int y);

		int getCaretPosition();

		int getMaxLength();
		void setMaxLength(int max);

		BitmapFont *getFont();

		void resetCaretFlash();
	};
};

#endif