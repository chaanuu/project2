#include "TUI/tui.h"
#include "menu.h"
#include "sell.h"
#include "filelog.h"
#include "report.h"
#include "admin.h"
#include "queue.h"

#include <cmath>
#include <Windows.h>
#include <regex>

using namespace std;

bool IsPhoneNumberValid(const string& str) {
	std::regex pattern("^010-[0-9]{4}-[0-9]{4}$");
	return std::regex_match(str, pattern);
}

void process_order(string customerHP, Sell& sell, Report_UI& reportUI, Queue& queue, bool isCouponUsed) {
	struct OrderInfo thisorder = sell.finish(customerHP, isCouponUsed); // USE THIS STRUCT IF NEEDED
	queue.addOrder(thisorder);
	string log_filename = filelog(thisorder);
	Report report = Report(log_filename);
	report.editReport(to_string(thisorder.number));
	reportUI.makeList();
}

int main()
{
	read_MenuDB();
	Sell sell;
	Queue queue;
	//Sleep(500);

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
	unsigned int coupons = 0;
	bool wrongFormat = false;
	tui::input_text input_SELL({ {20,2}, {0,0} });
	input_SELL.setPositionInfo({ {0,0}, {10,50} });
	string customerHP = "/0";
	tui::symbol_string input_txt;

	unsigned int queue_key = 0;

	Report_UI reportUI;

	unsigned int admin_key = 0;
	unsigned int tab_key = 0;
	admin Admin;
	string password_IP = "";
	tui::input_text input_ADMIN({ {20,2}, {0,0} });
	input_ADMIN.setPositionInfo({ {0,0}, {10,50} });
	bool wrongPassword = false;
	bool isValidPW = true;
	string date_admin;
	regex pattern(R"(^[0-9a-zA-Z]*$)");


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

					if (customerHP == "") {// 사용자HP미입력, 주문 완료
						sell_key = 0;
						wrongFormat = false;
						process_order(customerHP, sell, reportUI, queue, false);
					}
					else if (IsPhoneNumberValid(customerHP)) {
						wrongFormat = false;
						coupons = 2; //여기를 쿠폰확인 함수로 수정
						if (coupons == 0) { // 쿠폰없음, 주문완료
							sell_key = 0;
							wrongFormat = false;
							process_order(customerHP, sell, reportUI,queue, false);
						}
						else { 
							sell_key = 2;
						}
					}
					else { // 번호 형식이 아님 -> 번호 재입력 창
						wrongFormat = true;
					}
					input_SELL.setText("");
				}
				break;

			case 2: //쿠폰 사용 여부 확인하기
				sell.drawUI2(coupons);
				if (tui::input::isKeyPressed('Y') || tui::input::isKeyPressed('y')) { //쿠폰사용, 주문완료
					sell_key = 0;
					process_order(customerHP, sell, reportUI,queue, true);
				}
				if (tui::input::isKeyPressed('N') || tui::input::isKeyPressed('n')) { //쿠폰미사용, 주문완료
					sell_key = 0;
					process_order(customerHP, sell, reportUI,queue, false);
				}
				break;
			}

		case 1:
			queue.drawUI();
			queue.printQueue();

			switch (queue_key) {
			case 0: // 기본 모드
				if (tui::input::isKeyPressed(tui::input::KEY::DEL)) {
					queue_key = 1; // 삭제 모드로 전환
				}
				break;

			case 1: // 삭제 모드
				if (tui::input::isKeyPressed(tui::input::KEY::F1) ||
					tui::input::isKeyPressed(tui::input::KEY::F2) ||
					tui::input::isKeyPressed(tui::input::KEY::F3) ||
					tui::input::isKeyPressed(tui::input::KEY::F4) ||
					tui::input::isKeyPressed(tui::input::KEY::F5)) {
					queue.removeQueue();  // Queue 클래스 내부에서 키 검사 및 삭제 수행
					queue_key = 0;  // 다시 기본 모드로 전환
				}
				break;
			}
			break;
		case 2:
			reportUI.drawUI();
			break;
		case 3:
			switch (tab_key) {
			case 0:
				Admin.drawA2(wrongPassword);
				tui::output::draw(input_ADMIN);
				input_ADMIN.activate();
				if (tui::input::isKeyPressed(tui::input::END)) {
					tui::symbol_string temp_string = input_ADMIN.getText();
					password_IP = temp_string.getStdString();
					// 비밀번호 검사
					if (Admin.checkPassword(password_IP)) {
						tab_key = 1;
						wrongPassword = false;
					}
					else {
						wrongPassword = true;
					}
					input_ADMIN.setText("");
				}
				break;
			case 1:
				Admin.selectMenu();
				if (tui::input::isKeyPressed(tui::input::KEY::INS)) {
					tab_key = 2;
				}
				if (tui::input::isKeyPressed(tui::input::KEY::DEL)) {
					tab_key = 3;
				}
				if (tui::input::isKeyPressed(tui::input::KEY::ENTER)) {
					tabs.setSelected(0);
				}
				break;
			case 2:
				Admin.draw_clear();
				tui::output::draw(input_ADMIN);
				input_ADMIN.activate();
				if (tui::input::isKeyPressed(tui::input::END)) {
					tui::symbol_string temp_string = input_ADMIN.getText();
					date_admin = temp_string.getStdString();
					string filename = date_admin + ".csv";
					Admin.clear(filename);
					input_ADMIN.setText("");
					tab_key = 1;
				}
				break;
			case 3: // change pw
				Admin.drawA3(wrongPassword);
				tui::output::draw(input_ADMIN);
				input_ADMIN.activate();
				if (tui::input::isKeyPressed(tui::input::END)) {
					tui::symbol_string temp_string = input_ADMIN.getText();
					password_IP = temp_string.getStdString();
					// 비밀번호 검사
					if (Admin.checkPassword(password_IP)) {
						tab_key = 4;
						wrongPassword = false;
					}
					else {
						wrongPassword = true;
					}
					input_ADMIN.setText("");
				}
				break;
			case 4: // new pw
				Admin.drawA4(isValidPW);
				tui::output::draw(input_ADMIN);
				input_ADMIN.activate();
				if (tui::input::isKeyPressed(tui::input::END)) {
					tui::symbol_string temp_string = input_ADMIN.getText();
					password_IP = temp_string.getStdString();
					// 비밀번호 검사
					if (regex_match(password_IP, pattern)) {
						tab_key = 1;
						isValidPW = true;
					}
					else {
						isValidPW = false;
					}
					input_ADMIN.setText("");
				}
				break;
			}

			break;
		}
		
		tui::output::display();
	}
	return 0;
}
