//this file contains appearance that is inherited by widgets
#pragma once
#include "tui_text_utils.h"

namespace tui
{
	struct appearance
	{
	protected:
		template<typename T>
		void setElement(T& elem_to_set, T elem)
		{
			elem_to_set = elem;
			setAppearanceAction();
		}
		virtual void setAppearanceAction() {}
	public:
		virtual ~appearance() {}
		virtual void setColor(color Color) = 0;
	};
}