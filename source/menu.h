#pragma once

#include <map>
#include <string>
#include <iostream>
#include <fstream>

#define MENUDB_PATH "../../source/database/menuDB.CSV"

using namespace std;

/*
메뉴 이름 가져오기 : getMenuName(id)
메뉴 가격 가져오기 : getMenuPrice(id) 
*/
map<int, tuple<string, int>> menuMap; 

void read_MenuDB() {
	fstream menuDB;

	menuDB.open(MENUDB_PATH, ios::in);
	if (!menuDB.is_open()) {
		// menuDB file could not open
		printf("menuDB 열기 실패 \n");
		exit(0);
	}
	else {
		printf("menuDB.db 열기 성공 \n");
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

string getMenuName(int id) {
	return get<0>(menuMap.find(id)->second);
}

int getMenuPrice(int id) {
	return get<1>(menuMap.find(id)->second);
}