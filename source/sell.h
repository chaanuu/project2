#pragma once

#include "TUI/tui.h"
#include "menu.h"
#include "queue.h"

#include <map>
#include <tuple>
#include <string>

using namespace std;

struct OrderInfo {
	int number;
	map<int, unsigned int> information;
	string customerHP;
	bool coupon_used;
	
	OrderInfo(int orderNum, map<int, unsigned int> Info, string customer, bool is_coupon_used) {
		number = orderNum;
		information = Info;
		customerHP = customer;
		coupon_used = is_coupon_used;
	}
	~OrderInfo() {	}
};

class Sell_UI {
protected:
	tui::box orderBox; // 주문 목록 담는 사각형

	tui::list menuList; // 메뉴 목록

	tui::text textInBox; // orderBox 안에들어가는 텍스트
	tui::text guide; // 페이지 가이드 텍스트
	tui::text guide_end; // 페이지 가이드 텍스트
	tui::text guide_wrong_format; //틀린 형식 입력되었을때
	tui::text guide_succesful_order; //주문 성공 시 출력
	tui::text guide_use_coupon;

	tui::symbol_string orderBoxText;
	tui::symbol_string orderBoxText_whenEmpty;
	tui::symbol_string guideText;
	tui::symbol_string guideText_end;
	tui::symbol_string wrong_format;
	// tui::symbol_string succesful_order;
	tui::symbol_string guideText_use_coupon;

	unsigned int tmp_coupon = 0;
public:
	Sell_UI() {
		//Text to guide this Tab
		guideText << tui::COLOR::LIGHTBLUE << "INSERT";
		guideText += " to add to order, ";
		guideText << tui::COLOR::LIGHTBLUE << "DELETE";
		guideText += " to delete from order";
		guideText << tui::COLOR::LIGHTRED << "\nEND";
		guideText += " to finish order";
		guide.setSizeInfo({ {0,2}, {50,1} });
		guide.setPositionInfo({ {0,0}, {5,82} });
		guide.setText(guideText);

		//Text to guide to end
		guideText_end << tui::COLOR::LIGHTBLUE << "TYPE PHONE NUMBER";
		guideText_end += " to finish order (010-XXXX-XXXX)\nAnd press";
		guideText_end << tui::COLOR::LIGHTRED << " END";
		guideText_end += "\nor just press END to SKIP";
		guide_end.setSizeInfo({ {0,3}, {50,1} });
		guide_end.setPositionInfo({ {0,0}, {10,35} });
		guide_end.setText(guideText_end);

		//Text to guide to wrong format input
		wrong_format << tui::COLOR::LIGHTRED << "WRONG FORMAT !";
		guide_wrong_format.setSizeInfo({ {0,2}, {50,1} });
		guide_wrong_format.setPositionInfo({ {0,0}, {10,30} });
		guide_wrong_format.setText(wrong_format);

		//Text to guide to use coupon
		guideText_use_coupon = "You have ";
		guideText_use_coupon << tui::COLOR::LIGHTBLUE << to_string(tmp_coupon) << " coupons";
		guideText_use_coupon += "\nPress 'Y' to use, 'N' to not use.";
		guide_use_coupon.setSizeInfo({ {0,5}, {30,0} });
		guide_use_coupon.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
		guide_use_coupon.setText(guideText_use_coupon);
		
		/*
		//Text to show order process completed
		succesful_order << tui::COLOR::LIGHTBLUE << "Order processed scuccesfully!";
		guide_succesful_order.setSizeInfo({ { 1,20 }, {0, 0} });
		guide_succesful_order.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
		guide_succesful_order.setText(succesful_order);
		*/

		//Make Box for Order List
		orderBox.setAppearance(tui::box_appearance::thin_line);
		orderBox.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(30, 90) });
		orderBox.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(68, 4) });
		orderBox.setTitle("Order Box");
		orderBox.setTitlePosition(tui::POSITION::CENTER);

		//Text in order Box
		textInBox.setSizeInfo({ {0,0}, {28,75} });
		textInBox.setPositionInfo({ {0,1}, {70,6} });
		orderBoxText_whenEmpty << tui::COLOR::YELLOW << "Order Box is Empty Now";
		textInBox.setText(orderBoxText_whenEmpty);

		//LIST default
		menuList.setSizeInfo({ {0,0}, {30,75} });
		menuList.setPositionInfo({ {0,1}, {3,6} });
	}
};

class Sell : Sell_UI {
private:
	map<int, unsigned int> orderBoxMap;
	int orderNum;

public:
	Sell() {
		orderNum = 1;
		makeMenuList();
	}

	int getorderNum() {
		return orderNum;
	}

	void addorderNum() {
		orderNum++;
	}

	void drawUI0() {
		tui::output::draw(orderBox);
		tui::output::draw(menuList);
		tui::output::draw(textInBox);
		tui::output::draw(guide);

		menuList.activate();
	}

	void drawUI1(bool wrongFormat) {
		tui::output::draw(orderBox);
		tui::output::draw(textInBox);
		tui::output::draw(guide_end);
		if (wrongFormat) tui::output::draw(guide_wrong_format);

	}

	void drawUI2(unsigned int coupons) {
		guideText_use_coupon = "You have ";
		guideText_use_coupon << tui::COLOR::LIGHTBLUE << to_string(coupons) << " coupons";
		guideText_use_coupon += "\nPress 'Y' to use, 'N' to not use.";
		guide_use_coupon.setText(guideText_use_coupon);
		tui::output::draw(guide_use_coupon);
	}

	void makeMenuList() { 
		for (auto i = menuMap.begin(); i != menuMap.end(); i++) {
			string name = getMenuName(i->first);
			int price = getMenuPrice(i->first);
			tui::symbol_string menuAndPrice = name + "  " + to_string(price);
			tui::list_entry entry(menuAndPrice, tui::CHECK_STATE::NONCHECKABLE, nullptr, nullptr, nullptr);
			entry.setEntryID(i->first);
			menuList.addEntry(entry);
		}
		menuList.update();
	}

	void update() {
		bool isFirstIndex = true;
		int totalPrice = 0;

		if (orderBoxMap.empty()) {
			textInBox.setText(orderBoxText_whenEmpty);
		}
		else {
			for (auto iter = orderBoxMap.begin(); iter != orderBoxMap.end(); iter++) {
				string name = getMenuName(iter->first);
				tui::symbol_string currentLine = name + "  " + to_string(iter->second) + "\n";
				totalPrice += getMenuPrice(iter->first) * iter->second;
				if (isFirstIndex) {
					orderBoxText = currentLine;
					isFirstIndex = false;
				}
				else {
					orderBoxText += currentLine;
				}
			}
			tui::symbol_string currentLine = "\ntotal : " + to_string(totalPrice) + "\n";
			orderBoxText << tui::COLOR::YELLOW << currentLine;
			textInBox.setText(orderBoxText);
		}
	}

	void push() {
		int position = menuList.getCurrentPosition();
		tui::list_entry currentEntry = this->menuList.getEntryAt(position);
		int currentID = currentEntry.getMenuID();
		if ((orderBoxMap.size() == 10) && !(orderBoxMap.find(currentID) != orderBoxMap.end())) {
			return;
		}
		else {
			if (orderBoxMap.find(currentID) != orderBoxMap.end()) {
				orderBoxMap[currentID]++;
			}
			else {
				orderBoxMap.insert({ currentID, 1 });
			}
			update();
		}
	}

	void pull() {
		int position = menuList.getCurrentPosition();
		tui::list_entry currentEntry = this->menuList.getEntryAt(position);
		int currentID = currentEntry.getMenuID();
		if (orderBoxMap.find(currentID) != orderBoxMap.end()) {
			if (orderBoxMap[currentID] > 1) {
				orderBoxMap[currentID]--;
			}
			else {
				orderBoxMap.erase(currentID);
			}
		}
		update();
	}

	OrderInfo finish(string customer, bool is_coupon_used = false) {
		int currentOrderNum = getorderNum();
		struct OrderInfo thisorder = OrderInfo(currentOrderNum, orderBoxMap, customer, is_coupon_used);

		addorderNum();
		clear();
		update();

		return thisorder;
	}

	void clear() {
		orderBoxMap.clear();
	}

	bool isOrderExist() {
		return !orderBoxMap.empty();
	}
};