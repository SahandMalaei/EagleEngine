#include "Eagle.h"

namespace ProjectEagle
{
	const float TEXT_BOX_KEY_MINIMUM_HOLD_TIME = 0.4;
	const float TEXT_BOX_KEY_ENTER_INTERVAL = 0.05;

	Button::Button()
	{
		state = BUTTON_UP;
		up = new Sprite();
		over = new Sprite();
		down = new Sprite();
		up->setColor(COLOR_RGBA(255, 255, 255, 255));
		over->setColor(COLOR_RGBA(255, 255, 255, 255));
		down->setColor(COLOR_RGBA(255, 255, 255, 255));
		position.x = 0;
		position.y = 0;
		pressed = 0;
		released = 0;
	}

	bool Button::isUp()
	{
		return (state == BUTTON_UP);
	}

	bool Button::isMouseOverIt()
	{
		return (state == BUTTON_OVER);
	}

	bool Button::isDown()
	{
		return (state == BUTTON_DOWN);
	}

	bool Button::isPressed()
	{
		return pressed;
	}

	bool Button::isReleased()
	{
		return released;
	}

	void Button::Update()
	{
		/*up->setPosition(m_position);
		over->setPosition(m_position);
		down->setPosition(m_position);

		pressed = 0;
		released = 0;

		if(state == BUTTON_UP)
		{
			up->setVisible(1);
			over->setVisible(0);
			down->setVisible(0);

			if(up->getCollisionRectangle().isPointIntersecting(input.getMousePosition().x, input.getMousePosition().y)) {state = BUTTON_OVER; return;}
		}
		else if(state == BUTTON_OVER)
		{
			up->setVisible(0);
			over->setVisible(1);
			down->setVisible(0);

			if(over->getCollisionRectangle().isPointIntersecting(input.getMousePosition().x, input.getMousePosition().y))
			{
				if(input.isMouseButtonDown(Mouse_Left))
				{
					state = BUTTON_DOWN;
					pressed = 1;
					return;
				}
			}
			else
			{
				state = BUTTON_UP;
			}
		}
		else if(state == BUTTON_DOWN)
		{
			up->setVisible(0);
			over->setVisible(0);
			down->setVisible(1);

			if(input.isMouseButtonUp(Mouse_Left))
			{
				released = 1;
				state = BUTTON_OVER;
				return;
			}

			if(!down->getCollisionRectangle().isPointIntersecting(input.getMousePosition().x, input.getMousePosition().y))
			{
				released = 1;
				state = BUTTON_UP;
				return;
			}
		}*/
	}

	void Button::draw()
	{
		up->draw();
		over->draw();
		down->draw();
	}

	void Button::drawTransformed()
	{
		up->drawTransformed();
		over->drawTransformed();
		down->drawTransformed();
	}

#ifdef PLATFORM_WP8
	using namespace Windows::Foundation;
	using namespace Windows::UI::Core;
	using namespace Windows::Phone::UI::Core;
	using namespace Windows::System;
	using namespace Platform;
#endif

	TextBox::TextBox()
	{
		m_isActive = 0;
		m_font.setScale(1);
		m_font.setColor(COLOR_RGBA(0, 0, 0, 220));
		m_position.set(0, 0, 0);
		m_text = "";
		m_caretPosition = 0;
		m_caretInterval = 0.5;
		m_showCaret = 0;
		m_caretTimer.reset();
		m_offset.set(5, 5);
		m_maxLength = 15;

		m_width = 200;
		m_height = 50;

		m_currentHoldingButtonIndex = -1;

#ifdef PLATFORM_WP8
		//inputBuffer = ref new KeyboardInputBuffer();

		//inputBuffer->Text = initialText;
		//inputBuffer->TextChanged += ref new TypedEventHandler<KeyboardInputBuffer^, CoreTextChangedEventArgs^>(this, &TextBox::onTextChanged);
#endif
	}

	TextBox::~TextBox()
	{
	}

	void TextBox::update()
	{
		if(m_caretTimer.stopWatch(m_caretInterval))
		{
			m_showCaret = !m_showCaret;

			m_caretTimer.reset();
		}

		Vector2 mousePosition = Vector2(input.getMousePosition().x + graphics.getCamera()->getPosition().x - graphics.getScreenWidth() / 2, input.getMousePosition().y + graphics.getCamera()->getPosition().y - graphics.getScreenHeight() / 2);

		if(input.isMouseButtonPressed(Mouse_Left))
		{
			int min = mousePosition.x - m_position.x;
			Rectangle boundingBox(m_position.x, m_position.y, m_position.x + m_width, m_position.y + m_height);

			if(boundingBox.isPointIntersecting(mousePosition))
			{
				if(m_text.length() == 0)
				{
					m_caretPosition = 0;
				}
				else if (mousePosition.x - m_position.x < m_offset.x + m_font.getPrintedTextLength(m_text.substr(0, 1)) / 2)
				{
					m_caretPosition = 0;
				}
				else if(mousePosition.x - m_position.x > m_offset.x + m_font.getPrintedTextLength(m_text.substr(0, m_text.length() - 1)) + m_font.getPrintedTextLength(m_text.substr(m_text.length() - 1, 1)) / 2)
				{
					m_caretPosition = m_text.length();
				}
				else
				{
					for(int i = 1; i <= m_text.length(); ++i)
					{
						int l = m_font.getPrintedTextLength(m_text.substr(0, i));

						if(math.abs(mousePosition.x - (m_position.x + l + m_offset.x)) < min)
						{
							m_caretPosition = i;

							min = math.abs(mousePosition.x - (m_position.x + l + m_offset.x));
						}
					}

					if(m_caretPosition > m_text.length() - 1)
					{
						m_caretPosition = m_text.length() - 1;
					}
				}

				m_isActive = 1;
			}
			else
			{
				m_isActive = 0;
			}
		}

		if(m_isActive)
		{
			for(int i = 0; i < 256; ++i)
			{
				if(input.isKeyPressed((KeyboardKey)i))
				{
					m_currentHoldingButtonIndex = i;

					if(i == Keyboard_LShift)
					{
						continue;
					}
					else if(i == Keyboard_RShift)
					{
						continue;
					}
					else if(i == Keyboard_LAlt)
					{
						continue;
					}
					else if(i == Keyboard_LControl)
					{
						continue;
					}
					else if(i == Keyboard_Back)
					{
						if(m_caretPosition > 0)
						{
							m_text.erase(m_caretPosition - 1, 1);

							m_caretPosition--;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_Delete)
					{
						if(m_caretPosition < m_text.length())
						{
							m_text.erase(m_caretPosition, 1);

							m_showCaret = 1;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_Escape)
					{
						continue;
					}
					else if(i == Keyboard_Return)
					{
						continue;
					}
					else if(i == Keyboard_Up || i == Keyboard_Down)
					{
						continue;
					}
					else if(i == Keyboard_Left)
					{
						if(m_caretPosition > 0)
						{
							m_caretPosition--;

							m_showCaret = 1;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_Right)
					{
						if(m_caretPosition < m_text.length())
						{
							m_caretPosition++;

							m_showCaret = 1;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_V && input.isKeyDown(Keyboard_LControl))
					{
#ifndef PLATFORM_WP8
						if(!OpenClipboard(0))
						{
							continue;
						}

						HANDLE clipboardDataHandle = GetClipboardData(CF_TEXT);

						if(!clipboardDataHandle)
						{
							continue;
						}

						char *clipboardText = static_cast<char *>(GlobalLock(clipboardDataHandle));

						if(!clipboardText)
						{
							continue;
						}

						m_text.insert(m_caretPosition, clipboardText);

						m_caretPosition += strlen(clipboardText);

						GlobalUnlock(clipboardDataHandle);

						CloseClipboard();
#endif

						continue;
					}

					int c = dikToAscii(i);

					char ch = c;

					m_text.insert(m_caretPosition, &ch, 1);

					m_caretPosition++;

					m_keyEnterInterval.reset();

					resetCaretFlash();

					resetCaretTarget();
				}
				else if(input.isKeyDown((KeyboardKey)i) && input.getKeyHoldDuration((KeyboardKey)i) > TEXT_BOX_KEY_MINIMUM_HOLD_TIME && m_keyEnterInterval.stopWatch(TEXT_BOX_KEY_ENTER_INTERVAL) && m_currentHoldingButtonIndex == i)
				{
					m_keyEnterInterval.reset();

					if(i == Keyboard_LShift)
					{
						continue;
					}
					else if(i == Keyboard_RShift)
					{
						continue;
					}
					else if(i == Keyboard_LAlt)
					{
						continue;
					}
					else if(i == Keyboard_LControl)
					{
						continue;
					}
					else if(i == Keyboard_Back)
					{
						if(m_caretPosition > 0)
						{
							m_text.erase(m_caretPosition - 1, 1);

							m_caretPosition--;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_Delete)
					{
						if(m_caretPosition < m_text.length())
						{
							m_text.erase(m_caretPosition, 1);

							m_showCaret = 1;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_Escape)
					{
						continue;
					}
					else if(i == Keyboard_Return)
					{
						continue;
					}
					else if(i == Keyboard_Up || i == Keyboard_Down)
					{
						continue;
					}
					else if(i == Keyboard_Left)
					{
						if(m_caretPosition > 0)
						{
							m_caretPosition--;

							m_showCaret = 1;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_Right)
					{
						if(m_caretPosition < m_text.length())
						{
							m_caretPosition++;

							m_showCaret = 1;

							resetCaretFlash();
						}

						continue;
					}
					else if(i == Keyboard_V && input.isKeyDown(Keyboard_LControl))
					{
						continue;
					}

					int c = dikToAscii(i);

					char ch = c;

					m_text.insert(m_caretPosition, &ch, 1);

					m_caretPosition++;

					resetCaretFlash();

					for(int j = 0; j < 2; ++j)
					{
						moveCaret();
					}

					resetCaretTarget();
				}
			}

			if(m_text.length() > m_maxLength)
			{
				m_text = m_text.substr(0, m_maxLength);

				resetCaretTarget();
			}

			if(m_caretPosition < 0)
			{
				m_caretPosition = 0;

				resetCaretTarget();
			}
			else if(m_caretPosition > m_maxLength)
			{
				m_caretPosition = m_maxLength;

				resetCaretTarget();
			}
		}

		moveCaret();
		resetCaretTarget();

#ifdef PLATFORM_WP8
		//m_text = wstrToStr(inputBuffer->Text->Data());
#endif
	}

	void TextBox::moveCaret()
	{
		Vector2 distanceVector = m_targetCaretPosition - m_visualCaretPosition;
		m_visualCaretPosition += distanceVector / 3;
	}

	void TextBox::resetCaretTarget()
	{
		m_targetCaretPosition = Vector2(m_offset.x + m_position.x + m_font.getPrintedTextLength(m_text.substr(0, m_caretPosition)), m_offset.y + m_position.y);
	}

	void TextBox::draw()
	{
		m_font.print(m_offset.x + m_position.x, m_offset.y + m_position.y, m_position.z, m_text);

		if(m_isActive && m_showCaret)
		{
			graphics.drawLine(m_visualCaretPosition, m_visualCaretPosition + Vector2(0, m_font.getCharacterHeight() * m_font.getScale().y), 1 + m_font.getCharacterWidth() / 50, m_font.getColor(), m_position.z);
		}
	}

	void TextBox::drawTransformed()
	{
		m_font.printTransformed(m_offset.x + m_position.x, m_offset.y + m_position.y, m_position.z, m_text);

		if(m_isActive && m_showCaret)
		{
			graphics.drawLineTransformed(m_visualCaretPosition, m_visualCaretPosition + Vector2(0, m_font.getCharacterHeight() * m_font.getScale().y), 1 + m_font.getCharacterWidth() / 50, m_font.getColor(), m_position.z);
		}
	}

	std::string TextBox::getText()
	{
		return m_text;
	}

	void TextBox::setText(std::string value)
	{
		m_text = value;

		if(m_text.length() > m_maxLength)
		{
			m_text = value.substr(0, m_maxLength);
		}

		m_caretPosition = value.length();

		resetCaretFlash();
	}

	void TextBox::resetCaretFlash()
	{
		m_caretTimer.reset();
		m_showCaret = 1;
	}

	float TextBox::getWidth()
	{
		return m_width;
	}

	void TextBox::setWidth(float w)
	{
		m_width = w;
	}

	float TextBox::getHeight()
	{
		return m_height;
	}

	void TextBox::setHeight(float h)
	{
		m_height = h;
	}

	Vector2 TextBox::getDimensions()
	{
		return Vector2(m_width, m_height);
	}

	void TextBox::setDimensions(Vector2 dimensions)
	{
		m_width = dimensions.x;
		m_height = dimensions.y;
	}

	Vector3 TextBox::getPosition()
	{
		return m_position;
	}

	void TextBox::setPosition(Vector3 position)
	{
		m_position = position;
	}

	float TextBox::getCaretInterval()
	{
		return m_caretInterval;
	}

	void TextBox::setCaretInterval(float seconds)
	{
		m_caretInterval = seconds;
	}

	Vector2 TextBox::getOffset()
	{
		return m_offset;
	}

	void TextBox::setOffset(int x, int y)
	{
		m_offset.set(x, y);
	}

	int TextBox::getCaretPosition()
	{
		return m_caretPosition;
	}

	int TextBox::getMaxLength()
	{
		return m_maxLength;
	}

	void TextBox::setMaxLength(int maxLength)
	{
		m_maxLength = maxLength;
	}

	BitmapFont *TextBox::getFont()
	{
		return &m_font;
	}

	bool TextBox::isActive()
	{
		return m_isActive;
	}

	void TextBox::setActive(bool state)
	{
		m_isActive = state;
	}
};