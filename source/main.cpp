#include "TUI/tui.h"
#include "menu.h"
#include "sell.h"
#include "filelog.h"
#include "customer.h"

#include <cmath>
#include <Windows.h>

int main()
{
	read_MenuDB();
	DatabaseManager customerManager;
	Sell sell;
	Sleep(2000);

	//BOX
	tui::box main_box({ {0,0}, {100,100} });
	main_box.setTitle("POS project from B03");
	main_box.setTitlePosition(tui::POSITION::END);

	//TABS
	tui::tabs<tui::DIRECTION::HORIZONTAL> tabs({ 0,33 });
	tabs.setTabs({ "SELL", "QUEUE", "REPORT", "ADMIN" });
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

	tui::init();

	while (!tui::input::isKeyPressed(tui::input::KEY::ESC))
	{
		tui::output::clear();

		tui::output::draw(main_box);
		tui::output::draw(tabs);
		tui::output::draw(tabs_keys);

		switch (tabs.getSelected())
		{
		case 0:
			sell.drawUI();
			
			if (tui::input::isKeyPressed(tui::input::KEY::INS)) {
				sell.push();
			}
			if (tui::input::isKeyPressed(tui::input::KEY::DEL)) {
				sell.pull();
			}
			if (tui::input::isKeyPressed(tui::input::KEY::END)) {
				struct OrderInfo thisorder = sell.finish(); // USE THIS STRUCT IF NEEDED
				filelog(thisorder);
				//...
			}
			break;
		case 1:
			//queue.draw_UI();
    		//queue.printOrders();
        				
			break;
		case 2:
			break;
		case 3:
			break;
		}

		tui::output::display();
	}
	customerManager.~DatabaseManager();
	return 0;
}
