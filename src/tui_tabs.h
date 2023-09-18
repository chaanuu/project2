/*this file contains following elements:
struct tabs_appearance_a - describes active/inactive tabs appearance, used by tabs_appearance
struct tabs_appearance - describes tabs appearance
struct tabs - widget that displays tabs*/
#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"

#include <vector>

namespace tui
{
	struct tabs_appearance_a
	{
		symbol separator;
		symbol prev_arrow;
		symbol next_arrow;
	public:
		tabs_appearance_a(DIRECTION direction = DIRECTION::HORIZONTAL)
		{
			switch (direction)
			{
			case DIRECTION::HORIZONTAL:
				*this = tabs_appearance_a('|', U'\x25C4', U'\x25BA');
				break;
			case DIRECTION::VERTICAL:
				*this = tabs_appearance_a('-', U'\x25B2', U'\x25BC');
			}
		}
		tabs_appearance_a(symbol separator, symbol prev_arrow, symbol next_arrow)
			: separator(separator), prev_arrow(prev_arrow), next_arrow(next_arrow) {}

		void setColor(color Color)
		{
			separator.setColor(Color);
			prev_arrow.setColor(Color);
			next_arrow.setColor(Color);
		}
	};

	struct tabs_appearance : appearance
	{
	protected:
		tabs_appearance_a active_appearance;
		tabs_appearance_a inactive_appearance;
	public:
		tabs_appearance(DIRECTION direction = DIRECTION::HORIZONTAL) : active_appearance(direction), inactive_appearance(direction)
		{
			inactive_appearance.setColor(COLOR::DARKGRAY);
		}
		tabs_appearance(tabs_appearance_a active, tabs_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(tabs_appearance appearance) { setElement(*this, appearance); }
		tabs_appearance getAppearance() const { return *this; }

		void setActiveAppearance(tabs_appearance_a appearance) { setElement(active_appearance, appearance); }
		tabs_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(tabs_appearance_a appearance) { setElement(inactive_appearance, appearance); }
		tabs_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};

	template<DIRECTION direction>
	struct tabs : surface1D<direction>, tabs_appearance, active_element
	{
	private:
		struct tab_info
		{
			symbol_string string;
			unsigned int position = 0;
		};

		std::vector<tab_info> m_tabs;
		symbol_string m_generated_tabs;
		unsigned int m_selected = 0;
		unsigned int m_first_pos = 0;

		bool m_display_separator = true;
		bool m_wrap_around = true;
		bool m_redraw_needed = true;

		tabs_appearance_a getCurrentAppearance() const
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void generateTabs()
		{
			m_generated_tabs.clear();

			for (int i = 0; i < m_tabs.size(); i++)
			{
				m_tabs[i].position = m_generated_tabs.size();
				m_generated_tabs += getFullWidthString(m_tabs[i].string);

				if (m_display_separator && i < m_tabs.size() - 1) 
				{
					m_generated_tabs += getCurrentAppearance().separator;
				}
			}
		}

		void fill()
		{
			if (m_tabs.size() > 0)
			{
				bool too_long = m_generated_tabs.size() > surface1D<direction>::getSize();

				int len = surface1D<direction>::getSize() - 2 * too_long;

				if (too_long)
				{
					surface1D<direction>::setSymbolAt(getCurrentAppearance().prev_arrow, 0);
					surface1D<direction>::setSymbolAt(getCurrentAppearance().next_arrow, surface1D<direction>::getSize() - 1);

					if (m_generated_tabs.size() - m_first_pos < len)
					{
						m_first_pos = m_generated_tabs.size() - len;
					}

					if (m_tabs[m_selected].position + getFullWidthString(m_tabs[m_selected].string).size() > m_first_pos + len)
					{
						m_first_pos = m_tabs[m_selected].position + getFullWidthString(m_tabs[m_selected].string).size() - len;
					}

					if (m_tabs[m_selected].position <= m_first_pos)
					{
						m_first_pos = m_tabs[m_selected].position;
					}
				}
				else 
				{
					surface1D<direction>::makeTransparent();
					m_first_pos = 0; 
				}

				for (int i = too_long, j = m_first_pos; j < m_generated_tabs.size() && i < surface1D<direction>::getSize() - too_long; i++, j++)
				{
					switch (j >= m_tabs[m_selected].position && j < m_tabs[m_selected].position + getFullWidthString(m_tabs[m_selected].string).size())
					{
					case false:
						surface1D<direction>::setSymbolAt(m_generated_tabs[j], i);
						break;
					case true:
						symbol s = m_generated_tabs[j];
						s.invert();
						surface1D<direction>::setSymbolAt(s, i);
					}
				}
			}
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void setAppearanceAction() override 
		{
			generateTabs();
			m_redraw_needed = true; 
		}
		void drawAction(surface::action_proxy proxy) override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		}
		void activationAction() override
		{
			generateTabs();
			m_redraw_needed = true;
		}
		void deactivationAction() override
		{
			generateTabs();
			m_redraw_needed = true;
		}
	public:
		input::key_t key_prev = input::KEY::LEFT;
		input::key_t key_next = input::KEY::RIGHT;

		tabs(surface1D_size size = surface1D_size()) : tabs_appearance(direction) 
		{
			if (direction == tui::DIRECTION::VERTICAL)
			{
				key_prev = input::KEY::UP;
				key_next = input::KEY::DOWN;
			}

			surface1D<direction>::setSizeInfo(size);
		}

		void setTabs(const std::vector<symbol_string>& tabs)
		{
			m_tabs.resize(tabs.size());
			for (int i = 0; i < m_tabs.size(); i++)
			{
				m_tabs[i].string = tabs[i];
			}
			generateTabs();
			m_selected = 0;
			m_redraw_needed = true;
		}

		std::vector<symbol_string> getTabs() const
		{
			std::vector<symbol_string> tabs;

			for (int i = 0; i < m_tabs.size(); i++)
			{
				tabs.push_back(m_tabs[i].string);
			}

			return tabs;
		}

		void resizeToTabs()
		{
			surface1D<direction>::setSizeInfo({ m_generated_tabs.size() });
			m_redraw_needed = true;
		}

		unsigned int getTabCount() const { return m_tabs.size(); }
		
		void setSelected(unsigned int s)
		{
			if (m_selected != s)
			{
				m_selected = s;
				m_redraw_needed = true;
			}
		}
		unsigned int getSelected() const { return m_selected; }

		void displaySeparator(bool use)
		{
			if (m_display_separator != use) 
			{
				m_display_separator = use;
				generateTabs();
				m_redraw_needed = true; 
			}
		}
		bool isDiplayingSeparator() const { return m_display_separator; }

		void useWrappingAround(bool use) { m_wrap_around = use; }
		bool isUsingWrappingAround() { return m_wrap_around; }

		void nextTab()
		{
			m_selected = m_selected < m_tabs.size()-1 ? m_selected+1 : (m_wrap_around ? 0 : m_selected);
			m_redraw_needed = true;
		}
		void prevTab()
		{
			m_selected = m_selected > 0 ? m_selected-1 : (m_wrap_around ? m_tabs.size()-1 : m_selected);
			m_redraw_needed = true;
		}

		void update()
		{
			if (isActive())
			{
				if (input::isKeyPressed(key_prev)) { prevTab(); }
				if (input::isKeyPressed(key_next)) { nextTab(); }
			}
		}
	};
}
