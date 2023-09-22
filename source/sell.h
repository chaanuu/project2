#pragma once

#include "TUI/tui.h"

#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include <string>
#include <stack>

using namespace std;

class Sell_UI {
protected:
	tui::box orderBox;
	tui::text textInBox;
	tui::list menuList;
	tui::symbol_string orderBoxText;

public:
	Sell_UI() {
		//Make Box for Order List
		orderBox.setAppearance(tui::box_appearance::thin_line);
		orderBox.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(30, 90) });
		orderBox.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(68, 4) });
		orderBox.setTitle("Order Box");
		orderBox.setTitlePosition(tui::POSITION::CENTER);

		//Text in order Box
		textInBox.setSizeInfo({ {0,0}, {28,85} });
		textInBox.setPositionInfo({ {0,0}, {70,6} });
		orderBoxText = "List will be displayed here";
		textInBox.setText(orderBoxText);

		//LIST default
		menuList.setSizeInfo({ {0,0}, {30,90} });
		menuList.setPositionInfo({ {0,0}, {3,6} });
		//menuList.setEntries({			});
	}
	
};

class Sell : Sell_UI {
private:
	fstream menuDB;
	map<int, tuple<string, int>> menuMap; // id : key, value : tuple type (string, int)

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
			
			tui::symbol_string menuAndPrice = name + "   " + to_string(price);
			tui::list_entry child_entry(menuAndPrice, tui::CHECK_STATE::NOT_CHECKED, nullptr, nullptr, nullptr);
			
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

	void draw_UI() {
		tui::output::draw(orderBox);
		tui::output::draw(menuList);
		tui::output::draw(textInBox);

		menuList.activate();
	}

	~Sell() {
		menuMap.clear();
	}
};