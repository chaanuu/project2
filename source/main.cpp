#include "TUI/tui.h"
#include "sell.h"

#include <cmath>

int main()
{
	Sell sell;
	sell.makeMenuList();

	//BOX
	tui::box main_box({ {0,0}, {100,100} });
	main_box.setTitle("POS project from B03");
	main_box.setTitlePosition(tui::POSITION::END);

	//TABS
	tui::tabs<tui::DIRECTION::HORIZONTAL> tabs({ 0,33 });
	tabs.setTabs({ "SELL", "LIST", "REPORT", "ADMIN" });
	tabs.key_next = tui::input::KEY::CTRL_RIGHT;
	tabs.key_prev = tui::input::KEY::CTRL_LEFT;
	tabs.activate();

	tui::symbol_string t_desc;
	t_desc += "next tab: ";
	t_desc << tui::COLOR::GREEN << tui::input::getKeyName(tabs.key_next);
	t_desc += ", prev tab: ";
	t_desc << tui::COLOR::GREEN << tui::input::getKeyName(tabs.key_prev);
	tui::surface tabs_keys(t_desc);
	tabs_keys.setPositionInfo({ {-1,0}, {0,0}, {tui::POSITION::END, tui::POSITION::END} });

	//TEXT2
	tui::text textA({ {0,0}, {20,20} });
	textA.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
	tui::symbol_string str2;
	for (char i = 1; i < 127; i++) { str2.push_back(i); }
	str2 = "In Construction....\n";
	textA.setText(str2);

	//INPUT_TEXT
	tui::input_text itxt({ {0,0}, {15,25} });
	itxt.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });

	//NAVIGATION GROUP
	tui::navigation_group tabs_nav({ &textA, &itxt, nullptr });
	tabs_nav.key_next = tui::input::KEY::CTRL_RIGHT;
	tabs_nav.key_prev = tui::input::KEY::CTRL_LEFT;
	tabs_nav.activate();

	tui::init();

	while (!tui::input::isKeyPressed(tui::input::KEY::ESC))
	{
		tui::output::clear();

		tui::output::draw(main_box);
		tui::output::draw(tabs);
		tui::output::draw(tabs_keys);
		tabs_nav.update();

		switch (tabs.getSelected())
		{
		case 0:
			sell.draw_UI();
			break;
		case 1:
			//tui::output::draw(textA);
			break;
		case 2:
			//tui::output::draw(list);
			break;
		case 3:
			//tui::output::draw(textA);
			break;
		}

		tui::output::display();
	}

	return 0;
}