#pragma once

#include "TUI/tui.h"

#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include <string>

using namespace std;

class Sell_UI {
protected:
	tui::box orderBox;
	tui::text textInBox;
	tui::text guide;
	tui::list menuList;
	tui::symbol_string orderBoxText;
	tui::symbol_string orderBoxText_whenEmpty;
	tui::symbol_string guideText;

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
		guide.setPositionInfo({ {0,0}, {3,86} });
		guide.setText(guideText);

		//Make Box for Order List
		orderBox.setAppearance(tui::box_appearance::thin_line);
		orderBox.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(30, 90) });
		orderBox.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(68, 4) });
		orderBox.setTitle("Order Box");
		orderBox.setTitlePosition(tui::POSITION::CENTER);

		//Text in order Box
		textInBox.setSizeInfo({ {0,0}, {28,85} });
		textInBox.setPositionInfo({ {0,1}, {70,6} });
		orderBoxText_whenEmpty << tui::COLOR::YELLOW << "Order Box is Empty Now";
		textInBox.setText(orderBoxText_whenEmpty);

		//LIST default
		menuList.setSizeInfo({ {0,0}, {30,85} });
		menuList.setPositionInfo({ {0,1}, {3,6} });
		//menuList.setEntries({			});
	}
};

class Sell : Sell_UI {
private:
	fstream menuDB;
	map<int, tuple<string, int>> menuMap; // id : key, value : tuple type (string, int)
	map<int, unsigned int> orderBoxMap;

public:
	Sell() {
		menuDB.open("source/database/menuDB.CSV", ios::in);
		if (!menuDB.is_open()) {
			// menuDB file could not open
			printf("menuDB 열기 실패 \n");
			exit(0);
		}

		while (!menuDB.eof()) {
			string id_tmp, price_tmp, name;

			if (getline(menuDB, id_tmp, ',') && getline(menuDB, name, ',') && getline(menuDB, price_tmp, ',')) {
				int id = stoi(id_tmp);
				int price = stoi(price_tmp);

				tuple<string, int> menuInfo = make_tuple(name, price);
				menuMap.insert({ id, menuInfo });
			}

		}
		menuDB.close();
	}

	// Function to reload menuDB if needed.
	void update_menuMap() {
		menuMap.clear();

		menuDB.open("menuDB.csv", ios::in);

		while (!menuDB.eof()) {
			string id_tmp, price_tmp, name;

			getline(menuDB, id_tmp, ',');
			getline(menuDB, name, ',');
			getline(menuDB, price_tmp, ',');

			int id = stoi(id_tmp);
			int price = stoi(price_tmp);

			tuple<string, int> menuInfo = make_tuple(name, price);
			menuMap.insert({ id, menuInfo });
		}
		menuDB.close();
	}

	void makeMenuList() {
		for (auto i = menuMap.begin(); i != menuMap.end(); i++) {
			string name = get<0>(i->second);
			int price = get<1>(i->second);
			int category = i->first / 1000;

			tui::symbol_string menuAndPrice = name + "  " + to_string(price);
			tui::list_entry child_entry(menuAndPrice, tui::CHECK_STATE::NONCHECKABLE, nullptr, nullptr, nullptr);
			child_entry.setEntryID(i->first);

			menuList.addEntry(child_entry);
			/*
			tui::list_entry parent_entry = menuList.getEntries()[category - 1];

			if (category >= 1 && category <= 3) {
				if (parent_entry.nested_entries.empty()) {
					parent_entry.nested_entries = { child_entry };

				}
				else {
					parent_entry.nested_entries.push_back(child_entry);
				}
			}
			else {
				continue;
			}
			*/
			menuList.update();

		}
	}

	void updateOrderBox() {
		bool isFirstIndex = true;
		int totalPrice = 0;

		if (orderBoxMap.empty()) {
			textInBox.setText(orderBoxText_whenEmpty);
		}
		else {
			for (auto iter = orderBoxMap.begin(); iter != orderBoxMap.end(); iter++) {
				string name = get<0>(menuMap.find(iter->first)->second);
				tui::symbol_string currentLine = name + "  " + to_string(iter->second) + "\n";
				totalPrice += get<1>(menuMap.find(iter->first)->second) * iter->second;
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

	void addInOrderBox() {
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
			updateOrderBox();
		}
	}

	void deleteInOrderBox() {
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
		updateOrderBox();
	}

	void draw_UI() {
		tui::output::draw(orderBox);
		tui::output::draw(menuList);
		tui::output::draw(textInBox);
		tui::output::draw(guide);

		menuList.activate();
	}

	~Sell() {
		menuMap.clear();
	}
};