/*this file contains following elements:
struct surface_size - describes size of surface
struct surface1D_size - describes size of surface1D<DIRECTION>
struct surface_position - describes position of surface
struct anchor_position - describes position of anchor
struct surface - surface, could be viewed as array of symbols
struct surface1D<DIRECTION> - acts as an overlay on top of surface
enums: POSITION, SIDE, DIRECTION
*/
#pragma once
#include "tui_vec2.h"
#include "tui_text_utils.h"

#include <vector>
#include <cmath>
#include <functional>

namespace tui
{
	struct surface_size
	{
		vec2i fixed;
		vec2f percentage;

		surface_size(vec2i fixed = { 0,0 }, vec2f percentage = { 0,0 }) : fixed(fixed), percentage(percentage) {}
	};

	struct surface1D_size
	{
		int fixed;
		float percentage;

		surface1D_size(int fixed = 0, float percentage = 0) : fixed(fixed), percentage(percentage) {}
	};

	namespace POSITION
	{
		enum POSITION
		{
			BEGIN = 0, CENTER = 50, END = 100
		};
	}

	struct surface_position
	{
		vec2i offset;
		vec2f percentage_offset;
		vec2f relative;

		surface_position(vec2i offset = { 0,0 }, vec2f percentage_offset = { 0,0 }, vec2f relative = { POSITION::BEGIN, POSITION::BEGIN })
			: offset(offset), percentage_offset(percentage_offset), relative(relative) {}
	};

	enum class SIDE
	{
		TOP, BOTTOM, LEFT, RIGHT
	};

	struct anchor_position
	{
		SIDE side;
		float position;
		vec2i offset;

		anchor_position(SIDE side = SIDE::RIGHT, float position = POSITION::CENTER, vec2i offset = { 0,0 }) 
			: side(side), position(position), offset(offset) {}
	};

	struct surface
	{
	private:
		std::vector<symbol> m_symbols;
		unsigned int m_width = 1;
		surface_position m_position_info;
		vec2i m_position;
		vec2i m_global_position;
		surface_size m_size_info;

		surface* m_anchor = nullptr;
		anchor_position m_anchor_position_info;

		std::function<vec2i()> m_custom_size = nullptr;
		std::function<vec2i()> m_custom_position = nullptr;

		symbol m_clear_symbol = { U'\0', color(), COLOR_TRANSPARENCY::BG_FG };

		bool m_resized;

		void resize(vec2i size)
		{
			m_resized = size != getSize();

			if (m_resized)
			{
				vec2i new_size = size;
				if (size.x < 1) { new_size.x = 1; }
				if (size.y < 1) { new_size.y = 1; }

				m_symbols.resize(new_size.x * new_size.y);
				m_width = new_size.x;

				clear();
				resizeAction();
			}
		}

		void setString(const symbol_string& str)
		{
			symbol_string s = getFullWidthString(str);

			setSizeInfo({ {(int)s.size(),1},{0,0} });

			for (int i = 0; i < s.size(); i++)
			{
				setSymbolAt(s[i], { i,0 });
			}
		}

		template<typename T>
		struct array_proxy
		{
			friend class surface;
		private:
			T* surf;
			size_t x;
			array_proxy(T* s, size_t x) : surf(s), x(x) {}
		public:
			symbol& operator[](size_t y)
			{
				return surf->m_symbols[surf->m_width * y + x];
			}
			const symbol& operator[](size_t y) const
			{
				return surf->m_symbols[surf->m_width * y + x];
			}
		};

		template<typename T>
		struct property_override
		{
			friend struct surface;
		private:
			T value;
			bool use;
		public:
			property_override() : use(false) {}
			property_override(T value) : value(value), use(true) {}
		};
	public:
		struct color_override
		{
			friend struct surface;
		private:
			color value;
			int8_t foreground;
			bool use;
		public:
			color_override() : use(false) {}
			color_override(color value) : value(value), foreground(-1), use(true) {}
			color_override(rgb c, bool foreground) : foreground(foreground), use(true)
			{
				if (foreground) { value.foreground = c; }
				else { value.background = c; }
			}
		};
		using color_transparency_override = property_override<COLOR_TRANSPARENCY>;

	protected:
		struct action_proxy
		{
		private:
			surface* surf;
		public:
			color_override c_override;
			color_transparency_override c_t_override;

			action_proxy(surface* surf, color_override c_o, color_transparency_override c_t_o) 
				: surf(surf), c_override(c_o), c_t_override(c_t_o) {}

			vec2i getSize() const { return surf->getSize(); }
			void updateSurfaceSize(surface& s) { surf->updateSurfaceSize(s); }
			void updateSurfacePosition(surface& s) { surf->updateSurfacePosition(s); }
			void insertSurface(surface& s, bool update = true) 
			{
				surf->insertSurface(s, update);
			}
			void insertSurface(surface& s, color_override c_o, bool update = true)
			{
				surf->insertSurface(s, c_o, color_transparency_override(), update);
			}
			void insertSurface(surface& s, color_transparency_override c_t_o, bool update = true)
			{
				surf->insertSurface(s, color_override(), c_t_o, update);
			}
			void insertSurface(surface& s, color_override c_o, color_transparency_override c_t_o, bool update = true)
			{
				surf->insertSurface(s, c_o, c_t_o, update);
			}

		};
		virtual void resizeAction() {}
		virtual void updateAction() {}
		virtual void drawAction(action_proxy proxy) {}

		void clear()
		{
			fill(m_clear_symbol);
		}
	public:
		surface(surface_size size = {{ 1,1 }, { 0,0 }})
		{
			setSizeInfo(size);
		}
		surface(const symbol_string& str)
		{
			setString(str);
		}
		virtual ~surface() {}

		surface& operator=(const symbol_string& str)
		{
			setString(str);
			return *this;
		}

		array_proxy<surface> operator[](size_t x)
		{
			return array_proxy<surface>(this, x);
		}
		const array_proxy<const surface> operator[](size_t x) const
		{
			return array_proxy<const surface>(this, x);
		}

		bool isResized() const { return m_resized; }

		void setSymbolAt(const symbol& character, vec2i position) { m_symbols[position.y * m_width + position.x] = character; }
		symbol getSymbolAt(vec2i position) const { return m_symbols[position.y * m_width + position.x]; }

		void move(vec2i offset)
		{
			m_position_info.offset += offset;
		}

		void setPositionInfo(surface_position pos) { m_position_info = pos; }
		surface_position getPositionInfo() const { return m_position_info; }

		vec2i getPosition() const { return m_position; }
		vec2i getGlobalPosition() const { return m_global_position; }

		void setAnchor(surface* surf) 
		{
			if (surf != this)
			{
				m_anchor = surf;
			}
		}
		surface* getAnchor() const { return m_anchor; }

		void setAnchorPositionInfo(anchor_position anchor_pos) { m_anchor_position_info = anchor_pos; }
		anchor_position getAnchorPositionInfo() const { return m_anchor_position_info; }

		void setSizeInfo(surface_size size, bool update_fixed = true)
		{
			m_size_info = size;
			if (!m_custom_size)
			{
				if (size.percentage == vec2f(0.f, 0.f) && update_fixed)
				{
					resize(size.fixed);
				}
			}
		}
		surface_size getSizeInfo() const { return m_size_info; }

		vec2i getSize() const { return vec2i(m_width, m_symbols.size() / m_width); }

		void setSizeFunction(std::function<vec2i()> f_size) { m_custom_size = f_size; }
		std::function<vec2i()> getSizeFunction() const { return m_custom_size; }

		void setPositionFunction(std::function<vec2i()> f_position) { m_custom_position = f_position; }
		std::function<vec2i()> getPositionFunction() const { return m_custom_position; }

		void updateSurfaceSize(surface& surf) const
		{
			if (&surf != this)
			{	
				if (!surf.m_custom_size)
				{
					vec2i int_size = surf.getSizeInfo().fixed;
					vec2f perc_size = surf.getSizeInfo().percentage;

					int x = std::round((perc_size.x / 100.f) * getSize().x + int_size.x);
					int y = std::round((perc_size.y / 100.f) * getSize().y + int_size.y);

					surf.resize({ x,y });
				}
				else
				{
					surf.resize(surf.m_custom_size());
				}
			}
		}

		void updateSurfacePosition(surface& surf) const
		{
			if (&surf != this)
			{
				vec2f f_origin;
				vec2i i_origin;

				if (!surf.m_custom_position)
				{
					switch (surf.m_anchor == nullptr)
					{
					case true:
						f_origin.x = getSize().x * (surf.getPositionInfo().relative.x / 100.f) - surf.getSize().x * (surf.getPositionInfo().relative.x / 100.f) + surf.getPositionInfo().percentage_offset.x * getSize().x / 100.f;
						i_origin.x = surf.getPositionInfo().offset.x;		

						f_origin.y = getSize().y * (surf.getPositionInfo().relative.y / 100.f) - surf.getSize().y * (surf.getPositionInfo().relative.y / 100.f) + surf.getPositionInfo().percentage_offset.y * getSize().y / 100.f;
						i_origin.y = surf.getPositionInfo().offset.y;					
						break;

					case false:
						i_origin = surf.m_anchor->getPosition();

						switch (surf.m_anchor_position_info.side)
						{
						case SIDE::TOP:
						case SIDE::BOTTOM:
							f_origin.x += surf.m_anchor->getSize().x * (surf.m_anchor_position_info.position / 100.f) - surf.getSize().x * (surf.m_anchor_position_info.position / 100.f);
							break;
						case SIDE::LEFT:
						case SIDE::RIGHT:
							f_origin.y += surf.m_anchor->getSize().y * (surf.m_anchor_position_info.position / 100.f) - surf.getSize().y * (surf.m_anchor_position_info.position / 100.f);
						}

						switch (surf.m_anchor_position_info.side)
						{
						case SIDE::TOP:
							i_origin.y -= surf.getSize().y;
							break;
						case SIDE::BOTTOM:
							i_origin.y += surf.m_anchor->getSize().y;
							break;
						case SIDE::LEFT:
							i_origin.x -= surf.getSize().x;
							break;
						case SIDE::RIGHT:
							i_origin.x += surf.m_anchor->getSize().x;
						}

						i_origin += surf.m_anchor_position_info.offset;
					}
					i_origin.x += std::round(f_origin.x);
					i_origin.y += std::round(f_origin.y);
				}
				else
				{
					i_origin = surf.m_custom_position();
				}
				

				surf.m_position = i_origin;
				surf.m_global_position = m_global_position + i_origin;
			}
		}

		/*similar to setSymbolAt(), but sets symbol with respect to transparency instead of just replacing it and allows use of overrides*/
		void insertSymbolAt(const symbol& sym, vec2i position)
		{
			insertSymbolAt(sym, position, color_override(), color_transparency_override());
		}
		void insertSymbolAt(const symbol& sym, vec2i position, color_override c_override)
		{
			insertSymbolAt(sym, position, c_override, color_transparency_override());
		}
		void insertSymbolAt(const symbol& sym, vec2i position, color_transparency_override c_t_override)
		{
			insertSymbolAt(sym, position, color_override(), c_t_override);
		}
		void insertSymbolAt(
			const symbol& sym,
			vec2i position,
			color_override c_override,
			color_transparency_override c_t_override
		) 
		{  
			if (sym[0] != 0)//fully transparent, ignore
			{
				color n_color;
				switch (c_override.use)
				{
				case true:
					switch (c_override.foreground)
					{
					case -1:
						n_color = c_override.value;
						break;
					case 0:
						n_color = color{ sym.getColor().foreground, c_override.value.background };
						break;
					case 1:
						n_color = color{ c_override.value.foreground, sym.getColor().background };
					}
					break;
				case false:
					n_color = sym.getColor();
				}

				color o_color = (*this)[position.x][position.y].getColor();

				COLOR_TRANSPARENCY n_transparency = c_t_override.use ? c_t_override.value : sym.getColorTransparency();
				uint8_t n_transparency_val = static_cast<uint8_t>(n_transparency);
				uint8_t o_transparency_val = static_cast<uint8_t>((*this)[position.x][position.y].getColorTransparency());

				switch (n_transparency)
				{
				case COLOR_TRANSPARENCY::NONE:
					break;
				case COLOR_TRANSPARENCY::BG:
					n_color.background = o_color.background;
					break;
				case COLOR_TRANSPARENCY::FG:
					n_color.foreground = o_color.foreground;
					break;
				case COLOR_TRANSPARENCY::BG_FG:
					n_color = o_color;
				}
				symbol s = sym;
				s.setColor(n_color);
				s.setColorTransparency(static_cast<COLOR_TRANSPARENCY>(n_transparency_val & o_transparency_val));

				setSymbolAt(s, position);
			}
		}


		void insertSurface(surface& surf, bool update = true)
		{
			insertSurface(surf, color_override(), color_transparency_override(), update);
		}
		void insertSurface(surface& surf, color_override c_override, bool update = true)
		{
			insertSurface(surf, c_override, color_transparency_override(), update);
		}
		void insertSurface(surface& surf, color_transparency_override c_t_override, bool update = true)
		{
			insertSurface(surf, color_override(), c_t_override, update);
		}
		void insertSurface(
			surface& surf,
			color_override c_override,
			color_transparency_override c_t_override,
			bool update = true
		)
		{
			if (&surf != this)
			{
				updateSurfaceSize(surf);
				updateSurfacePosition(surf);

				if (update) { surf.updateAction(); }
				surf.drawAction({ this, c_override, c_t_override });

				const vec2i origin = surf.m_position;
				for (int y = 0; y < surf.getSize().y; y++)
				{
					for (int x = 0; x < surf.getSize().x; x++)
					{
						if (origin.x + x >= 0
							&& origin.y + y >= 0
							&& origin.x + x < getSize().x
							&& origin.y + y < getSize().y)
						{
							insertSymbolAt(surf[x][y], origin+vec2i{ x,y }, c_override, c_t_override);
						}
					}
				}
			}
		}

		void fill(const symbol& Symbol)
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i] = Symbol; }
		}
		void makeTransparent() 
		{
			fill({U'\0', color(), COLOR_TRANSPARENCY::BG_FG});
		}
		void makeBlank() { fill(' '); }

		void fillColorTransparency(COLOR_TRANSPARENCY c_t)
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i].setColorTransparency(c_t); }
		}

		void fillColor(color c)
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i].setColor(c); }
		}

		void setClearSymbol(const symbol& sym) { m_clear_symbol = sym; }
		symbol getClearSymbol() const { return m_clear_symbol; }

		void invert()
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i].invert(); }
		}
	};

	enum class DIRECTION : bool
	{
		HORIZONTAL = 0,
		VERTICAL
	};
	
	template<DIRECTION direction>
	struct surface1D : surface
	{
		surface1D(surface1D_size size = { 1, 0 })
		{
			setSizeInfo(size);
		}

		void setSizeInfo(surface1D_size size)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSizeInfo({ {size.fixed, 1},{size.percentage, 0} });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSizeInfo({ {1, size.fixed},{0, size.percentage} });
			}
		}
		int getSize() const
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::getSize().x;
			case tui::DIRECTION::VERTICAL:
				return surface::getSize().y;
			}
		}
		void setSymbolAt(const symbol& character, int position)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSymbolAt(character, { position, 0 });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSymbolAt(character, { 0, position });
			}
		}
		symbol getSymbolAt(int position) const
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::getSymbolAt({ position, 0 });
			case tui::DIRECTION::VERTICAL:
				return surface::getSymbolAt({ 0, position });
			}
		}
		symbol& operator[](size_t i)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::operator[](i)[0];
			case tui::DIRECTION::VERTICAL:
				return surface::operator[](0)[i];
			}
		}
		const symbol& operator[](size_t i) const 
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::operator[](i)[0];
			case tui::DIRECTION::VERTICAL:
				return surface::operator[](0)[i];
			}
		}
		surface1D_size getSizeInfo() const
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface1D_size(surface::getSizeInfo().fixed.x, surface::getSizeInfo().percentage.x);
			case tui::DIRECTION::VERTICAL:
				return surface1D_size(surface::getSizeInfo().fixed.y, surface::getSizeInfo().percentage.y);;
			}
		}
	};
}