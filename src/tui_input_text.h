/*this file contains following elements:
struct input_text_appearance_a - describes active/inactive input_text appearance, used by input_text_appearance
struct input_text_appearance - describes input_text appearance
struct input_text - widget that displays a textbox*/
#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"
#include "tui_time_frame.h"
#include "tui_text.h"

namespace tui
{
	struct input_text_appearance_a
	{
		symbol insert_cursor;
		symbol overtype_cursor;
		color text_color;
		text_appearance_a itxt_text_appearance_a;

		input_text_appearance_a() : input_text_appearance_a('_', U'\x2584', color()) {}
		input_text_appearance_a(symbol insert_cursor, symbol overtype_cursor, color text_color) : input_text_appearance_a(insert_cursor, overtype_cursor, text_color, text_appearance_a()) {}
		input_text_appearance_a(symbol insert_cursor, symbol overtype_cursor, color text_color, text_appearance_a txt_appearance)
			: insert_cursor(insert_cursor), overtype_cursor(overtype_cursor), text_color(text_color), itxt_text_appearance_a(txt_appearance) {}

		void setColor(color Color)
		{
			insert_cursor.setColor(Color);
			overtype_cursor.setColor(Color);
			text_color = Color;
			itxt_text_appearance_a.setColor(Color);
		}
	};

	struct input_text_appearance : appearance
	{
	protected:
		input_text_appearance_a active_appearance;
		input_text_appearance_a inactive_appearance;
	public:
		input_text_appearance() 
		{
			inactive_appearance.itxt_text_appearance_a.setColor(COLOR::DARKGRAY);
		}
		input_text_appearance(input_text_appearance_a active, input_text_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(input_text_appearance appearance) { setElement(*this, appearance); }
		input_text_appearance getAppearance() const { return *this; }

		void setActiveAppearance(input_text_appearance_a active) { setElement(active_appearance, active); }
		input_text_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(input_text_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		input_text_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};

	struct input_text : surface, active_element, input_text_appearance
	{
	private:
		text m_text;
		symbol_string m_str;
		vec2i m_cursor_pos = { 0,0 };
		unsigned int m_cursor_sym_idx = 0;
		time_frame m_cursor_blink;

		bool m_redraw_needed = true;
		bool m_insert_mode = true;
		bool m_confidential_mode = false;
		bool m_blink = true;

		input_text_appearance_a gca() const
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void updateCursorPos()
		{
			vec2i old_pos = m_cursor_pos;

			if (m_str.size() > 0)
			{
				auto getRelativePos = [&]()
				{
					vec2i pos = m_text.getSymbolPos(m_cursor_sym_idx);
					pos.y -= m_text.getLine();

					return pos;
				};

				m_cursor_pos = getRelativePos();

				if (m_cursor_pos.y >= getSize().y)
				{
					int c = m_cursor_pos.y - (getSize().y-1);

					m_text.goToLine(m_text.getLine()+c);
					m_cursor_pos = getRelativePos();
				}
				else if (m_cursor_pos.y < 0)
				{
					int c = m_cursor_pos.y * -1;

					m_text.goToLine(m_text.getLine()-c);
					m_cursor_pos = getRelativePos();
				}

			}
			else { m_cursor_pos = { 0,0 }; }

			if (m_cursor_pos != old_pos) { m_redraw_needed = true; }
		}

		void updateText()
		{
			symbol_string s = m_str;
			
			if (m_confidential_mode)
			{
				for (int i = 0; i < s.size(); i++)
				{
					if (s[i] != '\n') { s[i] = '*'; }
				}
			}
			
			s += " ";

			if (m_cursor_sym_idx > m_str.size()) { m_cursor_sym_idx = m_str.size(); }

			m_text.setText(s);
		}
		
		void moveCursorRight(unsigned int n = 1)
		{
			if (m_cursor_sym_idx + n <= m_str.size()) 
			{ 
				m_cursor_sym_idx += n; 
				m_redraw_needed = true;
			}
			else
			{
				m_cursor_sym_idx = m_str.size();
				m_redraw_needed = true;
			}
		}
		void moveCursorLeft(unsigned int n = 1)
		{
			if (m_cursor_sym_idx >= n) 
			{ 
				m_cursor_sym_idx -= n; 
				m_redraw_needed = true;
			}
			else
			{
				m_cursor_sym_idx = 0;
				m_redraw_needed = true;
			}
		}
		void moveCursorUp(unsigned int n = 1)
		{
			vec2i c_pos = m_text.getSymbolPos(m_cursor_sym_idx);

			if (c_pos.y > 0)
			{
				if (c_pos.y < n)
				{
					n = c_pos.y;
				}

				for (int l_pos = m_cursor_sym_idx - 1; l_pos >= 0; l_pos--)
				{
					if (m_text.getSymbolPos(l_pos).y == c_pos.y - n && m_text.getSymbolPos(l_pos).x <= c_pos.x)
					{
						m_cursor_sym_idx = l_pos;
						m_redraw_needed = true;
						break;
					}
					else if (m_text.getSymbolPos(l_pos).y < c_pos.y - n)
					{
						m_cursor_sym_idx = l_pos + 1;
						m_redraw_needed = true;
						break;
					}
				}
			}
		}
		void moveCursorDown(unsigned int n = 1)
		{
			vec2i c_pos = m_text.getSymbolPos(m_cursor_sym_idx);

			if (c_pos.y < m_text.getNumberOfLines()-1)
			{
				if (c_pos.y + n >= m_text.getNumberOfLines())
				{
					n = m_text.getNumberOfLines() - c_pos.y - 1;
				}

				for (int l_pos = m_cursor_sym_idx + 1; l_pos <= m_str.size(); l_pos++)
				{
					if (m_text.getSymbolPos(l_pos).y == c_pos.y + n && (m_text.getSymbolPos(l_pos).x >= c_pos.x || l_pos == m_str.size()))
					{
						m_cursor_sym_idx = l_pos;
						m_redraw_needed = true;
						break;
					}
					else if (m_text.getSymbolPos(l_pos).y > c_pos.y + n)
					{
						m_cursor_sym_idx = l_pos - 1;
						m_redraw_needed = true;
						break;
					}
				}
			}
		}

		void moveCursorHome()
		{
			vec2i c_pos = m_text.getSymbolPos(m_cursor_sym_idx);

			for (int l_pos = m_cursor_sym_idx - 1; l_pos >= 0; l_pos--)
			{
				if (m_text.getSymbolPos(l_pos).y < c_pos.y)
				{
					m_cursor_sym_idx = l_pos + 1;
					m_redraw_needed = true;
					break;
				}
				else if (l_pos == 0)
				{
					m_cursor_sym_idx = 0;
					m_redraw_needed = true;
					break;
				}
			}
		}
		void moveCursorEnd()
		{
			vec2i c_pos = m_text.getSymbolPos(m_cursor_sym_idx);

			for (int l_pos = m_cursor_sym_idx + 1; l_pos <= m_str.size(); l_pos++)
			{
				if (m_text.getSymbolPos(l_pos).y > c_pos.y)
				{
					m_cursor_sym_idx = l_pos - 1;
					m_redraw_needed = true;
					break;
				}
				else if (l_pos == m_str.size())
				{
					m_cursor_sym_idx = l_pos;
					m_redraw_needed = true;
					break;
				}
			}
		}

		void fill()
		{		
			updateCursorPos();

			if (m_redraw_needed)
			{
				clear();
				insertSurface(m_text, false);

				bool scr = m_text.isDisplayingScrollNow();

				for (int y = 0; y < getSize().y; y++)
				{
					for (int x = 0; x < getSize().x - scr; x++)
					{
						(*this)[x][y].setColor(gca().text_color);
					}
				}
			}

			if (isActive() && (m_cursor_blink.isEnd(false) || m_redraw_needed))
			{
				if (m_blink)
				{
					if (m_insert_mode) { setSymbolAt(gca().insert_cursor, m_cursor_pos); }
					else { setSymbolAt(gca().overtype_cursor, m_cursor_pos); }
				}
				else
				{
					symbol sym = m_text.getSymbolAt(m_cursor_pos);
					sym.setColor(gca().text_color);
					setSymbolAt(sym, m_cursor_pos);
				}

				if (m_cursor_blink.isEnd(true)) { m_blink = !m_blink; }
			}

			m_redraw_needed = false;
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction(action_proxy proxy) override { fill(); }

		void activationAction() override
		{
			m_text.activate();
			m_redraw_needed = true;
		}
		void deactivationAction() override
		{
			m_text.deactivate();
			m_redraw_needed = true;
		}

		void setAppearanceAction() override
		{
			m_text.setAppearance({active_appearance.itxt_text_appearance_a, inactive_appearance.itxt_text_appearance_a});
			m_redraw_needed = true;
		}
	public:
		input::key_t key_up = input::KEY::UP;
		input::key_t key_down = input::KEY::DOWN;
		input::key_t key_pgup = input::KEY::PGUP;
		input::key_t key_pgdn = input::KEY::PGDN;
		input::key_t key_left = input::KEY::LEFT;
		input::key_t key_right = input::KEY::RIGHT;
		input::key_t key_home = input::KEY::HOME;
		input::key_t key_end = input::KEY::END;
		input::key_t key_insert = input::KEY::INS;
		input::key_t key_backspace = input::KEY::BACKSPACE;

		input_text(surface_size size = surface_size()) : m_cursor_blink(std::chrono::milliseconds(500))
		{
			m_text.setSizeInfo({ {0,0},{100,100} });
			m_text.useWordBreaking(false);
			updateText();

			setSizeInfo(size);
		}

		void setText(const symbol_string& str)
		{
			m_str = str;
			updateText();
			m_redraw_needed = true;
		}
		symbol_string getText() const { return m_str; }

		void setCursorIndex(unsigned int idx)
		{
			m_cursor_sym_idx = std::min(idx, (unsigned int)m_str.size());
			m_redraw_needed = true;
		}
		unsigned int getCursorIndex() { return m_cursor_sym_idx; }

		vec2i getCursorPosition() { return m_cursor_pos; }

		void useConfidentialMode(bool use)
		{
			m_confidential_mode = use;
			updateText();
			m_redraw_needed = true;
		}
		bool isUsingConfidentialMode() const { return m_confidential_mode; }

		void useWordBreaking(bool use)
		{
			m_text.useWordBreaking(use);
			m_redraw_needed = true;
		}
		bool isUsingWordBreaking() { return m_text.isUsingWordBreaking(); }

		void update()
		{
			if (isActive())
			{
				std::vector<short> input = input::getInput();
				std::string buffer;

				auto addText = [&](symbol_string str)
				{
					if (m_insert_mode)
					{
						for (int i = 0; i < str.size(); i++)
						{
							m_str.insert(m_str.begin() + m_cursor_sym_idx + i, str[i]);
						}
					}
					else//overtype
					{
						for (int i = 0; i < str.size(); i++)
						{
							if (i + m_cursor_sym_idx < m_str.size())
							{
								m_str[i + m_cursor_sym_idx] = str[i];
							}
							else
							{
								m_str.push_back(str[i]);
							}
						}
					}
					moveCursorRight(str.size());
				};
				auto isSpecialKey = [&](short key)
				{
					return key == key_backspace
						|| key == key_insert
						|| key == key_left
						|| key == key_right
						|| key == key_up
						|| key == key_down
						|| key == key_pgup
						|| key == key_pgdn
						|| key == key_home
						|| key == key_end;
				};

				bool update_needed = false;
				auto update = [&]()
				{
					if (buffer.size() > 0 || update_needed) { updateText(); }
					update_needed = false;
				};

				for (int i = 0; i < input.size(); i++)
				{
					if (isSpecialKey(input[i]))
					{
						addText((symbol_string)buffer);

						if (input[i] == key_backspace)
						{
							if (m_cursor_sym_idx > 0)
							{
								m_str.erase(m_str.begin() + m_cursor_sym_idx - 1);
								moveCursorLeft();

								update_needed = true;
							}
						}
						else if (input[i] == key_left)
						{
							moveCursorLeft();
						}
						else if (input[i] == key_right)
						{
							moveCursorRight();
						}
						else if (input[i] == key_up)
						{
							update();
							moveCursorUp();
						}
						else if (input[i] == key_down)
						{
							update();
							moveCursorDown();
						}
						else if (input[i] == key_pgup)
						{
							update();
							moveCursorUp(getSize().y);
						}
						else if (input[i] == key_pgdn)
						{
							update();
							moveCursorDown(getSize().y);
						}
						else if (input[i] == key_home)
						{
							update();
							moveCursorHome();
						}
						else if (input[i] == key_end)
						{
							update();
							moveCursorEnd();
						}
						else if (input[i] == key_insert)
						{
							m_insert_mode = !m_insert_mode;
						}

						buffer.clear();
					}
					else if (input[i] >= 32 && input[i] <= 255)
					{
						buffer.push_back(input[i]);
					}
					else
					{
						switch (input[i])
						{
						case input::KEY::ENTER:
							buffer.push_back('\n');
							break;
						case input::KEY::TAB:
							buffer.push_back('\t');
						}
					}
				}

				addText((symbol_string)buffer);
				update();

			}
		}
	};
}
