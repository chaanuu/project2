/*this file contains following elements:
struct chart_appearance_a - describes active/inactive chart appearance, used by chart_appearance
struct chart_appearance - describes chart appearance
struct chart - widget that displays a chart*/
#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"
#include "tui_scroll.h"

#include <vector>
#include <algorithm>
#include <cmath>

namespace tui
{
	struct chart_appearance_a
	{
		symbol full;
		symbol lower_half;
		symbol upper_half;
		scroll_appearance_a chart_scroll_appearance_a;
		color value_labels_color;

		chart_appearance_a() : chart_appearance_a(U'\x2588', U'\x2584', U'\x2580') {}
		chart_appearance_a(symbol Full, symbol Lower, symbol Upper)
			: full(Full), lower_half(Lower), upper_half(Upper), chart_scroll_appearance_a(tui::DIRECTION::HORIZONTAL) {}

		void setColor(color Color)
		{
			full.setColor(Color);
			lower_half.setColor(Color);
			upper_half.setColor(Color);
			value_labels_color = Color;
			chart_scroll_appearance_a.setColor(Color);
		}
	};

	struct chart_appearance : appearance
	{
	protected:
		chart_appearance_a active_appearance;
		chart_appearance_a inactive_appearance;
	public:
		chart_appearance()
		{
			inactive_appearance.chart_scroll_appearance_a.setColor(tui::COLOR::DARKGRAY);
		}
		chart_appearance(chart_appearance_a active, chart_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}
		void setAppearance(chart_appearance appearance) { setElement(*this, appearance); }
		chart_appearance getAppearance() const { return *this; }

		void setActiveAppearance(chart_appearance_a active) { setElement(active_appearance, active); }
		chart_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(chart_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		chart_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};

	struct chart_data_unit
	{
		float value;
		tui::symbol_string name;
		chart_data_unit() : chart_data_unit(0) {}
		chart_data_unit(float value) : chart_data_unit(value, "") {}
		chart_data_unit(float value, symbol_string name) : value(value), name(name) {}
	};

	struct chart : surface, chart_appearance, active_element
	{
	private:
		std::vector<chart_data_unit> m_values;

		tui::scroll<tui::DIRECTION::HORIZONTAL> m_scroll;
		tui::surface m_chart;

		unsigned int m_distance = 2;
		bool m_display_value_labels = false;
		int m_value_labels_precision = -1;
		bool m_display_data_labels = true;

		float m_min = 0;
		float m_max = 0;
		symbol_string m_min_str;
		symbol_string m_max_str;

		std::string m_unit;

		bool m_redraw_needed = true;

		chart_appearance_a gca() const
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void updateMinMax()
		{
			if (m_values.size() > 0)
			{
				m_min = std::min_element(
					m_values.begin(),
					m_values.end(),
					[](const chart_data_unit& a, const chart_data_unit& b)
					{
						return a.value < b.value;
					}
				)->value;
				m_max = std::max_element(
					m_values.begin(),
					m_values.end(),
					[](const chart_data_unit& a, const chart_data_unit& b)
					{
						return a.value < b.value;
					}
				)->value;
			}
			else
			{
				m_min = 0;
				m_max = 0;
			}
			updateMinMaxStr();
		}
		void updateMinMaxStr()
		{
			m_max_str = (m_max > 0 ? symbol_string(toStringP(m_max, m_value_labels_precision), gca().value_labels_color) : symbol_string("0", gca().value_labels_color)) + getFullWidthString(symbol_string(m_unit, gca().value_labels_color));
			m_min_str = (m_min < 0 ? symbol_string(toStringP(m_min, m_value_labels_precision), gca().value_labels_color) : symbol_string("0", gca().value_labels_color)) + getFullWidthString(symbol_string(m_unit, gca().value_labels_color));
		}

		void fill()
		{
			clear();
			makeBlank();

			unsigned short label_str_width = m_display_value_labels * (m_max_str.size() > m_min_str.size() ? m_max_str.size() : m_min_str.size());

			m_scroll.setContentLength(m_values.size() * m_distance - (m_distance-1));
			m_scroll.setVisibleContentLength(getSize().x - label_str_width);
			insertSurface(m_scroll, false);

			if (m_scroll.isNeeded()) { m_chart.setSizeInfo({ {label_str_width * -1,-1},{100,100} }); }
			else { m_chart.setSizeInfo({ {label_str_width * -1,0},{100,100} }); }
			updateSurfaceSize(m_chart);
			m_chart.makeTransparent();

			if (m_display_value_labels)
			{
				for (int i = 0; i < m_max_str.size(); i++)
				{
					setSymbolAt(m_max_str[i], { i,0 });
				}
				for (int i = 0; i < m_min_str.size(); i++)
				{
					int y_pos = std::max(0, m_chart.getSize().y - 1 - m_display_data_labels);
					setSymbolAt(m_min_str[i], { i,y_pos });
				}
			}

			float distance = (fabs(m_min - m_max));
			if (m_min > 0) { distance += m_min; }
			if (m_max < 0) { distance += fabs(m_max); }

			if (distance > 0)
			{
				int halves = (m_chart.getSize().y - m_display_data_labels) * 2;
				int p_halves = round(m_max / distance * halves) * (m_max>=0);

				int scroll_pos = m_scroll.getTopPosition();
				int x = m_distance *(scroll_pos % m_distance != 0) - scroll_pos % m_distance;

				for (int i = ceil(scroll_pos / (float)m_distance); (i < m_values.size() && x < m_chart.getSize().x); i++, x += m_distance)
				{
					int h = round(fabs(m_values[i].value) / distance * halves);

					auto isFull = [&](int y)
					{
						switch (m_values[i].value >= 0)
						{
						case true:
							return y >= p_halves - h && y < p_halves;
						case false:
							return y >= p_halves && y < p_halves + h;
						}
					};
						
					for (int y = 0; y < halves; y+=2)
					{
						if (isFull(y) && isFull(y + 1))
						{
							m_chart.setSymbolAt(gca().full, { x, y / 2 });
						}
						else if (isFull(y) && !isFull(y + 1))
						{
							m_chart.setSymbolAt(gca().upper_half, { x, y / 2 });
						}
						else if (!isFull(y) && isFull(y + 1))
						{
							m_chart.setSymbolAt(gca().lower_half, { x, y / 2 });
						}
					}	

					if (m_display_data_labels)
					{
						symbol_string fw_name = getFullWidthString(m_values[i].name);

						for (int j = 0; j < fw_name.size() && x + j < m_chart.getSize().x; j++)
						{
							m_chart.setSymbolAt(fw_name[j], { x + j, m_chart.getSize().y - 1 });
						}
					}
				}
			}
			insertSurface(m_chart);
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction(action_proxy proxy) override
		{
			if (m_redraw_needed)
			{
				fill();
				m_redraw_needed = false;
			}
		}

		void activationAction() override
		{
			m_scroll.activate();
			updateMinMaxStr();
			m_redraw_needed = true; 
		}
		void deactivationAction() override
		{
			m_scroll.deactivate();
			updateMinMaxStr();
			m_redraw_needed = true;
		}

		void setAppearanceAction() override 
		{
			m_scroll.setAppearance({active_appearance.chart_scroll_appearance_a, inactive_appearance.chart_scroll_appearance_a});
			updateMinMaxStr();
			m_redraw_needed = true; 
		}

	public:
		input::key_t& key_up = m_scroll.key_up;
		input::key_t& key_down = m_scroll.key_down;
		input::key_t& key_pgup = m_scroll.key_pgup;
		input::key_t& key_pgdn = m_scroll.key_pgdn;

		chart(surface_size size = surface_size()) : m_scroll({0,100}) 
		{
			m_scroll.setPositionInfo({ { 0,-1 }, { 0,100 } });
			m_chart.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::END, tui::POSITION::BEGIN} });

			setSizeInfo(size);
		}

		void setData(const std::vector<chart_data_unit>& values)
		{
			m_values = values;
			updateMinMax();
			m_redraw_needed = true;
		}
		std::vector<chart_data_unit> getData() const { return m_values; }

		void setDataAt(const chart_data_unit& data, unsigned int i) 
		{ 
			m_values[i] = data; 
			updateMinMax();
			m_redraw_needed = true;
		}
		chart_data_unit getDataAt(unsigned int i) const { return m_values[i]; }

		void removeDataAt(unsigned int i) 
		{ 
			m_values.erase(m_values.begin() + i); 
			updateMinMax();
			m_redraw_needed = true;
		}

		void insertDataAt(const chart_data_unit& data, unsigned int i)
		{
			m_values.insert(m_values.begin() + i, data);
			
			bool dst_ch = data.value > m_max || data.value < m_min;

			m_max = std::max(m_max, data.value);
			m_min = std::min(m_min, data.value);

			if (dst_ch) { updateMinMaxStr(); }

			m_redraw_needed = true;
		}

		void addData(const chart_data_unit& data, bool scroll_to = false)
		{
			m_values.push_back(data);

			bool dst_ch = data.value > m_max || data.value < m_min;

			m_max = std::max(m_max, data.value);
			m_min = std::min(m_min, data.value);

			if (dst_ch) { updateMinMaxStr(); }

			if (scroll_to) { goToBar(m_values.size()); }

			m_redraw_needed = true;
		}

		unsigned int size() const { return m_values.size(); }

		void goToBar(unsigned int line)
		{
			m_scroll.setTopPosition(line * m_distance);
			m_redraw_needed = true;
		}
		//returns index of first visible bar
		unsigned int getBar() const { return std::ceil(m_scroll.getTopPosition() / (float)m_distance); }

		void setDistance(unsigned int distance)
		{
			if (distance > 0) { m_distance = distance; }
			else { m_distance = 1; }
			m_redraw_needed = true;
		}
		unsigned int getDistance() const { return m_distance; }

		void displayValueLabels(bool display)
		{
			m_display_value_labels = display;
			m_redraw_needed = true;
		}
		bool isDisplayingValueLabels() const { return m_display_value_labels; }

		void setValueLabelsPrecision(int precision)
		{
			m_value_labels_precision = precision;
			m_redraw_needed = true;
		}
		int getValueLabelsPrecision() const { return m_value_labels_precision; }

		void setValueUnit(const std::string& unit)
		{
			m_unit = unit;
			updateMinMaxStr();
			m_redraw_needed = true;
		}
		std::string getValueUnit() const { return m_unit; }

		void displayDataLabels(bool display)
		{
			m_display_data_labels = display;
			m_redraw_needed = true;
		}
		bool isDisplayingDataLabels() const { return m_display_data_labels; }

		void update()
		{
			int old_scroll_handle_pos = m_scroll.getTopPosition();
			m_scroll.update();
			if (m_scroll.getTopPosition() != old_scroll_handle_pos)
			{
				m_redraw_needed = true;
			}
		}
	};
}