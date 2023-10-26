#include "TUI/tui.h"
#include "menu.h"
#include "sell.h"
#include "filelog.h"
#include "customer.h"

#include <cmath>
#include <Windows.h>
#include <regex>

using namespace std;

bool IsPhoneNumberValid(const string& str) {
	std::regex pattern("^010-[0-9]{4}-[0-9]{4}$");
	return std::regex_match(str, pattern);
}

int main()
{
	read_MenuDB();
	DatabaseManager customerManager;
	Sell sell;
	Sleep(500);

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

	// variables for SELL tab
	unsigned int sell_key = 0;
	bool wrongFormat = false;
	tui::input_text input_SELL({ {20,2}, {0,0} });
	input_SELL.setPositionInfo({ {0,0}, {10,50} });
	string customerHP = "/0";
	tui::symbol_string input_txt;
	// ..

	tui::init();

	while (!tui::input::isKeyPressed(tui::input::KEY::ESC))
	{
		tui::output::clear();

		tui::output::draw(main_box);
		tui::output::draw(tabs);
		tui::output::draw(tabs_keys);

		switch (tabs.getSelected())
		{
		case 0: //주문 초기화면
			switch (sell_key) {
			case 0:
				sell.drawUI0();

				if (tui::input::isKeyPressed(tui::input::KEY::INS)) {
					sell.push();
				}
				if (tui::input::isKeyPressed(tui::input::KEY::DEL)) {
					sell.pull();
				}
				if (tui::input::isKeyPressed(tui::input::KEY::END) && sell.isOrderExist()) {
					sell_key = 1;
				}
				break;

			case 1: // END눌렸을때
				sell.drawUI1(wrongFormat);
				tui::output::draw(input_SELL);
				input_SELL.activate();

				if (tui::input::isKeyPressed(tui::input::KEY::END)) {
					input_txt = input_SELL.getText();
					customerHP = input_txt.getStdString();

					if (customerHP == "") {// 주문 완료
						sell_key = 0;
						wrongFormat = false;
						struct OrderInfo thisorder = sell.finish(customerHP); // USE THIS STRUCT IF NEEDED
						filelog(thisorder);
					}
					else if (IsPhoneNumberValid(customerHP)) {
						wrongFormat = false;
						// 번호 형식이 맞는 경우
						// 쿠폰 확인 (DB 조회) 후 사용 여부 묻기 (case 2)
						sell_key = 2;
					}
					else { // 번호 형식이 아님 -> 번호 재입력 창
						wrongFormat = true;
					}
					input_SELL.setText("");
				}
				break;
			
			case 2: 
				break;
			}
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
