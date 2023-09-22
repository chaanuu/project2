#pragma once

#include "sell.h"

#include "TUI/tui.h"

#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include <string>
#include <stack>

using namespace std;

class Menu {
private:
	fstream menuDB;
	map<int, tuple<string, int>> menuMap; // id : key, value : tuple type (string, int)

public:
	Menu() {
		menuDB.open("menuDB.csv", ios::in);
		if (!menuDB.is_open()) {
			// menuDB file could not open
		}

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

	void makeMenuList(Sell_UI& sellUI) {
		for (auto i = menuMap.begin(); i != menuMap.end(); i++) {
			string name = get<0>(i->second);
			int price = get<1>(i->second);
			int category = i->first / 1000;

			tui::symbol_string menuAndPrice = name + "\t" + to_string(price);
			tui::list_entry child_entry(menuAndPrice, tui::CHECK_STATE::NOT_CHECKED, nullptr, nullptr, nullptr);

			switch(category) {
			case 1: /* iced */
				tui::list_entry & parent_entry = sellUI.menuList.getEntries()[0];
				parent_entry.nested_entries.push_back(child_entry);
				sellUI.menuList.update();
				break;
			case2: /* hot */
				tui::list_entry& parent_entry = sellUI.menuList.getEntries()[1];
				parent_entry.nested_entries.push_back(child_entry);
				sellUI.menuList.update();
				break;
			case3: /* Smoothie */
				tui::list_entry& parent_entry = sellUI.menuList.getEntries()[2];
				parent_entry.nested_entries.push_back(child_entry);
				sellUI.menuList.update();
				break;
			default:
				break;
			}
		}
	}

	~Menu() {
		menuMap.clear();
	}
};
